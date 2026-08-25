#include "Player.h"

#include"../../Camera/CameraBase.h"
#include"../Enemy/EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../System/CollisionManager/CollisionManager.h"

#include"../../FlyText/FlyTextManager.h"

void Player::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Player/Player.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape
		("Player",Math::Vector3(0,0.5,0),0.5,KdCollider::TypeBump);

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// オブジェクト名セット
		SetObjectName("Player");

		// カテゴリーをセット
		SetObjectCategory(ObjectCategory::Character);

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);

		// パラメータークラス初期化
		m_parameter.Init();
		m_moveSpeed = m_parameter.GetParam().m_moveSpeed;
		m_turnSpeed = m_parameter.GetParam().m_turnSpeed;

		m_maxHP = m_parameter.GetParam().m_maxHP;
		m_hp = m_maxHP;


		m_bumpPushRate = 0.0f;
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f, 0.0f, 0.0f });
}

void Player::Update()
{
	
	////// 現在のオブジェクト数をデバッグ
	//KdDebugGUI::Instance().ClearLog();

	//KdDebugGUI::Instance().AddLog("HP%f\n", m_hp);

	UpdateInput();
	UpdateActionState();
	UpdateMoveState();

	UpdateMove();

	UpdateAttackCollision();
}

void Player::PostUpdate()
{

	UpdateGroundState();

	UpdateAnimation();
	
	CharacterBase::PostUpdate();
}

void Player::SetUpReference()
{
	if(!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}
}

void Player::DrawLit()
{
	CharacterBase::DrawLit();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.5, kRedColor);
}

void Player::DrawInspector()
{
	CharacterBase::DrawInspector();
	// パラメーター変更
	m_parameter.DrawInspecter();
}

void Player::UpdateAttackCollision()
{

	if (m_actionState != PlayerActionState::Attack)
	{
		return;
	}

	m_animFrame++;

	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	// 攻撃判定を出す位置
	Math::Vector3 attackPos = GetPos()+Math::Vector3(0.0f,0.5f,0.0f);
	// プレイヤーの向いてる方向
	Math::Vector3 attackDir = m_mWorld.Backward();

	attackDir.y = 0;
	if (attackDir.LengthSquared() < 0.000001f)
	{
		return;
	}

	attackDir.Normalize();

	// プレイヤーの少し前に出す
	attackPos += attackDir * 0.8f;
	
	// 攻撃判定用のスフィアを作成
	DirectX::BoundingSphere sphere;
	sphere.Center = attackPos;
	sphere.Radius = 0.7;

	const auto& characters =
		CollisionManager::Instance().GetObjects(CollisionLayer::CharacterBump);

	for (const auto& weakObj : characters)
	{
		auto obj = weakObj.lock();

		if (!obj)
		{
			continue;
		}
		// 自分自身は攻撃しない
		if (obj == shared_from_this())
		{
			continue;
		}

		// Enemyだけ取得
		auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

		if (!enemy)
		{
			continue;
		}

		// 一度当たった敵はスキップ
		if (IsAlreadyHit(enemy))
		{
			continue;
		}


		KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

		std::list < KdCollider::CollisionResult>result;

		if (enemy->Intersects(sphereInfo, &result));

		if (!result.empty())
		{

			// ノックバック方向を作る
			Math::Vector3 knockBackDir = enemy->GetPos() - GetPos();
			knockBackDir.y = 0.0f;

			if (knockBackDir.LengthSquared() > 0.000001f)
			{
				knockBackDir.Normalize();
			}

			AttackInfo attackInfo;
			attackInfo.damage = 10;
			attackInfo.knockBackDir = knockBackDir;
			attackInfo.knockBackPower = 0.1f;

			enemy->OnHit(attackInfo);
			m_hitTargets.emplace_back(enemy);
		}
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

}

void Player::UpdateInput()
{
	// 入力関数
	UpdateMoveInput();
	UpdateGuardInput();
	UpdateComboInput();
	// ガード中はジャンプ、攻撃の入力は受付ない
	if (m_actionState == PlayerActionState::Guard)
	{
		UpdateParryInput();
		return;
	}
	UpdateJumpInput();
	UpdateAttackInput();
	
}

void Player::UpdateMoveInput()
{
	Math::Vector3 moveDir = Math::Vector3::Zero;
	m_dirType = 0;
	m_moveFlg = false;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		moveDir.z += 1.0f;
		m_dirType |= Up;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		moveDir.z -= 1.0f;
		m_dirType |= Down;
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		moveDir.x -= 1.0f;
		m_dirType |= Left;
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		moveDir.x += 1.0f;
		m_dirType |= Right;
	}

	if (moveDir != Math::Vector3::Zero)
	{
		m_moveFlg = true;
	}

	SetMoveDir(moveDir);
}

void Player::UpdateJumpInput()
{
		const bool currentJumpButton = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

		m_jumpButton = currentJumpButton&&m_gravity<=0;	
}

void Player::UpdateAttackInput()
{
	bool currentAttackButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

	m_attackTrigger = currentAttackButton && !m_prevAttackButton;

	m_attackButton = currentAttackButton;
	m_prevAttackButton = currentAttackButton;

}

void Player::UpdateGuardInput()
{
	bool currentGuardButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

	m_guardTrigger = currentGuardButton && !m_prevGuardButton;

	m_guardButton = currentGuardButton;
	m_prevGuardButton = currentGuardButton;

	// ガードの状態を変更
	if (m_guardTrigger)
	{
		// ガード解除
		if (m_actionState == PlayerActionState::Guard)
		{
			m_guardState = GuardState::GuardCancel;
		}
		else if (m_actionState != PlayerActionState::Guard)
		{
			m_guardState = GuardState::Guard;
		}
	}
}

