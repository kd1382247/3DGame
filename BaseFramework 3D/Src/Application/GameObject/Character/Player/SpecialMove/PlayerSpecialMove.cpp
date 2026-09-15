#include "PlayerSpecialMove.h"

#include"../Player.h"

#include"../../../Camera/CameraBase.h"

void PlayerSpecialMove::UpdateSpecialMoveInput()
{
	bool currentSpecialButton = (GetAsyncKeyState('E') & 0x8000);
	m_isSpecialMovePressed = currentSpecialButton;
}

void PlayerSpecialMove::UpdateSpcecialMove(Player& player)
{
	Math::Vector3 move = m_specialMoveDir * (0.3 * 60.0f) * player.GetDeltaTime();
	player.AddPendingMove(move);
}

void PlayerSpecialMove::SetSpecialMoveTiming(float& hitStart, float& hitEnd)
{
	hitStart = 10.0f;
	hitEnd = 30.0f;
}

void PlayerSpecialMove::CreateSpecialMoveDir(Player& player)
{

	Math::Matrix camRotYMat = Math::Matrix::Identity;

	auto spCamera = player.GetCamera().lock();
	if (!spCamera)
	{
		return;
	}

	camRotYMat = spCamera->GetRotationYMatrix();

	// カメラから見て前方向に向かせたい
	Math::Vector3 toDir = Math::Vector3::TransformNormal(Math::Vector3::Backward, camRotYMat);

	toDir.y = 0;
	toDir.Normalize();

	m_specialMoveDir = toDir;
}
