#include "Player.h"

#include"../../Camera/CameraBase.h"
#include"../Enemy/EnemyBase.h"

#include"../../Stage/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../Stage/Collision/AABBCollision/AABBCollisionManager.h"

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

		InitCharacterModel("Asset/Models/Player/Player.gltf", "Player"
			, Math::Vector3(0, 0.5, 0), 0.5, "Player");

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
		m_bumpPushRate = 0.1f;

		// ステートマシンに持ち主をセット
		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<PlayerNormalState>();

		m_maxWalkableSlopeAngle = 45.0f;
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f, 0.0f, 0.0f });
}

void Player::Update()
{
	// 操作入力
	UpdateInput();

	UpdateComboGrace();

	// 各ステートの更新
	m_stateMachine.Update();

	UpdateGravity();

	if (!IsGrounded())
	{
		UpdateGroundPosY();
	}
}

void Player::PostUpdate()
{

	//アニメーション更新
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

void Player::UpdateAttackFrame()
{
	// Attack timings use elapsed 60 Hz frames, not animation clip frames.
	m_animFrame += 60.0f * m_deltaTime;
}

void Player::DrawDebug()
{
	CollisionManager::Instance().DrawDebug();

	m_pDebugWire->AddDebugSphere(GetPos() + Math::Vector3(0, 0.5, 0), 0.5, kRedColor);
	m_pDebugWire->Draw();
}

void Player::DrawInspector()
{
	CharacterBase::DrawInspector();
	// パラメーター変更
	m_parameter.DrawInspecter();
}


void Player::ClearHitTargets()
{
	m_hitCooldownTimer -= 60.0f * m_deltaTime;

	if (m_hitCooldownTimer <= 0)
	{
		m_hitTargets.clear();
		const auto& param = m_parameter.GetParam();
		m_hitCooldownTimer = param.m_hitCooldownDuration;
	}
}

void Player::UpdateAttackCollision(const AttackType type)
{
	if (type != AttackType::NormalAttack && type != AttackType::SpecialMove)
	{
		return;
	}

	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	// スフィアを作る
	DirectX::BoundingSphere sphere;
	float damage = 0.0f;

	if(type==AttackType::NormalAttack)
	{
		sphere = CreateAttackSphere();
		damage = m_parameter.GetParam().m_attackPower;
	}
	else if (type == AttackType::SpecialMove)
	{
		sphere = CreateSpecialMoveSphere();
		damage = m_parameter.GetParam().m_specialAttackPower;
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

		// キャラクター(Enemy・Boss)だけ取得
		auto enemy = std::dynamic_pointer_cast<CharacterBase>(obj);

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

		if (enemy->Intersects(sphereInfo, &result) && !result.empty())
		{

			// ノックバック方向を作る
			Math::Vector3 knockBackDir = enemy->GetPos() - GetPos();
			knockBackDir.y = 0.0f;

			if (knockBackDir.LengthSquared() > 0.000001f)
			{
				knockBackDir.Normalize();
			}

			AttackInfo attackInfo;
			attackInfo.damage = damage;
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

	m_playerMove.UpdateMoveInput(*this);
	m_playerAttack.UpdateAttackInput();
	m_playerJump.UpdateJumpInput(*this);
	m_playerGuard.UpdateGuardInput();
	m_playerSpecialMove.UpdateSpecialMoveInput();
}

void Player::UpdateParryInput()
{
	m_playerGuard.UpdateParryInput();
}


void Player::UpdateMove()
{
	m_playerMove.UpdateMove(*this);
}

void Player::UpdateAttackMove()
{
	m_playerAttack.UpdateAttackMove(*this);
}

void Player::UpdateGravity()
{
	const float gravityAcceleration = m_parameter.GetParam().m_gravityAcceleration;

	m_gravity += gravityAcceleration * m_deltaTime;

	Math::Vector3 gravityMove = { 0.0f,-m_gravity * m_deltaTime ,0.0f };

	AddPendingMove(gravityMove);

	//m_pDebugWire->AddDebugLine(GetPos(),Math::Vector3::Down,m_gravity*deltaTime,kBlackColor);

}

void Player::UpdateSpecialMove()
{
	
	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	m_playerSpecialMove.UpdateSpecialMove(*this);

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
	nowDir.Normalize();

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
	dot = std::clamp(dot, -1.0f,1.0f);
	// 角度に変換
	float angle = DirectX::XMConvertToDegrees(acos(dot));

	float rotationY = GetRotation().y;

	// 少しでも回転する必要があったら
	if (angle >= 0.1f)
	{
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
	}

	SetRotation(Math::Vector3(0.0f, rotationY, 0.0f));
}

void Player::ApplyCameraRelativeMove(float speed)
{
	auto spCamera = m_wpCamera.lock();

	if (!spCamera)
	{
		return;
	}

	Math::Matrix camRotYMat = spCamera->GetRotationYMatrix();

	// 入力方向をカメラの向きに合わせて回転させる
	SetMoveDir(Math::Vector3::TransformNormal(GetMoveDir(), camRotYMat));

	Math::Vector3 dir = GetMoveDir();

	if (dir.LengthSquared() > 0.0f)
	{
		dir.Normalize();
	}

	Math::Vector3 move = dir * (speed * 60.0f) * m_deltaTime;

	AddPendingMove(move);
}

void Player::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Player::UpdateGroundPosY()
{
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();

	// 少し高いところから飛ばす(段差の許容範囲)
	static float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;
	// レイの長さを設定
	rayInfo.m_range = 100.f;

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround | KdCollider::TypeBump;

	std::vector<const std::vector< std::weak_ptr<KdGameObject>>*>lists;

	lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::Ground));
	lists.push_back(&CollisionManager::Instance().GetObjects(CollisionLayer::AABB));


	for (const auto& objList : lists)
	{
		for (const auto wpGameObj : *objList)
		{
			std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
			if (spGameObj)
			{
				std::list<KdCollider::CollisionResult> retRayList;
				spGameObj->Intersects(rayInfo, &retRayList);

				// レイに当たったリストから一番近いオブジェクトを検出
				float maxOverLap = 0;
				Math::Vector3 hitPos = {};
				for (auto& ret : retRayList)
				{
					// レイを遮断しオーバーした長さが
					// 一番長いものを探す
					if (maxOverLap < ret.m_overlapDistance)
					{
						maxOverLap = ret.m_overlapDistance;
						SetGroundYPos(ret.m_hitPos.y);
					}
				}
			}
		}
	}
}


void Player::StartComboGrace()
{
	m_playerAttack.StartComboGrace();
}

bool Player::IsAlreadyHit(const std::shared_ptr<CharacterBase>& enemy) const
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
		m_stateMachine.ChangeState<PlayerDieState>();
	}
	else
	{
		if (GetStateType() != PlayerStateType::AttackState)
		{
			m_stateMachine.ChangeState<PlayerDamageState>();
		}
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage,GetPos(), m_flyTextPath);

	AddKnockBack(attackInfo.knockBackDir,attackInfo.knockBackPower);
}

