#include "Beholder.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"State/States/BeholderNormalState.h"

#include"SparkBullet/BeholderSparkBulletManager.h"
#include"SparkBullet/BeholderSparkBulletDesc.h"

namespace
{
	// 追従弾の軌道が膨らむ向き[°](パラメータにはせず、プログラム側で固定する。45°なら右上に膨らむ)
	constexpr float SparkBulletCurveAngle = 45.0f;
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

	static bool key = false;

	if (GetAsyncKeyState('F') & 0x8000)
	{
		if (!key)
		{
			FireBolt();
			key = true;
		}
	}
	else
	{
		key = false;
	}

	UpdateAttack();

	UpdateGravity();

	m_stateMachine.Update();
}

void Beholder::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere();

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

void Beholder::PlayAnimation(BeholderAnimationType type)
{
	m_animation.Play(type);
}

void Beholder::FireBolt()
{
	const auto& param = m_parameter.GetParam();
	const auto& spark = param.m_sparkBullet;

	BeholderSparkBulletDesc desc;

	desc.pos = GetPos() + Math::Vector3(0.0f,param.m_bumpSphereOffsetY,0.0f);

	// 軌道が膨らむ向きは、コード側で固定
	desc.curveAngle = SparkBulletCurveAngle;

	// パラメータをセット
	desc.curveRate = spark.m_curveRate;
	desc.damage = spark.m_damage;
	desc.knockBackPower = spark.m_knockBack;
	desc.speed = spark.m_speed;
	desc.trackingstopDistance = spark.m_trackingStopDistance;
	desc.vanishT = spark.m_vanishT;
	desc.radius = spark.m_radius;
	desc.reflectSpeed = spark.m_reflectSpeed;

	desc.effect.size = spark.m_effect.m_size;
	desc.effect.speed = spark.m_effect.m_speed;
	desc.effect.startFrame = static_cast<float>(spark.m_effect.m_startFrame);
	desc.effect.endFrame = static_cast<float>(spark.m_effect.m_endFrame);

	BeholderSparkBulletManager::Instance().CreateBolt(
		std::dynamic_pointer_cast<CharacterBase>(shared_from_this()), desc);
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
