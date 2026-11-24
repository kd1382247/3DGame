#include "EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../../System/CollisionManager/CollisionManager.h"
#include"../../../System/TimeManager/TimeManager.h"

#include"../../../GameObject/Stage/WayPoint/WayPointManager.h"
#include"../../../GameObject/Stage/WayPoint/WayPoint.h"
#include"../../../GameObject/Stage/Collision/AIBlockCollision/AIBlock.h"
#include"../../../GameObject/Stage/Collision/AIBlockCollision/AIBlockManager.h"

#include"../../../Editor/EditorManager.h"

#include"../Player/Player.h"

#include"../../Gimmick/EnemySpawner/EnemySpawner.h"

#include"../../FlyText/FlyTextManager.h"

#include"../../Gimmick/Room/RoomManager.h"
#include"../../Gimmick/Room/Room.h"

void EnemyBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);

	m_bumpPushRate = 1.0f;

}

void EnemyBase::PreUpdate()
{
	CharacterBase::PreUpdate();

	// 部屋がクリア状態になったら
	if (IsRoomCleared())
	{
		ForceKill();
	}
}

void EnemyBase::Update()
{

}

void EnemyBase::PostUpdate()
{
	CharacterBase::PostUpdate();
}

void EnemyBase::DrawInspector()
{
	// 共通(当たり判定)
	CharacterBase::DrawInspector();

	// 各敵のパラメータ(到達距離・攻撃・クールタイムなども各Parameterの中に含まれる)
	DrawParameterInspector();
}

void EnemyBase::DrawInspectorHeader()
{
	CharacterBase::DrawInspectorHeader();

	// 対応する部屋(None のときは、部屋に関係なく召喚する)
	m_roomLink.DrawCombo();
}

