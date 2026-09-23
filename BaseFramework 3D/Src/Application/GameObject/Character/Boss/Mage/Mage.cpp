#include "Mage.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../Player/Player.h"
#include"../../Enemy/EnemyBase.h"

#include"../../../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../EnergyBullet/EnergyBulletManager.h"
#include"../../../MageMagicCircle/MageMagicCircleManager.h"

#include"State/States/MageNormalState.h"

void Mage::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Boss/Mage/Mage.gltf", "Mage",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "Mage");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータークラス初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<MageNormalState>();
	}

	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(1.5f);
}

void Mage::Update()
{
	UpdateAttack();

	m_stateMachine.Update();
}

void Mage::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Mage::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Mage::UpdateAnimation()
{
	m_animation.Update(m_deltaTime);
}

void Mage::PlayAnimation(MageAnimationType type)
{
	m_animation.Play(type);
}

void Mage::RePlayAnimation(MageAnimationType type)
{
	m_animation.RePlay(type);
}

void Mage::OnHit(const AttackInfo attackInfo)
{
	if (m_isClone)
	{
		// 分身は偽物なので、1発当たれば即座に消える
		Destroy();
		return;
	}

	BossBase::OnHit(attackInfo);
}

void Mage::UpdateAttack()
{
	m_attackCooldown -= m_deltaTime;

	if (m_attackCooldown <= 0.0f)
	{
		m_attackFlg = true;
	}
}

void Mage::EndAttack()
{
	m_attackFlg = false;
	m_attackCooldown = m_attackCooldownDuration;
}

MageAttackPattern Mage::SelectAttackPattern()
{
	std::vector<float> weights(static_cast<size_t>(MageAttackPattern::Num), 1.0f);

	// HP50%を超えている間は、後半専用パターンを除外する
	if (!IsSecondPhase())
	{
		weights[static_cast<size_t>(MageAttackPattern::CirculeAreaAttack)] = 0.0f;
		weights[static_cast<size_t>(MageAttackPattern::Clone)] = 0.0f;
	}

	// 分身自身はこれ以上分身を出さない(無限増殖防止)
	if (m_isClone)
	{
		weights[static_cast<size_t>(MageAttackPattern::Clone)] = 0.0f;
	}

	int index = LotteryPattern(weights);

	if (index < 0)
	{
		return MageAttackPattern::Bolt;
	}

	return static_cast<MageAttackPattern>(index);
}

void Mage::SummonEnemy()
{
	std::string enemyName;

	switch (KdRandom::GetInt(0, 3))
	{
	case 0: enemyName = "Cactas";     break;
	case 1: enemyName = "Mushroom";   break;
	case 2: enemyName = "Slime";      break;
	case 3: enemyName = "TurtleShell"; break;
	}

	auto obj = KdGameObjectFactory::Instance().CreateGameObject(enemyName);

	auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

	if (!enemy)
	{
		return;
	}

	// Mageの少し前方に召喚する
	Math::Vector3 spawnPos = GetPos() + m_mWorld.Backward() * 1.5f;

	enemy->Init();
	enemy->SetPos(spawnPos);
	enemy->SetPrevPos(spawnPos);
	enemy->SetUpReference();

	SceneManager::Instance().AddObject(enemy);
}

void Mage::CastMagicCircle()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	MageMagicCircleManager::Instance().CreateMagicCircle(
		spPlayer->GetPos(),
		/*radius=*/2.0f,
		/*telegraphTime=*/0.8f,
		/*damage=*/m_parameter.GetParam().m_attackPow,
		"Tornado/Tornado.efkefc",
		0.5,
		2.0f,
		0,
		165);
}

void Mage::ForwardAreaAttack()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 forward = m_mWorld.Backward();
	forward.y = 0.0f;

	if (forward.LengthSquared() > 0.000001f)
	{
		forward.Normalize();
	}

	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f) + forward * 1.5f;
	sphere.Radius = 1.2f;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	if (spPlayer->Intersects(sphereInfo, nullptr))
	{
		Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
		knockBackDir.y = 0.0f;

		if (knockBackDir.LengthSquared() > 0.000001f)
		{
			knockBackDir.Normalize();
		}

		AttackInfo attackInfo;

		attackInfo.damage = m_parameter.GetParam().m_attackPow;
		attackInfo.knockBackDir = knockBackDir;
		attackInfo.knockBackPower = 0.2f;

		spPlayer->OnHit(attackInfo);
	}
}

void Mage::FireBolt()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 spawnPos = GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f);

	Math::Vector3 dir = (spPlayer->GetPos() + Math::Vector3(0.0f, 0.8f, 0.0f)) - spawnPos;

	if (dir.LengthSquared() > 0.000001f)
	{
		dir.Normalize();
	}

	EnergyBulletManager::Instance().CreateEnergyBullet(
		spawnPos, dir, /*speed=*/0.12f, /*radius=*/0.2f,
		/*damage=*/m_parameter.GetParam().m_attackPow, /*knockBackPower=*/0.15f, /*lifeTime=*/4.0f);
}

void Mage::CirculeAreaAttack()
{
	
	MageMagicCircleManager::Instance().CreateMagicCircle(
		GetPos(),
		/*radius=*/6.0f,
		/*telegraphTime=*/1.5f,
		/*damage=*/m_parameter.GetParam().m_attackPow,
		"Salamander/Salamander.efkefc",
		1.2f,
		1.0f,
		0,
		45);
}

void Mage::SpawnClones()
{
	constexpr int cloneCount = 3;
	constexpr float offsetRadius = 2.0f;

	for (int i = 0; i < cloneCount; i++)
	{
		auto obj = KdGameObjectFactory::Instance().CreateGameObject("Mage");

		auto clone = std::dynamic_pointer_cast<Mage>(obj);

		if (!clone)
		{
			continue;
		}

		float angle = i * (2.0f * 3.1415926535f / cloneCount);

		Math::Vector3 offset;
		offset.x = std::sinf(angle) * offsetRadius;
		offset.y = 0.0f;
		offset.z = std::cosf(angle) * offsetRadius;

		clone->SetIsClone(true);
		clone->Init();
		clone->SetPos(GetPos() + offset);
		clone->SetUpReference();

		SceneManager::Instance().AddObject(clone);
	}
}
