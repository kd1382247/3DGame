#include "Player.h"

#include"../../Camera/CameraBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../System/CollisionManager/CollisionManager.h"

#include"../../../Scene/SceneManager.h"

#include"../../FlyText/FlyTextManager.h"

#include"../../HPBar/PlayerHPBar/PlayerHPBar.h"

#include"State/States/PlayerNormalState.h"
#include"State/States/PlayerAttackState.h"
#include"State/States/PlayerSpecialMoveState.h"
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

		// 剣の軌跡クラス初期化
		m_playerSwordTrail.Init();

		// パラメータークラス初期化
		m_parameter.Init();

		// 各アクションクラスには、自分に必要なパラメータだけを渡す
		m_playerAttack.Init(m_parameter.GetAttack());
		m_playerSpecialMove.Init(m_parameter.GetSpecialMove());

		m_hp = m_parameter.GetBody().m_maxHP;
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
	UpdateDebugCommand();

	// 操作入力
	UpdateInput();

	// 各ステートの更新
	m_stateMachine.Update();

	// コンボ猶予(受付)の更新(通常状態の間だけ進める)
	if (m_stateMachine.IsState<PlayerNormalState>())
	{
		m_playerAttack.UpdateComboGrace(m_input, m_deltaTime);
	}

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

	m_playerSwordTrail.UpdateTrail(*this);
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

void Player::DrawEffect()
{
	auto spTrailPoly = m_playerSwordTrail.GetTrailPolygon();

	if (!spTrailPoly)
	{
		return;
	}

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*spTrailPoly, Math::Matrix::Identity);
}

void Player::UpdateAttackFrame()
{
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

void Player::UpdateDebugCommand()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		m_hp += 10;
		if (m_hp > GetMaxHP())
		{
			m_hp = static_cast<float>(GetMaxHP());
		}
	}
}

//================================
// 攻撃判定
//================================

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

	// スフィアとダメージを決める
	DirectX::BoundingSphere sphere;
	float damage = 0.0f;

	if(type==AttackType::NormalAttack)
	{
		sphere = CreateAttackSphere();
		damage = m_playerAttack.GetCurrentAttackPower();
	}
	else
	{
		sphere = CreateSpecialMoveSphere();
		damage = m_playerSpecialMove.GetAttackPower();
	}

	// 当たった相手へダメージを与える
	constexpr float knockBackPower = 0.1f;
	m_hitChecker.Check(*this, sphere, damage, knockBackPower);

	m_pDebugWire->AddDebugSphere(sphere.Center, sphere.Radius, kGreenColor);

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

//================================
// 入力・移動
//================================

void Player::UpdateInput()
{
	m_input.Update(m_deltaTime);
	m_playerGuard.Update(m_input);
}

void Player::UpdateMove()
{
	ApplyCameraRelativeMove(GetMoveSpeed());

	UpdateFacingDirection();
}

void Player::UpdateAttackMove()
{
	ApplyCameraRelativeMove(m_playerAttack.GetMoveSpeed());

	FacingDirectionToCamera();
}

void Player::UpdateGravity()
{
	const float gravityAcceleration = m_parameter.GetBody().m_gravityAcceleration;

	m_gravity += gravityAcceleration * m_deltaTime;

	Math::Vector3 gravityMove = { 0.0f,-m_gravity * m_deltaTime ,0.0f };

	AddPendingMove(gravityMove);
}

bool Player::GetCameraForward(Math::Vector3& outDir) const
{
	auto spCamera = m_wpCamera.lock();
	if (!spCamera)
	{
		return false;
	}

	Math::Matrix camRotYMat = spCamera->GetRotationYMatrix();

	// カメラから見て前方向
	Math::Vector3 forward = Math::Vector3::TransformNormal(Math::Vector3::Backward, camRotYMat);

	forward.y = 0;
	if (forward.LengthSquared() <= 0.000001f)
	{
		return false;
	}

	forward.Normalize();

	outDir = forward;

	return true;
}

