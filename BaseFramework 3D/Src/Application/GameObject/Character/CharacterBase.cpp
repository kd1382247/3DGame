#include "CharacterBase.h"

#include"../../Editor/EditorManager.h"

#include"../../System/CollisionManager/CollisionManager.h"
#include"../../System/TimeManager/TimeManager.h"

#include"../Gimmick/AreaVolume/AreaVolume.h"

namespace
{
	// 半径が0以下だと、衝突判定が成り立たないので下限を決めておく
	constexpr float MinBumpRadius = 0.01f;

	// 胴体の中心の高さ(足元から、Scale 1 のとき)
	constexpr float BodyCenterHeight = 0.5f;

	// Scaleの最大の成分
	// (KdSphereCollisionが使うBoundingSphere::Transformは、半径に最大の拡大率を掛けるので、それに合わせる)
	float MaxScaleComponent(const Math::Vector3& scale)
	{
		return std::max(std::max(std::abs(scale.x), std::abs(scale.y)), std::abs(scale.z));
	}
}

CharacterBase::CharacterBase()
{
	m_spParentLink = AddComponent<ParentLink>();

	m_spParentLink->SetFollowMode(ParentLink::FollowMode::PlaceOnce);
	m_spParentLink->SetFollowScale(false);
	m_spParentLink->SetFollowRotation(true);

}

CharacterBase::~CharacterBase()
{
	Release();
}

// 初期化
void CharacterBase::Init()
{}

void CharacterBase::PreDraw()
{
	// EditorSceneかつ編集中だったら
	if(EditorManager::Instance().IsEditing())
	{
		m_spParentLink->Apply(*this);
	}
}

void CharacterBase::PreUpdate()
{
	// 部品の更新を呼ぶ
	UpdateComponents();

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

void CharacterBase::DrawDebug()
{
	if (!m_pDebugWire)
	{
		return;
	}

	DrawBumpDebugSphere();
	m_pDebugWire->Draw();
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

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, localMat,kWhiteColor,m_emissiveColor);

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

	// 親がいる場合、ローカル座標を変更する
	if (m_spParentLink->HasParent())
	{
		DrawNameInspector();

		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawScaleInspector();
		}

		return;
	}

	// 名前・位置・回転・大きさ(スクロールしない部分)
	DrawBasicInspecter();
}

void CharacterBase::DrawInspector()
{
	// 当たり判定(押し戻し)球の大きさは、各キャラのパラメータクラスで編集する
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
	return GetBumpSphereAt(GetPos());
}

DirectX::BoundingSphere CharacterBase::GetBumpSphereAt(const Math::Vector3& pos) const
{
	// KdColliderに登録した球は、ワールド行列(Scale込み)で変換されるので、
	// ここでもScaleを掛けて、同じ大きさ・同じ位置の球にする
	const Math::Vector3 scale = GetScale();

	DirectX::BoundingSphere sphere;

	sphere.Center = pos + Math::Vector3(0.0f, m_bumpSphereOffsetY * scale.y, 0.0f);
	sphere.Radius = m_bumpSphereRadius * MaxScaleComponent(scale);

	return sphere;
}

float CharacterBase::GetBumpSphereRadius() const
{
	return m_bumpSphereRadius * MaxScaleComponent(GetScale());
}

float CharacterBase::GetBumpSphereOffsetY() const
{
	return m_bumpSphereOffsetY * GetScale().y;
}

float CharacterBase::GetSizeScale() const
{
	return MaxScaleComponent(GetScale());
}

Math::Vector3 CharacterBase::GetBodyCenter() const
{
	return GetPos() + Math::Vector3(0.0f, BodyCenterHeight * GetScale().y, 0.0f);
}

void CharacterBase::SetBumpSphere(float offsetY, float radius)
{
	const float newRadius = (radius > MinBumpRadius) ? radius : MinBumpRadius;

	// 値が変わっていなければ何もしない(Editorでは毎フレーム呼ばれるため)
	if (m_bumpSphereOffsetY == offsetY && m_bumpSphereRadius == newRadius)
	{
		return;
	}

	m_bumpSphereOffsetY = offsetY;
	m_bumpSphereRadius  = newRadius;

	// KdColliderの形状も新しい値に合わせる
	RebuildBumpCollider();
}

void CharacterBase::RebuildBumpCollider()
{
	// InitCharacterModelの前は、まだコライダーを作らない
	if (m_bumpColliderName.empty())
	{
		return;
	}

	// RegisterCollisionShapeは同じ名前の形状を上書きしないので、コライダーごと作り直す
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape(m_bumpColliderName,
		Math::Vector3(0.0f, m_bumpSphereOffsetY, 0.0f), m_bumpSphereRadius, KdCollider::TypeBump);
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

	// 当たり判定(押し戻し)球の大きさを覚えて、KdColliderの形状もその値から作る
	// (球の中心は足元の真上なので、offsetはYだけ使う)
	m_bumpColliderName  = colliderName;
	m_bumpSphereOffsetY = colliderOffset.y;
	m_bumpSphereRadius  = (colliderRadius > MinBumpRadius) ? colliderRadius : MinBumpRadius;

	RebuildBumpCollider();

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// オブジェクト名セット
	SetObjectName(objectName);
}

void CharacterBase::DrawBumpDebugSphere()
{
	const DirectX::BoundingSphere sphere = GetBumpSphere();

	// 球の色は現在のエリアIDで変える
	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, GetAreaDebugColor());
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
	m_gravity += 55.0f * m_deltaTime;

	Math::Vector3 gravityMove = { 0.0f,-m_gravity * m_deltaTime ,0.0f };

	AddPendingMove(gravityMove);
}

