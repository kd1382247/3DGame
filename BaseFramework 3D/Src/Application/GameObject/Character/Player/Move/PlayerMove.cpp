#include "PlayerMove.h"

#include"../Player.h"

void PlayerMove::UpdateMove(Player& player)
{
	player.ApplyCameraRelativeMove(player.GetMoveSpeed());

	player.UpdateFacingDirection();
}

void PlayerMove::UpdateMoveInput(Player& player)
{

	Math::Vector3 moveDir = Math::Vector3::Zero;
	player.SetMoveType(Player::MoveType::IDLE);

	m_movePressed = false;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		moveDir.z += 1.0f;
		player.SetMoveType(Player::MoveType::BWD);
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		moveDir.z -= 1.0f;
		player.SetMoveType(Player::MoveType::FWD);
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		moveDir.x -= 1.0f;
		player.SetMoveType(Player::MoveType::LFT);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		moveDir.x += 1.0f;
		player.SetMoveType(Player::MoveType::RGT);
	}

	if (moveDir != Math::Vector3::Zero)
	{
		m_movePressed = true;
	}

	player.SetMoveDir(moveDir);
}