void Player::FacingDirectionToCamera()
{
	// カメラから見て前方向に向かせたい
	Math::Vector3 toDir;
	if (!GetCameraForward(toDir))
	{
		return;
	}

	// 現在向いている方向
	Math::Vector3 nowDir = m_mWorld.Backward();

	nowDir.y = 0;

	if (nowDir.LengthSquared() <= 0.000001f)
	{
		return;
	}
	nowDir.Normalize();

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

void Player::ApplyCameraRelativeMove(const float speed)
{
	auto spCamera = m_wpCamera.lock();

	if (!spCamera)
	{
		return;
	}

	Math::Matrix camRotYMat = spCamera->GetRotationYMatrix();

	// 入力方向(生の向き)を、カメラの向きに合わせて回転させる
	// ※入力そのものは書き換えないので、同じフレームに何度呼んでも結果は変わらない
	Math::Vector3 dir = Math::Vector3::TransformNormal(m_input.GetMoveDir(), camRotYMat);

	// キャラを進む向きへ向けるために、CharacterBase側へ渡しておく(UpdateFacingDirectionが使う)
	SetMoveDir(dir);

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

//================================
// 被弾
//================================

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
		// 攻撃中・必殺技中は怯まない
		if (!m_stateMachine.IsState<PlayerAttackState>() &&
		    !m_stateMachine.IsState<PlayerSpecialMoveState>())
		{
			m_stateMachine.ChangeState<PlayerDamageState>();
		}
	}

	StartOverlay({ 1,0,0 }, 0.8f,m_overlayDuration);

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage,GetPos(), m_flyTextPath);

	AddKnockBack(attackInfo.knockBackDir,attackInfo.knockBackPower);
}

//================================
// アクション
//================================

void Player::PlayAnimation(PlayerAnimationType type)
{
	m_animation.Play(type);
}

void Player::ApplyActionTiming(const PlayerActionTiming& timing)
{
	m_attackTiming.hitStart = timing.hitStart;
	m_attackTiming.hitEnd = timing.hitEnd;

	m_playerSwordTrail.SetTrailTiming(timing.trailStart, timing.trailEnd);
}

void Player::StartJump()
{
	m_playerAttack.ResetCombo();

	SetIsGrounded(false);

	// 重力をジャンプ力ぶん減らして、上向きに飛び出させる
	const float jumpPower = m_parameter.GetJump().m_jumpPow;

	SetGravity(GetGravity() - (jumpPower * 60.0f));
}

void Player::StartCurrentAttack()
{
	// 前の攻撃で当たった相手の記録を消す
	m_hitChecker.Clear();

	m_playerAttack.StartAttack();

	const PlayerAttack::AttackData& attackData = m_playerAttack.GetCurrentAttackData();

	m_playerSwordTrail.StartTrail();
	ApplyActionTiming(attackData.timing);

	PlayAnimation(attackData.animation);

	// フレームを0に
	m_animFrame = 0.0f;
}

void Player::EndAttack()
{
	m_playerSwordTrail.EndTrail();
}

void Player::StartSpecialMove()
{
	m_playerAttack.ResetCombo();

	// 攻撃がHitした敵リストをクリア
	m_hitChecker.Clear();
	m_hitChecker.ResetRehitTimer(m_playerSpecialMove.GetHitCooldownDuration());

	// 移動する方向を決める(カメラの前方向)
	Math::Vector3 moveDir = Math::Vector3::Zero;
	GetCameraForward(moveDir);
	m_playerSpecialMove.SetMoveDir(moveDir);

	FacingDirectionToCamera();

	m_playerSwordTrail.StartTrail();
	ApplyActionTiming(m_playerSpecialMove.GetTiming());

	m_animFrame = 0.0f;
}

void Player::UpdateSpecialMove()
{

	if (m_animFrame <= m_attackTiming.hitStart || m_animFrame >= m_attackTiming.hitEnd)
	{
		return;
	}

	AddPendingMove(m_playerSpecialMove.CalcMoveVector(m_deltaTime));

	m_hitChecker.UpdateRehit(m_deltaTime, m_playerSpecialMove.GetHitCooldownDuration());
}

void Player::EndSpecialMove()
{
	m_playerAttack.ResetCombo();
	m_playerSwordTrail.EndTrail();
}
