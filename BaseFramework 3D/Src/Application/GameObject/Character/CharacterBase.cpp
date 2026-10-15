#include "CharacterBase.h"

#include"../../System/CollisionManager/CollisionManager.h"
#include"../../System/TimeManager/TimeManager.h"

#include"../../Editor/EditorManager.h"
#include"../../Editor/InspectorWidgets.h"

#include"../Gimmick/AreaVolume/AreaVolume.h"


CharacterBase::CharacterBase()
{

}

CharacterBase::~CharacterBase()
{
	Release();
}

// 初期化
void CharacterBase::Init()
{}

void CharacterBase::PreUpdate()
{
	// 移動開始前の位置をセット
	SetPrevPos(GetPos());
	// 前フレームの移動量をクリア
	ClearPendingMove(Math::Vector3::Zero);

	// デルタタイムをセット
	SetDeltaTime(TimeManager::Instance().GetDeltaTime());

	UpdateCurrentAreaID();

}

// 更新
void CharacterBase::Update()
{}

void CharacterBase::PostUpdate()
{
	UpdateOverlay();

	UpdateMatrix();
}

// 描画
void CharacterBase::DrawLit()
{
	if (!m_spModel)
	{
		return;
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos());
	Math::Matrix rotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(GetRotation().y));
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());

	Math::Matrix localMat = scaleMat * rotYMat * transMat;

	if (IsSelected())
	{
		KdShaderManager::Instance().m_StandardShader.SetSelected(true);
	}

	KdShaderManager::Instance().m_StandardShader.SetColorOverlay(m_overlayColor, GetOverlayRate());

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, localMat);

	KdShaderManager::Instance().m_StandardShader.SetSelected(false);

}

void CharacterBase::GenerateDepthMapFromLight()
{
	/*if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}*/
}

void CharacterBase::DrawInspectorHeader()
{
	// 名前・位置・回転・大きさ(スクロールしない部分)
	DrawBasicInspecter();
}

void CharacterBase::DrawInspector()
{
	if (InspectorUI::BeginGroup("Collision"))
	{
		// 当たり判定(押し戻し)球の半径
		InspectorUI::Float("BumpSphereRadius", m_bumpSphereRadius, 0.01f, 0.01f, "%.2f m");

		InspectorUI::EndGroup();
	}
}

void CharacterBase::SetRotation(const Math::Vector3& rotation)
{
	
	// キャラは基本Y軸回転しかしないのでY軸の回転行列を作る
	Math::Matrix rotYMat =
		Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(rotation.y));

	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos());
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());

	m_mWorld = scaleMat * rotYMat * transMat;

	m_rotation = Math::Vector3(0.0f, rotation.y, 0.0f);
}

DirectX::BoundingSphere CharacterBase::GetBumpSphere() const
{
	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos() + Math::Vector3(0, 0.5, 0);
	sphere.Radius = m_bumpSphereRadius;

	return sphere;
}


float CharacterBase::GetOverlayRate() 
{
	if (m_overlayTime <= 0)
	{
		return m_maxOverlayRate = 0.0f;
	}

	return m_maxOverlayRate *= m_overlayTime / m_overlayDuration;
}

void CharacterBase::UpdateOverlay()
{
	float UnscaledDeltaTime = TimeManager::Instance().GetUnscaleeDeltaTime();

	m_overlayTime -= UnscaledDeltaTime;

	if (m_overlayTime <= 0)
	{
		m_overlayTime = 0.0f;
	}
}

// 解放
void CharacterBase::Release()
{
	m_spModel = nullptr;
}

void CharacterBase::InitCharacterModel(const std::string& modelPath, const std::string& colliderName, const Math::Vector3& colliderOffset, float colliderRadius, const std::string& objectName)
{
	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData(modelPath);

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(colliderName, colliderOffset, colliderRadius, KdCollider::TypeBump);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// オブジェクト名セット
	SetObjectName(objectName);
}

