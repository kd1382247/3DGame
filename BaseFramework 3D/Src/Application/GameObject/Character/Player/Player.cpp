#include "Player.h"

#include"../../Camera/CameraBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../Terrains/Ground/Ground.h"
void Player::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Character/Player1.gltf");

		m_objectName = "Player";

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
	}

	SetPos({ -12.0f, 12.5f, 1.5f });
}

void Player::Update()
{

	UpdateInput();
	UpdateActionState();
	UpdateMoveState();

	UpdateMove();
}

void Player::PostUpdate()
{

	CharacterBase::PostUpdate();

	UpdateGroundState();

	UpdateAnimation();

}

void Player::SetUpReference()
{
	std::shared_ptr<Ground>ground = GameObjectFinder::Instance().FindObject<Ground>();
	m_wpHitObjectList.push_back(ground);
}

void Player::UpdateInput()
{
	// 入力関数
	UpdateMoveInput();
	UpdateJumpInput();
	UpdateAttackInput();
	UpdateComboInput();
}

void Player::UpdateMoveInput()
{
	m_inputMoveDir = Math::Vector3::Zero;
	m_dirType = 0;
	m_moveFlg = false;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_inputMoveDir.z += 1.0f;
		m_dirType |= Up;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_inputMoveDir.z -= 1.0f;
		m_dirType |= Down;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_inputMoveDir.x -= 1.0f;
		m_dirType |= Left;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_inputMoveDir.x += 1.0f;
		m_dirType |= Right;
	}

	if (m_inputMoveDir != Math::Vector3::Zero)
	{
		m_moveFlg = true;
	}
}

void Player::UpdateJumpInput()
{
	const bool currentJumpButton = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

	m_jumpTrigger = currentJumpButton && !m_prevJumpButton;

	m_jumpButton = currentJumpButton;
	m_prevJumpButton = currentJumpButton;
}

void Player::UpdateAttackInput()
{
	bool currentAttackButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	m_attackTrigger = currentAttackButton && !m_prevAttackButton;

	m_attackButton = currentAttackButton;
	m_prevAttackButton = currentAttackButton;

}

void Player::UpdateComboInput()
{
	// コンボ受付
	if (m_canCombo)
	{	
		++m_comboInputCnt;

		if (m_attackButton)
		{
			m_canCombo = false;
			m_comboInputCnt = 0;

			m_currentAttackCombo = m_nextAttackCombo;
			ChangeActionState(PlayerActionState::Attack);
			return;
		}

		if (m_comboInputCnt > 20)
		{
			ResetCombo();
		}
	}
}

void Player::UpdateMove()
{
	
	Math::Vector3 nowPos = GetPos();

	Math::Matrix camRotYMat = Math::Matrix::Identity;

	if (auto camera=m_wpCamera.lock())
	{
		camRotYMat = camera->GetRotationYMatrix();
	}

	Math::Vector3 dir = Math::Vector3::TransformNormal(m_inputMoveDir, camRotYMat);

	if (m_moveFlg)
	{
		// 正規化
		dir.Normalize();

		// キャラが向いている方向
		Math::Vector3 nowDir = m_mWorld.Backward();

		// 向きたい方向
		Math::Vector3 toDir = dir;

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
				m_angle += angle;
			}
			else
			{
				// 左回転
				m_angle -= angle;
			}

			// 角度を循環
			if (m_angle >= 360)
			{
				m_angle -= 360;
			}
			else if (m_angle < 0)
			{
				m_angle += 360;
			}
		}

	}

	m_Gravity += 0.02;

	nowPos += dir * m_moveSpeed;

	nowPos.y -= m_Gravity;

	Math::Matrix transMat = Math::Matrix::CreateTranslation(nowPos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());
	Math::Matrix rotationMat = Math::Matrix::Identity;
	rotationMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_angle));

	m_mWorld = scaleMat * rotationMat * transMat;

}

void Player::UpdateActionState()
{

	if (m_actionState == PlayerActionState::Dead)
	{
		return;
	}
	if (m_actionState == PlayerActionState::Damage)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(PlayerActionState::Normal);
		}
		return;
	}
	
	if (m_actionState == PlayerActionState::Attack)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(PlayerActionState::Normal);
			
			// 3段目以外なら次のコンボを受け付ける
			if (m_currentAttackCombo != AttackCombo::Attack3)
			{
				m_canCombo = true;
				m_comboInputCnt = 0;
			}
			else
			{
				ResetCombo();
			}

		}
		return;
	}

	if (m_actionState == PlayerActionState::JumpStart)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(PlayerActionState::JumpAir);
		}
		return;
	}
	if (m_actionState == PlayerActionState::JumpAir)
	{
		return;
	}
	if (m_actionState == PlayerActionState::JumpLand)
	{
		if (m_animation.IsFinished())
		{
			ChangeActionState(PlayerActionState::Normal);
		}
		return;
	}

	// ジャンプキーが押されたら
	if (m_jumpButton)
	{
		ChangeActionState(PlayerActionState::JumpStart);
		return;
	}
	// 攻撃キーが押されたら
	if (m_attackButton)
	{
		ChangeActionState(PlayerActionState::Attack);
		return;
	}

	ChangeActionState(PlayerActionState::Normal);
}

