#include "CharacterBase.h"

#include"../../System/CollisionManager/CollisionManager.h"
#include"../../System/TimeManager/TimeManager.h"

#include"../../Editor/EditorManager.h"


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

}

// 更新
void CharacterBase::Update()
{}

void CharacterBase::PostUpdate()
{
	UpdateMatrix();
}

// 描画
void CharacterBase::DrawLit()
{
	if (!m_spModel)
	{
		return;
	}

	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos() + m_visualOffset);
	Math::Matrix rotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(GetRotation().y));
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());

	Math::Matrix localMat = scaleMat * rotYMat * transMat;

	if (IsSelected())
	{
		KdShaderManager::Instance().m_StandardShader.SetSelected(true);
	}

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

void CharacterBase::DrawInspector()
{

	DrawBasicInspecter();

	ImGui::Separator();

	// 当たり判定(押し戻し)球の半径
	if (ImGui::DragFloat("BumpSphereRadius", &m_bumpSphereRadius, 0.01f, 0.01f))
	{
		EditorManager::Instance().MarkDirty();
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

void CharacterBase::UpdateHitShake()
{

	float UnscaledDeltaTime = TimeManager::Instance().GetUnscaleeDeltaTime();

	m_hitShakeTime -= UnscaledDeltaTime;

	if (m_hitShakeTime <= 0)
	{
		m_hitShakeTime = 0;
		m_visualOffset = Math::Vector3::Zero;
		SetIsHitShake(false);
		return;
	}

	float progress = 1.0f - (m_hitShakeTime / m_hitShakeDuration);

	float currentPower = std::lerp(m_hitShakePower, 0.0f, progress);

	// 揺れ位置を変更するまでの時間を進める
	m_hitShakeIntervalTimer += UnscaledDeltaTime;

	if(m_hitShakeIntervalTimer>=m_hitShakeInterval)
	{
		m_hitShakeIntervalTimer = 0.0f;

		m_visualOffset.x = KdRandom::GetFloat(-currentPower, currentPower);
		m_visualOffset.y = KdRandom::GetFloat(-currentPower, currentPower);
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
	m_pDebugWire->AddDebugSphere(GetPos() + offset, radius, kRedColor);
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

int CharacterBase::GetCurrentAreaID(const Math::Vector3& pos)
{
	if (pos.y >= 3.5f)
	{
		return 2;
	}

	return 0;
}

void CharacterBase::AddKnockBack(const Math::Vector3& dir, const float power)
{
	if (m_knockBack.Length() >= 0.6)
	{
		return;
	}
	m_knockBack += dir * power;
}
