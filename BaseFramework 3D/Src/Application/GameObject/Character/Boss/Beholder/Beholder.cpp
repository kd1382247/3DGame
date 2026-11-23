#include "Beholder.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../HPBar/BossHPBar/BossHPBar.h"

#include"../../Player/Player.h"

#include"State/States/BeholderNormalState.h"
#include"State/States/BeholderDieState.h"
#include"State/States/BeholderDamageState.h"
#include"State/States/BeholderDizzyState.h"


namespace
{
	// モデルの目玉のローカルでの正面
	constexpr Math::Vector3 EyeForwardLocal = { 0,-1,0 };

	// 目玉の上限の回転角度
	constexpr float MaxEyeAngle = 30.0f;

	// 混ぜる割合の速度
	constexpr float EyeFollowSpeed = 10.0f;

	// 元の位置に復帰するまでの時間
	constexpr float FloatRecoveryDuration = 1.0f;

}

void Beholder::Init()
{
	if (!m_spModel)
	{
		// パラメータ初期化(当たり判定の大きさに使うので、モデル生成より先に読み込む)
		m_parameter.Init();

		const auto& param = m_parameter.GetParam();

		InitCharacterModel("Asset/Models/Boss/Beholder/Beholder.gltf", "Beholder",
			Math::Vector3(0.0f, param.m_bumpSphereOffsetY, 0.0f), param.m_bumpSphereRadius, "Beholder");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		m_health.Init(param.m_maxHP);

		// 当たり判定(押し戻し)球の大きさをパラメータから反映
		SetBumpSphere(param.m_bumpSphereOffsetY, param.m_bumpSphereRadius);

		// 重力加速度をパラメータから反映
		SetGravityAcceleration(param.m_gravityAcceleration);

		m_reachDistance = param.m_reachDistance;
		m_reachDistanceMargin = param.m_reachDistanceMargin;

		m_attackCooldownDuration = param.m_attackCooldown;
		m_attackCooldown = m_attackCooldownDuration;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<BeholderNormalState>();
	}

	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });

}

void Beholder::Update()
{
	if (GetAsyncKeyState('F') & 0x8000)
	{
		m_isEyeTracking = true;
	}
	if (GetAsyncKeyState('G') & 0x8000)
	{
		m_isEyeTracking = false;
	}

	UpdateAttack();


	m_stateMachine.Update();
}

void Beholder::PostUpdate()
{
	BossBase::PostUpdate();
	
	UpdateAnimation();

	UpdateEyeRotationX();

	DrawBumpDebugSphere();

}

void Beholder::OnHit(const AttackInfo attackInfo)
{
	// 倒された後は攻撃を受けない
	if (m_health.IsDead())
	{
		return;
	}

	// 倒された時は、撃破ヒットストップが入って、やられ状態へ切り替わる
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<BeholderDieState>();
	}
	else
	{
		// 混乱中じゃなかったら
		if(!m_stateMachine.IsState<BeholderDizzyState>())
		{
			m_stateMachine.ChangeState<BeholderDamageState>();
		}
	}
}

void Beholder::UpdateAttack()
{
	m_attackCooldown -= m_deltaTime;

	if (m_attackCooldown <= 0.0f)
	{
		m_attackFlg = true;
	}
}

void Beholder::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

void Beholder::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

Math::Quaternion Beholder::CalcEyeTargetRotation(const Math::Matrix& eyeWorldMat)
{

	auto spPlayer = m_wpPlayer.lock();
	if (!spPlayer)
	{ 
		return Math::Quaternion::Identity;
	}

	// 高さのオフセット値を含んだ位置
	Math::Vector3 playerPos = spPlayer->GetBumpSphere().Center;

	// 目の位置を取得
	Math::Vector3 eyePos = eyeWorldMat.Translation();

	// 向きたい方向ベクトルを計算
	Math::Vector3 toDir = playerPos - eyePos;
	toDir.Normalize();

	// 目から見た方向に変換(目の正面は(0,-1,0))
	Math::Vector3 toLocal = Math::Vector3::TransformNormal(toDir, eyeWorldMat.Invert());
	toLocal.Normalize();

	// ローカルの正面と目標の内積をとる
	float cosAngle = std::clamp(EyeForwardLocal.Dot(toLocal), -1.0f, 1.0f);

	float angle = std::min( acos(cosAngle),DirectX::XMConvertToRadians(MaxEyeAngle));

	// どの軸で回すかを外積でとる
	Math::Vector3 axis = EyeForwardLocal.Cross(toLocal);

	if (axis.Length() < 0.0001f)
	{
		return Math::Quaternion::Identity;
	}

	axis.Normalize();

	return Math::Quaternion::CreateFromAxisAngle(axis, angle);
}

void Beholder::UpdateEyeRotationX()
{

	// 目標の回転
	Math::Quaternion targetQuaternion = Math::Quaternion::Identity;

	// モデルの目のボーンを取得
	KdModelWork::Node* pNode = m_spModel->FindWorkNode("EyeBallCTRL");

	if (!pNode) { return; }

	// 追従する
	if (m_isEyeTracking)
	{
		Math::Matrix eyeWorldMat = pNode->m_worldTransform * GetMatrix();
		targetQuaternion = CalcEyeTargetRotation(eyeWorldMat);
	}

	m_eyeQuaternion =
		Math::Quaternion::Slerp(m_eyeQuaternion, targetQuaternion, 1 - std::exp(-EyeFollowSpeed * m_deltaTime));

	Math::Matrix eyeRotMat = Math::Matrix::CreateFromQuaternion(m_eyeQuaternion);

	pNode->m_localTransform = eyeRotMat * pNode->m_localTransform;

	m_spModel->CalcNodeMatrices();

}