void Player::UpdateMoveState()
{
	if (m_moveFlg)
	{ 
		m_moveState = PlayerMoveState::Run;
	}
	else
	{
		m_moveState = PlayerMoveState::Idle;
	}
}

void Player::UpdateComboState()
{
	// 次の攻撃パターンを予約
	switch (m_currentAttackCombo)
	{
	case Player::AttackCombo::Attack1:
		m_nextAttackCombo = AttackCombo::Attack2;
		break;
	case Player::AttackCombo::Attack2:
		m_nextAttackCombo = AttackCombo::Attack3;
		break;
	case Player::AttackCombo::Attack3:
		m_nextAttackCombo = AttackCombo::Attack1;
		break;
	}
}

void Player::UpdateGroundState()
{
	if (m_actionState != PlayerActionState::JumpAir)
	{
		return;
	}

	// 地面判定でジャンプフラグがfalseになったら着地
	if (!m_jumpFlg)
	{
		ChangeActionState(PlayerActionState::JumpLand);
	}
}

void Player::UpdateAnimation()
{

	PlayerAnimationType nextAnimation = PlayerAnimationType::Idle;

	if (m_actionState == PlayerActionState::Damage)
	{
		nextAnimation = PlayerAnimationType::GetHit;
	}
	else if (m_actionState == PlayerActionState::JumpStart)
	{
		nextAnimation = PlayerAnimationType::JumpStart;
	}
	else if (m_actionState == PlayerActionState::JumpAir)
	{
		nextAnimation = PlayerAnimationType::JumpAir;
	}
	else if (m_actionState == PlayerActionState::JumpLand)
	{
		nextAnimation = PlayerAnimationType::JumpLand;
	}
	else if(m_actionState==PlayerActionState::Attack)
	{
		switch (m_currentAttackCombo)
		{
		case Player::AttackCombo::Attack1:
			nextAnimation = PlayerAnimationType::Attack1;
			break;
		case Player::AttackCombo::Attack2:
			nextAnimation = PlayerAnimationType::Attack2;
			break;
		case Player::AttackCombo::Attack3:
			nextAnimation = PlayerAnimationType::Attack3;
			break;
		}
	}
	else if (m_moveState == PlayerMoveState::Run)
	{
		if (m_dirType & Up)
		{
			nextAnimation = PlayerAnimationType::MoveFWD;
		}
		if (m_dirType & Down)
		{
			nextAnimation = PlayerAnimationType::MoveFWD;
		}
		if (m_dirType & Left)
		{
			nextAnimation = PlayerAnimationType::MoveFWD;
		}
		if (m_dirType & Right)
		{
			nextAnimation = PlayerAnimationType::MoveFWD;
		}
	}
	else
	{
		nextAnimation = PlayerAnimationType::Idle;
	}

	m_animation.Change(nextAnimation);
	m_animation.Update();
}


void Player::ResetCombo()
{
	m_currentAttackCombo = AttackCombo::Attack1;
	m_nextAttackCombo = AttackCombo::Attack1;

	m_canCombo = false;
	m_comboInputCnt = 0;
}

void Player::ChangeActionState(PlayerActionState _nextState)
{
	if (m_actionState == _nextState)
	{
		return;
	}

	ExitState(m_actionState);
	m_actionState = _nextState;
	EnterState(m_actionState);

}

void Player::EnterState(PlayerActionState _state)
{
	switch (_state)
	{
	case PlayerActionState::Normal:
		break;
	case PlayerActionState::Attack:
		UpdateComboState();
		break;
	case PlayerActionState::Damage:
		break;
	case PlayerActionState::Dead:
		break;
	case PlayerActionState::JumpStart:
		m_jumpFlg = true;
		m_Gravity = -0.4f;
		break;
	case PlayerActionState::JumpAir:

		break;
	case PlayerActionState::JumpLand:
		break;
	default:
		break;
	}
}

void Player::ExitState(PlayerActionState _state)
{

}