PlayerAnimationType Player::GetChargeMoveAnimation() const
{
	return m_playerAttack.GetChargeMoveAnimation(*this);
}

void Player::StartJump()
{
	m_playerJump.StartJump(*this);
}

void Player::StartSpecialMove()
{
	// 攻撃がHitした敵リストをクリア
	m_hitTargets.clear();

	const auto& param = m_parameter.GetParam();
	m_hitCooldownTimer = param.m_hitCooldownDuration;

	// 移動する方向を決める
	m_playerSpecialMove.CreateSpecialMoveDir(*this);

	FacingDirectionToCamera();

	m_playerSpecialMove.SetSpecialMoveTiming(m_attackTiming.hitStart,m_attackTiming.hitEnd);

	m_animFrame = 0.0f;
}

void Player::EndSpecialMove()
{
	ResetCombo();
}

void Player::PlayAnimation(PlayerAnimationType type)
{
	m_animation.Play(type);
}


void Player::StartCurrentAttack()
{
	m_hitTargets.clear();

	m_playerAttack.StartAttack();
	m_playerAttack.SetAttackTiming(m_attackTiming.hitStart, m_attackTiming.hitEnd);
	m_playerAttack.ComboInputStartFrame();

	PlayAnimation(GetAttackAnimation());

	// フレームを0に
	m_animFrame = 0.0f;
}

void Player::NextCombo()
{
	m_playerAttack.UpdateComboState();
}

void Player::UpdateComboReception()
{
	m_playerAttack.UpdateComboReception(m_animFrame);
}

void Player::UpdateComboGrace()
{
	if (GetStateType() == PlayerStateType::NormalState)
	{
		m_playerAttack.UpdateComboGrace();
	}
}

void Player::SetStateType(PlayerStateType type)
{
	m_playerStateType = type;
	if (type != PlayerStateType::NormalState && type != PlayerStateType::AttackState)
	{
		ResetCombo();
	}
}


void Player::ResetCombo()
{
	m_playerAttack.ResetCombo();
}