void Beholder::PlayAnimation(BeholderAnimationType type)
{
	m_animation.Play(type);
}

void Beholder::CreateSparkBullet()
{
	const auto& spark = m_parameter.GetParam().m_sparkBullet;
	
	// 弾の生成
	m_sparkAttack.CreateSparkBullet(
		std::dynamic_pointer_cast<CharacterBase>(shared_from_this()), spark);
}

void Beholder::FireSparkBullet()
{
	m_sparkAttack.StartFiring();
}

bool Beholder::IsStandbySparkBullet()
{
	return m_sparkAttack.IsStandbySparkBullet();
}

void Beholder::CancelSparkBullet()
{
	m_sparkAttack.CancelAll();
}

void Beholder::CreateChargeSparkBullet()
{
	const auto& param = m_parameter.GetParam().m_chargeSparkBullet;

	m_chargeShot.CreateChargeSparkBullet(
		std::dynamic_pointer_cast<CharacterBase>(shared_from_this()), param);
}

void Beholder::FireChargeSparkBullet()
{
	m_chargeShot.StartFiring();
}

bool Beholder::IsStandbyChargeSparkBullet()
{
	return m_chargeShot.IsStandbySparkBullet();
}

void Beholder::CancelChargeSparkBullet()
{
	m_chargeShot.Cancel();
}

void Beholder::StartDizzy()
{
	// 落下状態にする
	m_dizzyState = DizzyState::Falling;

	m_dizzyTime = m_dizzyDuration;

	m_floatRecoveryTime = 0.0f;

	m_t = 0.0f;

	// 今いる高さを保存
	m_originalPosY = GetPos().y;

	m_dizzyCount = 0;
}

void Beholder::UpdateDizzy()
{
	switch (m_dizzyState)
	{
	case Beholder::DizzyState::Falling:
		UpdateFalling();
		break;
	case Beholder::DizzyState::Dizzy:
		UpdateDizzyTimeCountDown();
		break;
	case Beholder::DizzyState::Returning:
		UpdateReturnIngHeight();
		break;
	}
}

void Beholder::DizzyCountUp()
{
	m_dizzyCount++;

	if (m_dizzyCount >= m_dizzyCountMax)
	{
		m_stateMachine.ChangeState<BeholderDizzyState>();
	}
}

void Beholder::CreateHPBar()
{
	std::shared_ptr<BossHPBar>hpBar = std::make_shared<BossHPBar>();

	hpBar->SetNamePrateParam(GetFactoryClassName(), 227, 50);
	hpBar->Init();
	hpBar->SetBoss(std::dynamic_pointer_cast<BossBase>(shared_from_this()));

	SceneManager::Instance().AddObject(hpBar);
}

void Beholder::UpdateFalling()
{
	UpdateGravity();

	// 地面に着いたら混乱状態にする
	if (IsGrounded())
	{
		m_dizzyState = DizzyState::Dizzy;
	}
}

void Beholder::UpdateDizzyTimeCountDown()
{
	// 地面に着いたら時間を減らしていく
	m_dizzyTime -= m_deltaTime;

	// 時間が0以下になったら、元の高さに戻る状態にする
	if (m_dizzyTime <= 0.0f)
	{
		m_dizzyState = DizzyState::Returning;

		m_fallStartY = GetPos().y;

		PlayAnimation(BeholderAnimationType::Idle);
	}
}

void Beholder::UpdateReturnIngHeight()
{

	m_floatRecoveryTime += m_deltaTime;
	
	m_t = std::clamp(m_floatRecoveryTime / FloatRecoveryDuration, 0.0f, 1.0f);
	
	float currentPosY = GetPos().y;

	// 今フレームで上がるYを求める
	float nextPosY = std::lerp(m_fallStartY, m_originalPosY, m_t);

	// Yの移動量を求める
	float moveY = nextPosY - currentPosY;

	if (m_t >= 1.0f)
	{
		m_dizzyState = DizzyState::FloatRecovered;
		moveY = m_originalPosY - currentPosY;
	}

	AddPendingMove({0.0f,moveY,0.0f});

}

void Beholder::PlayWalkAnimation()
{
	m_animation.Play(BeholderAnimationType::Walk);
}

void Beholder::PlayIdleAnimation()
{
	m_animation.Play(BeholderAnimationType::Idle);
}

void Beholder::DrawParameterInspector()
{
	m_parameter.DrawInspecter();

	// 当たり判定球の変更をすぐに反映
	const auto& bumpParam = m_parameter.GetParam();
	SetBumpSphere(bumpParam.m_bumpSphereOffsetY, bumpParam.m_bumpSphereRadius);

	// クールダウンの変更もすぐに反映
	m_attackCooldownDuration = bumpParam.m_attackCooldown;
}