void Player::UpdateParryInput()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_guardState = GuardState::Parry;
	}
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

			UpdateComboState();

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

	auto spCamera = m_wpCamera.lock();
	if (!spCamera)
	{
		return;
	}

	camRotYMat = spCamera->GetRotationYMatrix();
	SetMoveDir(Math::Vector3::TransformNormal(GetMoveDir(), camRotYMat));


	if (m_moveFlg&&
		m_actionState!=PlayerActionState::Attack&&
		m_actionState!=PlayerActionState::Guard)
	{
		UpdateFacingDirection();
	}
	
	if (m_actionState == PlayerActionState::Attack||
		m_actionState==PlayerActionState::Guard)
	{
		AttackFacingDirection();
	}


	m_gravity += 0.02;

	Math::Vector3 dir = GetMoveDir();
	dir.Normalize();

	nowPos += dir * m_moveSpeed;

	nowPos.y -= m_gravity;

	SetPos(nowPos);

}

void Player::AttackFacingDirection()
{
	Math::Matrix camRotYMat = Math::Matrix::Identity;

	auto spCamera = m_wpCamera.lock();
	if (!spCamera)
	{
		return;
	}

	camRotYMat = spCamera->GetRotationYMatrix();
	
	// 現在向いている方向
	Math::Vector3 nowDir = m_mWorld.Backward();

	nowDir.y = 0;

	if (nowDir.LengthSquared() <= 0.000001f)
	{
		return;
	}

    // カメラから見て前方向に向かせたい
	Math::Vector3 toDir = Math::Vector3::TransformNormal(Math::Vector3::Backward, camRotYMat);

	toDir.y = 0;
	if (toDir.LengthSquared() <= 0.000001f)
	{
		return;
	}

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
		if (angle > m_attackTurnSpeed)
		{
			angle = m_attackTurnSpeed;
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

void Player::UpdateActionState()
{

	if (m_actionState == PlayerActionState::Die)
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
	// ガード
	if (m_actionState == PlayerActionState::Guard)
	{
	
		if (m_guardState == GuardState::GuardCancel)
		{
			ChangeActionState(PlayerActionState::Normal);
		}
		else if (m_guardState == GuardState::GuardHit||m_guardState==GuardState::Parry)
		{
			if (m_animation.IsFinished())
			{
				m_guardState = GuardState::Guard;
			}
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

	// ガードキーが押されたら
	if (m_guardTrigger)
	{
		ChangeActionState(PlayerActionState::Guard);
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
	if (m_preAttackCombo == AttackCombo::Attack1)
	{
		m_currentAttackCombo = AttackCombo::Attack2;
	}
	else if (m_preAttackCombo == AttackCombo::Attack2)
	{
		m_currentAttackCombo = AttackCombo::Attack3;
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
	else if (m_actionState == PlayerActionState::Guard)
	{
		if (m_guardState == GuardState::Guard)
		{
			nextAnimation = PlayerAnimationType::Defend;
		}
		else if (m_guardState == GuardState::GuardHit)
		{
			nextAnimation = PlayerAnimationType::Defend;
		}
		else if (m_guardState == GuardState::Parry)
		{
			nextAnimation = PlayerAnimationType::Parry;
		}

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

	m_animation.Play(nextAnimation);
	m_animation.Update();
}

void Player::SetAttackTiming()
{
	if (m_currentAttackCombo == AttackCombo::Attack1)
	{
		m_attackTiming.hitStart=1;
		m_attackTiming.hitEnd=7;
	}
	if (m_currentAttackCombo == AttackCombo::Attack2)
	{
		m_attackTiming.hitStart=1;
		m_attackTiming.hitEnd=7;
	}
	if (m_currentAttackCombo == AttackCombo::Attack3)
	{
		m_attackTiming.hitStart=3;
		m_attackTiming.hitEnd=9;
	}
	// フレームを0に
	m_animFrame = 0.0f;
}

bool Player::IsAlreadyHit(const std::shared_ptr<EnemyBase>& enemy) const
{

	for (const auto& weakEnemy : m_hitTargets)
	{
		auto hitEnemy = weakEnemy.lock();

		if (!hitEnemy)
		{
			continue;
		}

		if (hitEnemy == enemy)
		{
			return true;
		}

	}

	return false;

}

void Player::OnHit(const AttackInfo& attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
	}
	
	FlyTextManager::Instance().CreateDamateText(attackInfo.damage,GetPos());

	AddKnockBack(attackInfo.knockBackDir,attackInfo.knockBackPower);
}

void Player::ResetCombo()
{
	m_currentAttackCombo = AttackCombo::Attack1;

	m_preAttackCombo = AttackCombo::Attack1;

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

		// 攻撃がHitした敵リストをクリア
		m_hitTargets.clear();

		// 当たり判定をするタイミングをセット
		SetAttackTiming();
		
		break;
	case PlayerActionState::Damage:
		break;
	case PlayerActionState::Die:
		break;
	case PlayerActionState::JumpStart:

		ResetCombo();
		m_jumpFlg = true;
		m_gravity = -m_parameter.GetParam().m_jumpPow;

		break;
	case PlayerActionState::JumpAir:

		break;
	case PlayerActionState::JumpLand:
		break;
	}
}

void Player::ExitState(PlayerActionState _state)
{
	switch (_state)
	{
	case PlayerActionState::Normal:
		break;
	case PlayerActionState::Attack:
		
		m_preAttackCombo = m_currentAttackCombo;
	
		break;
	case PlayerActionState::Damage:
		break;
	case PlayerActionState::Die:
		break;
	case PlayerActionState::JumpStart:
		break;
	case PlayerActionState::JumpAir:

		break;
	case PlayerActionState::JumpLand:
		break;
	}
}
