#include "Player.h"

#include"../../Camera/CameraBase.h"
#include"../Enemy/EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../System/CollisionManager/CollisionManager.h"

#include"../../../Scene/SceneManager.h"

#include"../../FlyText/FlyTextManager.h"

#include"../../HPBar/PlayerHPBar/PlayerHPBar.h"


#include"State/States/PlayerNormalState.h"
#include"State/States/PlayerDamageState.h"
#include"State/States/PlayerDieState.h"


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
		const auto& param = m_parameter.GetParam();

		m_hp = param.m_maxHP;
		m_bumpPushRate = 0.0f;

		m_stateMachine.ChangeState(*this, std::make_unique<PlayerNormalState>());

	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f, 0.0f, 0.0f });
}

void Player::Update()
{
	// 操作入力
	UpdateInput();

	// 各ステートの更新
	m_stateMachine.Update(*this);

	UpdateGravity();

}

void Player::PostUpdate()
{
	// アニメーション更新
	UpdateAnimation();
	
	CharacterBase::PostUpdate();
}

void Player::SetUpReference()
{
	if(!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}

	std::shared_ptr<PlayerHPBar>hpBar = std::make_shared<PlayerHPBar>();
	hpBar->Init();
	hpBar->SetPlayer(std::dynamic_pointer_cast<Player>(shared_from_this()));
	SceneManager::Instance().AddObject(hpBar);
}

void Player::DrawLit()
{
	CharacterBase::DrawLit();
}

void Player::DrawDebug()
{
	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.5, kRedColor);
	m_pDebugWire->Draw();
}

void Player::DrawInspector()
{
	CharacterBase::DrawInspector();
	// パラメーター変更
	m_parameter.DrawInspecter();
}

void Player::SetSpecialMoveTiming()
{
	m_attackTiming.hitStart = 10.0f;
	m_attackTiming.hitEnd = 40.0f;

	m_animFrame = 0.0f;
}

void Player::ClearHitTargets()
{
	m_hitCooldownTimer--;

	if (m_hitCooldownTimer <= 0)
	{
		m_hitTargets.clear();
		const auto& param = m_parameter.GetParam();
		m_hitCooldownTimer = param.m_hitCooldownDuration;
	}
}

void Player::UpdateAttackCollision(const AttackType type)
{

	m_animFrame++;

	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	// スフィアを作る
	DirectX::BoundingSphere sphere;

	if(type==AttackType::NormalAttack)
	{
		sphere = CreateAttackSphere();
	}
	if (type == AttackType::SpecialMove)
	{
		sphere = CreateSpecialMoveSphere();
	}

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

		if (enemy->IsInOutro())
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

		if (enemy->Intersects(sphereInfo, &result))

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

	UpdateJumpInput();
	UpdateAttackInput();
	UpdateSpecialMoveInput();
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

	m_jumpButton = currentJumpButton && m_gravity <= 0;
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
		if (m_guardState==GuardState::Guard)
		{
			m_guardState = GuardState::GuardCancel;
		}
		else
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

void Player::UpdateSpecialMoveInput()
{

	bool currentSpecialButton = (GetAsyncKeyState('E') & 0x8000);
	m_specialMoveButton = currentSpecialButton;

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

	UpdateFacingDirection();
	
	Math::Vector3 dir = GetMoveDir();
	dir.Normalize();

	const auto& param = m_parameter.GetParam();

	nowPos += dir *param.m_moveSpeed;
	SetPos(nowPos);

}

void Player::UpdateAttackMove()
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
	
	FacingDirectionToCamera();

	Math::Vector3 dir = GetMoveDir();
	dir.Normalize();

	const auto& param = m_parameter.GetParam();

	nowPos += dir * param.m_attackMoveSpeed;
	SetPos(nowPos);
}

void Player::UpdateGravity()
{
	Math::Vector3 nowPos = GetPos();
	m_gravity += 0.02f;
	nowPos.y -= m_gravity;
	SetPos(nowPos);
}

void Player::UpdateSpecialMove()
{
	
	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	UpdateGroundCollision();
	ClearHitTargets();
}

void Player::FacingDirectionToCamera()
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
	else if (m_preAttackCombo == AttackCombo::Attack3)
	{
		ResetCombo();
	}

	m_preAttackCombo = m_currentAttackCombo;
}

