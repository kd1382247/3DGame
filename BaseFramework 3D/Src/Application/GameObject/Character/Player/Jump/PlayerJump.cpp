#include "PlayerJump.h"

#include"../Player.h"

void PlayerJump::UpdateJumpInput(Player& player)
{

	const bool currentJumpButton = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

	m_jumpPressed = currentJumpButton && player.IsGrounded();
}

void PlayerJump::StartJump(Player& player)
{

	player.ResetCombo();

	player.SetIsGrounded(false);

	float jumpPower = player.GetJumpPower();

	float gravity = player.GetGravity();

	gravity-= (jumpPower * 60.0f);

	player.SetGravity(gravity);
}
