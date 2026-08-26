#include "CharacterBase.h"

#include"../../System/CollisionManager/CollisionManager.h"

CharacterBase::CharacterBase()
{}

CharacterBase::~CharacterBase()
{
	Release();
}

// 初期化
void CharacterBase::Init()
{}

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
	if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::DrawInspector()
{
	DrawBasicInspecter();

	ImGui::Separator();
}

DirectX::BoundingSphere CharacterBase::GetBumpSphere() const
{
	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos() + Math::Vector3(0, 0.5, 0);
	sphere.Radius = 0.5;

	return sphere;
}

void CharacterBase::UpdateCollision()
{
	//UpdateCharacterBumpCollision();
	//UpdateWallCollision();
	//UpdateGroundCollision();

	//UpdateWallCollision();
}

// 解放
void CharacterBase::Release()
{
	m_spModel = nullptr;
}

void CharacterBase::UpdateFacingDirection()
{

	Math::Vector3 nowDir = m_mWorld.Backward();

	Math::Vector3 toDir = GetMoveDir();

	toDir.Normalize();

	// 内積を求める
	float dot = nowDir.Dot(toDir);
	dot = std::clamp(dot, -1.0f, 1.0f);
	// 角度に変換
	float angle = DirectX::XMConvertToDegrees(acos(dot));

	// 少しでも回転する必要があったら
	if (angle >= 0.1f)
	{
		// 回転角度の上限を設定
		if (angle > m_turnSpeed)
		{
			angle = m_turnSpeed;
		}

		// 外積を求める
		Math::Vector3 cross = nowDir.Cross(toDir);
		if (cross.y >= 0)
		{
			// 右回転
			m_charaAngle += angle;
		}
		else
		{
			// 左回転
			m_charaAngle -= angle;
		}

		// 角度を循環
		if (m_charaAngle >= 360)
		{
			m_charaAngle -= 360;
		}
		else if (m_charaAngle < 0)
		{
			m_charaAngle += 360;
		}
	}

}

void CharacterBase::UpdateMatrix()
{
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());
	Math::Matrix rotYMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_charaAngle));
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