void Player::UpdateAnimation()
{
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

void Player::CreateSpecialMoveDir()
{
	Math::Matrix camRotYMat = Math::Matrix::Identity;

	auto spCamera = m_wpCamera.lock();
	if (!spCamera)
	{
		return;
	}

	camRotYMat = spCamera->GetRotationYMatrix();

	// カメラから見て前方向に向かせたい
	Math::Vector3 toDir = Math::Vector3::TransformNormal(Math::Vector3::Backward, camRotYMat);

	toDir.y = 0;
	toDir.Normalize();

	const auto& param = m_parameter.GetParam();

	m_specialMoveDir=toDir*param.m_specialMoveSpeed;
}

void Player::UpdateGroundCollision()
{

	auto character = std::dynamic_pointer_cast<CharacterBase>(shared_from_this());

	Math::Vector3 push = m_specialMoveDir;

	constexpr float MaxStep = 0.05f;

	float length = push.Length();

	if (length <= 0.000001f)
	{
		return;
	}

	int stepCount = static_cast<int>(std::ceil(length / MaxStep));

	Math::Vector3 delta = push / static_cast<float>(stepCount);

	for (int i = 0; i < stepCount; ++i)
	{
		// すでに進める方向が残っていない
		if (delta.LengthSquared() <= 0.000001f)
		{
			break;
		}

		SetPos(GetPos() + delta);

		CollisionManager::Instance().ResolveWallCollisionForCharacter(character);
		CollisionManager::Instance().ResolveGroundCollisionForCharacter(character);
	}
	
}

DirectX::BoundingSphere Player::CreateAttackSphere() const
{
	// 攻撃判定を出す位置
	Math::Vector3 attackPos = GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f);
	// プレイヤーの向いてる方向
	Math::Vector3 attackDir = m_mWorld.Backward();

	attackDir.y = 0;
	if (attackDir.LengthSquared() < 0.000001f)
	{
		return {};
	}

	attackDir.Normalize();

	// プレイヤーの少し前に出す
	attackPos += attackDir * 0.8f;

	// 攻撃判定用のスフィアを作成
	DirectX::BoundingSphere sphere;
	sphere.Center = attackPos;
	sphere.Radius = 0.7f;

	return sphere;
}

DirectX::BoundingSphere Player::CreateSpecialMoveSphere() const
{
	Math::Vector3 spherePos = GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f);

	DirectX::BoundingSphere sphere;

	sphere.Center = spherePos;
	sphere.Radius = 1.5f;

	return sphere;

}

void Player::OnHit(const AttackInfo attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		ChangeState<PlayerDieState>();
	}
	else
	{
		ChangeState<PlayerDamageState>();
	}

	
	FlyTextManager::Instance().CreateDamateText(attackInfo.damage,GetPos());

	AddKnockBack(attackInfo.knockBackDir,attackInfo.knockBackPower);
}

void Player::StartAttack()
{
	m_hitTargets.clear();
	SetAttackTiming();
}

void Player::EntAttack()
{
	m_canCombo = true;
}

void Player::StartJump()
{
	ResetCombo();

	m_isGrounded = false;

	const auto& param = m_parameter.GetParam();

	m_gravity -= param.m_jumpPow;
}

void Player::StartSpecialMove()
{
	// 攻撃がHitした敵リストをクリア
	m_hitTargets.clear();

	const auto& param = m_parameter.GetParam();
	m_hitCooldownTimer = param.m_hitCooldownDuration;

	// 移動する方向を決める
	CreateSpecialMoveDir();

	FacingDirectionToCamera();

	SetSpecialMoveTiming();
}

void Player::EndSpecialMove()
{
	ResetCombo();
}

PlayerAnimationType Player::GetGuardAnimation()const
{
	switch (m_guardState)
	{
	case Player::GuardState::Guard:
		return PlayerAnimationType::Defend;

	case Player::GuardState::GuardHit:
		return PlayerAnimationType::DefendHit;

	case Player::GuardState::Parry:
		return PlayerAnimationType::Parry;

	default :
		return PlayerAnimationType::Defend;
	}
}

void Player::PlayAnimation(PlayerAnimationType type)
{
	m_animation.Play(type);
}

PlayerAnimationType Player::GetAttackAnimation() const
{
	switch (m_currentAttackCombo)
	{
	case Player::AttackCombo::Attack1:
		return PlayerAnimationType::Attack1;

	case Player::AttackCombo::Attack2:
		return PlayerAnimationType::Attack2;

	case Player::AttackCombo::Attack3:
		return PlayerAnimationType::Attack3;
	}
}

void Player::ResetCombo()
{
	m_currentAttackCombo = AttackCombo::Attack1;

	m_preAttackCombo = AttackCombo::Attack1;

	m_canCombo = false;
	m_comboInputCnt = 0;
}