void CharacterBase::DrawBumpDebugSphere(const Math::Vector3& offset, float radius)
{
	// 球の色は現在のエリアIDで変える
	m_pDebugWire->AddDebugSphere(GetPos() + offset, radius, GetAreaDebugColor());
}

Math::Color CharacterBase::GetAreaDebugColor() const
{
	return AreaVolume::GetAreaColor(m_currentAreaID);
}

void CharacterBase::UpdateFacingDirection()
{

	Math::Vector3 nowDir = m_mWorld.Backward();
	nowDir.y = 0.0f;


	Math::Vector3 toDir = GetMoveDir();
	toDir.y = 0.0f;

	if (nowDir.LengthSquared() <= 0.000001f ||
		toDir.LengthSquared() <= 0.000001f)
	{
		return;
	}

	// 正規化
	nowDir.Normalize();
	toDir.Normalize();

	// 内積を求める
	float dot = nowDir.Dot(toDir);
	dot = std::clamp(dot, -1.0f, 1.0f);
	// 角度に変換
	float angle = DirectX::XMConvertToDegrees(acos(dot));

	float rotationY = GetRotation().y;

	// 少しでも回転する必要があったら
	if (angle >= 0.1f)
	{

		float turnSpeed = GetTurnSpeed();
		// 回転角度の上限を設定
		if (angle >turnSpeed )
		{
			angle = turnSpeed;
		}

		// 外積を求める
		Math::Vector3 cross = nowDir.Cross(toDir);
		if (cross.y >= 0)
		{
			// 右回転
			rotationY += angle;
		}
		else
		{
			// 左回転
			rotationY -= angle;
		}

		// 角度を循環
		if (rotationY >= 360)
		{
			rotationY -= 360;
		}
		else if (rotationY < 0)
		{
			rotationY += 360;
		}
	}

	SetRotation(Math::Vector3(0.0f, rotationY, 0.0f));
}

void CharacterBase::UpdateMatrix()
{
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());
	Math::Matrix rotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(GetRotation().y));
	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos());

	m_mWorld = scaleMat * rotYMat * transMat;

}

void CharacterBase::UpdateCurrentAreaID()
{

	DirectX::BoundingSphere sphere = GetBumpSphere();

	// ボックスの中に入っている割合が高いIDとCoverageを保存
	std::unordered_map<int, float> maxCoverageByID;

	for (const auto& obj : CollisionManager::Instance().GetObjects(CollisionLayer::Area))
	{
		auto areaVolume = std::dynamic_pointer_cast<AreaVolume>(obj.lock());

		if (!areaVolume)
		{
			continue;
		}

		float coverage = areaVolume->CalcCoverage(sphere);
		int   areaID = areaVolume->GetAreaID();

		if (maxCoverageByID[areaID] < coverage)
		{
			maxCoverageByID[areaID] = coverage;
		}
	}


	int   bestID = -1;
	float bestCoverage = 0.0f;

	for (const auto&[id,coverage] : maxCoverageByID)
	{
		// 割合の高いIDを探す
		if (coverage > bestCoverage)
		{
			bestCoverage = coverage;
			bestID = id;
		}

	}
	
	if (bestID < 0)
	{
		return;
	}

	auto it = maxCoverageByID.find(m_currentAreaID);

	float currentCoverage = (maxCoverageByID.end()!=it)?it->second:0.0f;
	
	// 今のエリアから完全に出た、別のエリアが、十分に深く入ってる
	if (currentCoverage <= 0.0f||
		bestCoverage > currentCoverage + AreaSwitchMargin)
	{
		m_currentAreaID = bestID;
	}
}

void CharacterBase::AddKnockBack(const Math::Vector3& dir, const float power)
{
	if (m_knockBack.Length() >= 0.6)
	{
		return;
	}
	m_knockBack += dir * power;
}

void CharacterBase::UpdateGravity()
{
	m_gravity += GetGravityAcceleration() * m_deltaTime;

	Math::Vector3 gravityMove = { 0.0f,-m_gravity * m_deltaTime ,0.0f };

	AddPendingMove(gravityMove);
}