void EnemyBase::SetUpReference()
{
	CharacterBase::SetUpReference();

	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void EnemyBase::DrawDebug()
{
	DrawBumpDebugSphere();
	m_pDebugWire->Draw();
}

void EnemyBase::InheritSpawnerTo(const std::shared_ptr<EnemyBase>& child)const
{
	if (!child)
	{
		return;
	}

	auto spSpawner = m_wpSpawner.lock();

	// スポナー経由でない敵(ステージに直置きなど)は、引き継ぐものがない
	if (!spSpawner)
	{
		return;
	}

	child->SetSpawner(spSpawner);

	// 自分が消える前に、子をスポナーの生存リストへ入れる
	// (順番が逆だと、リストが空になった瞬間に次のウェーブへ進んでしまう)
	spSpawner->RegisterSpawnedEnemy(child);
}

void EnemyBase::StartAttack()
{
	m_hitTarget = false;
	m_animFrame = 0.0f;

	// 攻撃中は元の大きさにしておく
	m_drawScaleRate = 1.0f;
}

void EnemyBase::UpdateAttackCollision()
{
	UpdateMeleeAttackCollision(m_meleeAttack.m_knockBackPower, GetAttackPower(), m_meleeAttack.m_sphereRadius, m_meleeAttack.m_forwardOffset);
}

void EnemyBase::Launch(const Math::Vector3& dir, float power)
{
	m_launchVec = dir;
	m_launchFlg = true;

	// 重力を反転
	m_gravity -= power*60.0f;
}

Math::Vector3 EnemyBase::CreateSpawnDirection()
{
	// 飛び出す方向を決める
	Math::Vector3 launchVec = {};

	float deg = KdRandom::GetFloat(1.0f, 360.0f);

	launchVec.x = sinf(DirectX::XMConvertToRadians(deg));
	launchVec.z = cosf(DirectX::XMConvertToRadians(deg));
	launchVec.y = 0;

	launchVec.Normalize();

	return launchVec;
}

bool EnemyBase::IsRoomCleared()
{
	// 部屋と紐づいているか
	if (!m_roomLink.HasLink()) { return false; }

	auto spRoom = m_roomLink.GetRoom();

	if (!spRoom) { return false; }

	// クリア、ボスの撃破演出状態ならばtrueを返す
	return spRoom->IsOutro()||spRoom->IsBossDefeat();
}

void EnemyBase::PlayHitEffect()
{

	auto spPlayer = m_wpPlayer.lock();

	Math::Vector3 playerPos = {};

	if (spPlayer)
	{
		playerPos = spPlayer->GetPos();
	}

	Math::Vector3 bossDir = playerPos - GetPos();

	bossDir.y = 0.0f;
	if (bossDir.LengthSquared() <= 0.000001f)
	{
		bossDir = Math::Vector3::Backward;
	}
	bossDir.Normalize();

	// ボスから見てプレイヤーの方向に、エフェクトの位置を持っていく
	// エフェクトの位置は、キャラの大きさに合わせる
	Math::Vector3 effectPos = GetPos() + bossDir * (m_effectDist * GetSizeScale()) + Math::Vector3(0.0f, 0.8f * GetScale().y, 0.0f);

	KdEffekseerManager::GetInstance().
		Play("Common/Hit/Slash_hit.efkefc", effectPos, 0.3f, 0.8f, false);

	KdEffekseerManager::GetInstance().
		Play("Common/Hit/Star_hit.efkefc", effectPos, 0.3f, 0.6f, false);

	KdEffekseerManager::GetInstance().
		Play("Common/Hit/Flash_hit.efkefc", effectPos, 0.4f, 0.6f, false);

}

void EnemyBase::UpdateDirectChase()
{

	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 pos = GetPos();

	float         moveSpeed = GetMoveSpeed();

	Math::Vector3 targetDir = spPlayer->GetPos() - pos;

	// Y成分はいらない
	targetDir.y = 0;

	SetMoveDir(targetDir);

	const float distance = targetDir.Length();

	// 到達判定の距離と、到達後に「離れた」とみなす余白は、キャラの大きさに合わせる
	const float reachDistance = GetReachDistance();
	const float reachDistanceMargin = reachDistance + 0.5f * GetSizeScale();

	if (m_hasReachedTarget)
	{
		if (distance > reachDistanceMargin)
		{
			m_hasReachedTarget = false;
		}
		else
		{
			m_hasReachedTarget = true;
			return;
		}
	}
	else
	{
		if (distance < reachDistance)
		{
			m_hasReachedTarget = true;
			return;
		}
		else
		{
			m_hasReachedTarget = false;
		}
	}

	targetDir.Normalize();

	Math::Vector3 move = targetDir * (moveSpeed*60.0f)*m_deltaTime;

	AddPendingMove(move);
}

void EnemyBase::UpdateFollowPath()
{

	UpdateFlowGoal();

	// 進むウェイポイントがない
	if (m_targetWayPointID < 0)
	{
		// 一定時間で新たにウェイポイントを探す
		m_pathRefreshTimer -= m_deltaTime;
		if (m_pathRefreshTimer < 0)
		{
			m_pathRefreshTimer = PathRefreshDuration;
			SelectStartWayPoint();
		}

		// 固まらないようにする
		UpdateDirectChase();

		return;
	}

	// IDからWayPointを取得
	auto targetPoint = WayPointManager::Instance().FindWayPoint(m_targetWayPointID);

	if (!targetPoint) 
	{
		m_targetWayPointID = -1;
		return;
	}

	// WayPointへの方向
	Math::Vector3 targetDir = targetPoint->GetPos() - GetPos();


	// X・Z平面だけで移動・到着判定する
	targetDir.y = 0.0f;

	SetMoveDir(targetDir);


	constexpr float arrivalDistance = 2.0f;

	if (targetDir.LengthSquared() <= arrivalDistance * arrivalDistance)
	{
		// ゴールに到達しているか
		if(WayPointManager::Instance().IsFlowGoal(m_targetWayPointID))
		{
			return;
		}
		else
		{

			// ゴールでなければ次のWayPointを探す
			int nextID = WayPointManager::Instance().GetNextWayPointID(m_targetWayPointID);

			m_targetWayPointID = nextID;

			// -1が帰ってきたら処理を処理を飛ばす
			if (m_targetWayPointID == -1)
			{
				return;
			}
		}
	}

	float distance = targetDir.Length();

	float moveSpeed = GetMoveSpeed();

	if (distance < moveSpeed)
	{
		moveSpeed = distance;
	}

	targetDir.Normalize();


	Math::Vector3 move = targetDir * (moveSpeed * 60.0f) * m_deltaTime;

	AddPendingMove(move);
}

bool EnemyBase::IsSightClear(const Math::Vector3& playerPos)
{
	// 遠いときはレイを撃たない(ウェイポイント移動のまま)
	if ((playerPos - GetPos()).LengthSquared() > SightCheckDistance * SightCheckDistance)
	{
		// 近づいたときにすぐ判定でき、かつ全員が同じフレームに撃たないよう、ばらつかせる
		m_sightCheckTimer = KdRandom::GetFloat(0.0f, SightCheckInterval);

		// 判定していない間は遮られている扱いにする
		m_isSightBlocked = true;

		return false;
	}

	// 一定間隔でだけレイを撃ち、結果を保持する
	m_sightCheckTimer -= m_deltaTime;

	if (m_sightCheckTimer <= 0.0f)
	{
		m_sightCheckTimer = SightCheckInterval;

		const Math::Vector3 offset = { 0.0f,SightRayHeight,0.0f };

		m_isSightBlocked = IsBlockedByAIBlock(GetPos() + offset, playerPos + offset);
	}

	return !m_isSightBlocked;
}

bool EnemyBase::IsBlockedByAIBlock(const Math::Vector3& from, const Math::Vector3& to) const
{
	Math::Vector3 direction = to - from;

	const float rayLength = direction.Length();

	if (rayLength <= 0.0f)
	{
		return false;
	}

	direction /= rayLength;

	const DirectX::XMVECTOR origin = DirectX::XMLoadFloat3(&from);
	const DirectX::XMVECTOR dir    = DirectX::XMLoadFloat3(&direction);

	for (const auto& block : AIBlockManager::Instance().GetAIBlockList())
	{
		if (!block)
		{
			continue;
		}

		float hitDistance = 0.0f;

		// AABBとレイの判定。プレイヤーより奥のブロックは無視する
		if (block->GetBox().Intersects(origin, dir, hitDistance) &&
			hitDistance <= rayLength)
		{
			return true;
		}
	}

	return false;
}

void EnemyBase::CanDirectChase()
{

	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	if (GetAreaID() < 0 || spPlayer->GetAreaID()<0)
	{
		return;
	}

	// 敵とプレイヤーが同じAreaIDだったら直接追跡
	if (GetAreaID()!= spPlayer->GetAreaID())
	{
		if (IsSightClear(spPlayer->GetPos()))
		{
			// Areaが違っても、近くて間に壁が無ければ直接追跡する
			m_nextMoveState = MoveState::DirectChase;
		}
		// ウェイポイントが無ければ変更しない
		else if(!WayPointManager::Instance().GetWayPoints().empty())
		{
			m_nextMoveState = MoveState::FollowPath;

			m_hasReachedTarget = false;
		}
	}
	else
	{
		m_nextMoveState = MoveState::DirectChase;
	}
}

void EnemyBase::SelectStartWayPoint()
{
	
	// スタート地点
	auto startPoint =
		WayPointManager::Instance().FindNearest(GetPos(), GetAreaID());

	if (!startPoint)
	{
		m_targetWayPointID = -1;
		return;
	}

	m_targetWayPointID = startPoint->GetID();
}

void EnemyBase::ChangeMoveState(const MoveState nextState)
{
	if (m_currentMoveState == nextState)
	{
		return;
	}

	m_currentMoveState = nextState;

	if (m_currentMoveState == MoveState::FollowPath)
	{
		m_targetWayPointID = -1;
		SelectStartWayPoint();
	}
}

void EnemyBase::UpdateFlowGoal()
{

	auto player = m_wpPlayer.lock();

	if (!player)
	{
		return;
	}

	auto currentGoalPoint =WayPointManager::Instance().FindNearest(player->GetPos(),player->GetAreaID());

	if (!currentGoalPoint)
	{
		return;
	}

	WayPointManager::Instance().UpdateFlowGoal(currentGoalPoint->GetID());

}

void EnemyBase::UpdateMove()
{

	if (m_launchFlg)
	{
		return;
	}

	if (m_knockBack != Math::Vector3::Zero)
	{
		// キャラの向き
		auto spPlayer = m_wpPlayer.lock();
		if (!spPlayer)
		{
			return;
		}

		Math::Vector3 toDir = spPlayer->GetPos() - GetPos();
		SetMoveDir(toDir);
		UpdateFacingDirection();

		return;
	}

	if (m_hasReachedTarget)
	{
		PlayIdleAnimation();
	}
	else
	{
		PlayWalkAnimation();
	}

	CanDirectChase();

	ChangeMoveState(m_nextMoveState);

	switch (m_currentMoveState)
	{
	case MoveState::DirectChase:
		UpdateDirectChase();
		break;
	case MoveState::FollowPath:
		UpdateFollowPath();
		break;
	}

	// キャラの向き
	UpdateFacingDirection();
}

void EnemyBase::HoldIdle()
{
	// 空中で止まらないように、落下だけは続ける
	UpdateGravity();

	PlayIdleAnimation();
}

void EnemyBase::ForceKill()
{
	// すでに倒されている敵は、やられモーションの途中なのでそのままにする
	if (m_health.IsDead())
	{
		return;
	}

	m_isForceKilled = true;

	// 拡縮の途中でも、元の大きさに戻してからやられ状態にする
	ResetAttackPulse();

	// HPを0にしてから、ダメージ0の攻撃を当てる
	// (各敵のOnHitは、HPが0ならやられ状態へ切り替える。
	//  ダメージが0なので、ダメージ表示・ヒットエフェクト・ヒットストップは出ない)
	m_health.Kill();

	OnHit(AttackInfo());
}

void EnemyBase::CreateDeathSmoke()
{
	KdEffekseerManager::GetInstance().
		Play("Common/Smoke/Smoke_purple.efkefc",GetPos(),m_smokeEffectScale,0.7f,false);

	// SEを流す
	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Enemy/Death/Death.wav");

}

void EnemyBase::UpdateAttack()
{
	// ターゲットに到達したら攻撃する
	if (m_hasReachedTarget)
	{
		m_attackFlg = true;
	}

	m_attackCooldown -= m_deltaTime;
	if (m_attackCooldown <= 0)
	{
		m_attackCooldown = 0;
	}

	if (m_attackCooldownDuration != 0)
	{
		float rate = m_attackCooldown / m_attackCooldownDuration;

		// もうすぐ攻撃をするとき
		if (rate <= 0.6)
		{
			// モデルを2回、大きくして元に戻す
			UpdateAttackPulse();
		}
		else
		{
			// クールタイムがまだ長い(攻撃が終わった直後など)ときは、拡縮を最初からにしておく
			ResetAttackPulse();
		}
	}
	else if (m_attackFlg)
	{
		// クールタイムが無い敵は、攻撃フラグが立ったらすぐ拡縮を始める
		UpdateAttackPulse();
	}

	// クールタイムがある場合は攻撃しない
	if (m_attackFlg)
	{
		if (m_attackCooldown != 0)
		{
			m_attackFlg = false;
		}
	}
	else
	{
		m_attackCooldown = m_attackCooldownDuration;

		// 攻撃しないことになったので、拡縮も途中でやめて元の大きさに戻す
		ResetAttackPulse();
	}
}

void EnemyBase::UpdateAttackPulse()
{
	// 決められた回数を終えたら、元の大きさのまま攻撃を待つ
	if (m_isScalePulseFinished)
	{
		m_drawScaleRate = 1.0f;
		return;
	}

	m_scalePulseTime += m_deltaTime;

	const float totalTime = kAttackPulseCount * kAttackPulseDuration;

	if (m_scalePulseTime >= totalTime)
	{
		m_isScalePulseFinished = true;
		m_drawScaleRate = 1.0f;
		return;
	}

	// 1回分の中での進み具合(0〜1)
	const float phase = fmodf(m_scalePulseTime, kAttackPulseDuration) / kAttackPulseDuration;

	// 元の大きさ → 一番大きく → 元の大きさ、と滑らかに変える(1回の終わりでちょうど1.0に戻る)
	m_drawScaleRate = 1.0f + kAttackPulseScaleUp * sinf(DirectX::XM_PI * phase);
}

void EnemyBase::ResetAttackPulse()
{
	m_scalePulseTime = 0.0f;
	m_isScalePulseFinished = false;
	m_drawScaleRate = 1.0f;
}

bool EnemyBase::UpdateMeleeAttackCollision(float knockBackPower, float damage, float sphereRadius, float forwardOffset)
{
	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{
		return false;
	}

	// 攻撃が当たっていたら
	if (m_hitTarget)
	{
		return false;
	}

	m_animFrame += 60.0f * m_deltaTime;

	if (m_animFrame <= m_meleeAttack.m_attackTiming.hitStart ||
		m_animFrame >= m_meleeAttack.m_attackTiming.hitEnd)
	{
		return false;
	}

	// 攻撃する位置(胴体の中心)
	Math::Vector3 attackPos = GetBodyCenter();

	// 攻撃する方向
	Math::Vector3 attackDir = m_mWorld.Backward();
	attackDir.y = 0;

	if (attackDir.LengthSquared() <= 0.000001f)
	{
		return false;
	}

	// m_mWorldのBackwardにはScaleが含まれているので、向きだけにしてから、大きさの倍率を掛ける
	attackDir.Normalize();

	// プレイヤーの少し前に出す(前に出す距離と半径は、キャラの大きさに合わせる)
	attackPos += attackDir * (forwardOffset * GetSizeScale());

	DirectX::BoundingSphere sphere;

	sphere.Center = attackPos;
	sphere.Radius = sphereRadius * GetSizeScale();

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	bool hit = false;

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{
		AttackPlayer(spPlayer, knockBackPower, damage);

		m_hitTarget = true;
		hit = true;
	}

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

	return hit;
}

void EnemyBase::UpdateLaunch()
{
	// 着地したら飛び出し終了
	if (IsGrounded())
	{
		m_launchFlg = false;
	}

	Math::Vector3 move = m_launchVec * 60.0f * m_deltaTime;

	AddPendingMove(move);
}

void EnemyBase::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

nlohmann::json EnemyBase::SaveData() const
{
	// 位置・回転・大きさなどの共通データ
	nlohmann::json json = CharacterBase::SaveData();

	// 自分が設置された部屋の名前(選んでいない時は空 = None)
	m_roomLink.SaveData(json);

	return json;
}

void EnemyBase::LoadData(const nlohmann::json& json)
{
	CharacterBase::LoadData(json);

	// キーが無い古いステージは、部屋と紐づけない(None)まま動く
	m_roomLink.LoadData(json);
}

bool EnemyBase::ApplyDamage(const AttackInfo& attackInfo)
{


	// 攻撃前の拡縮の途中で被弾したら、拡縮はやめて元の大きさに戻す
	ResetAttackPulse();

	const bool isDead = m_health.TakeDamage(attackInfo.m_damage);

	AddKnockBack(attackInfo.m_knockBackDir, attackInfo.m_knockBackPower);

	// ダメージが0より上だけ以下の処理をする
	if(attackInfo.m_damage>0)
	{
		// 敵が生きているかでヒットストップ変更
		if (!isDead)
		{
			TimeManager::Instance().StartHitStop(attackInfo.m_hitStop);
		}
		else
		{
			// ヒットストップとスロー演出をする
			TimeManager::Instance().StartHitStop(attackInfo.m_killHitStop);
			TimeManager::Instance().StartSlowMotion(attackInfo.m_SlowScale, attackInfo.m_SlowDuration);
		}

		PlayHitEffect();
		FlyTextManager::Instance().CreateDamateText(attackInfo.m_damage, GetPos(), m_flyTextPath);
		StartOverlay({ 1,1,1 }, 2.0f, m_overlayDuration);
	}



	return isDead;
}

void EnemyBase::AttackPlayer(const std::shared_ptr<Player>& spPlayer, const float knockBackPower, const float damage)
{
	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
	knockBackDir.y = 0;
	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir = knockBackDir;
	attackInfo.m_knockBackPower = knockBackPower;
	attackInfo.m_damage = static_cast<int>(damage);

	spPlayer->OnHit(attackInfo);
}
