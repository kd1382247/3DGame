#include "PlayerMove.h"

#include"../Player.h"
#include"../../../Camera/CameraBase.h"

#include"../../../../System/TimeManager/TimeManager.h"

void PlayerMove::UpdateMove(Player& player)
{

	Math::Matrix camRotYMat = Math::Matrix::Identity;

	auto spCamera = player.GetCamera().lock();
	if (!spCamera)
	{
		return;
	}

	camRotYMat = spCamera->GetRotationYMatrix();
	player.SetMoveDir(Math::Vector3::TransformNormal(player.GetMoveDir(), camRotYMat));

	player.UpdateFacingDirection();

	Math::Vector3 dir = player.GetMoveDir();

	if(dir.LengthSquared()>0.0f)
	{
		dir.Normalize();
	}

	float moveSpeed=player.GetMoveSpeed();

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	Math::Vector3 move = dir * (moveSpeed * 60.0f) * deltaTime;

	// 移動量をセット
	player.AddPendingMove(move);

}

void PlayerMove::UpdateMoveInput(Player& player)
{

	Math::Vector3 moveDir = Math::Vector3::Zero;

	m_movePressed = false;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		moveDir.z += 1.0f;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		moveDir.z -= 1.0f;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		moveDir.x -= 1.0f;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		moveDir.x += 1.0f;
	}

	if (moveDir != Math::Vector3::Zero)
	{
		m_movePressed = true;
	}

	player.SetMoveDir(moveDir);
}