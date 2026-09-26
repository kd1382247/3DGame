#include "Mage.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../Player/Player.h"
#include"../../Enemy/EnemyBase.h"

#include"../../../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../EnergyBullet/EnergyBulletManager.h"
#include"../../../MageMagicCircle/MageMagicCircleManager.h"
#include"../../../MageMagicSector/MageMagicSectorManager.h"
#include"../../../MageBeam/MageBeamManager.h"


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
		weights[static_cast<size_t>(MageAttackPattern::NovaCircle)] = 0.0f;
	}

	int index = LotteryPattern(weights);

	if (index < 0)
	{
		return MageAttackPattern::Bolt;
	}

	return static_cast<MageAttackPattern>(index);
}

void Mage::PrepareSummonPositions()
{
	constexpr int enemyCount = 5;
	constexpr float offsetRadius = 4.0f;

	m_summonPositions.clear();

	for (int i = 0; i < enemyCount; i++)
	{
		// ボスを囲むように、円周上に等間隔で配置する
		float angle = i * (2.0f * 3.1415926535f / enemyCount);

		Math::Vector3 offset;
		offset.x = std::sinf(angle) * offsetRadius;
		offset.y = 0.0f;
		offset.z = std::cosf(angle) * offsetRadius;

		Math::Vector3 spawnPos = GetPos() + offset;

		m_summonPositions.push_back(spawnPos);

		// 出現前の煙エフェクト
		KdEffekseerManager::GetInstance().
			Play("Smoke/Smoke_White.efkefc", spawnPos + Math::Vector3(0.0f, 0.5f, 0.0f), 0.3f, 1.0f, false);
	}
}

void Mage::SpawnEnemies()
{
	for (const auto& spawnPos : m_summonPositions)
	{
		std::string enemyName;

		switch (KdRandom::GetInt(0, 5))
		{
		case 0: enemyName = "Cactas";     break;
		case 1: enemyName = "Mushroom";   break;
		case 2: enemyName = "Slime";      break;
		case 3: enemyName = "TurtleShell"; break;
		case 4: enemyName = "StarFish"; break;
		case 5: enemyName = "Bomb"; break;
		}

		auto obj = KdGameObjectFactory::Instance().CreateGameObject(enemyName);

		auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

		if (!enemy)
		{
			continue;
		}

		enemy->Init();
		enemy->SetPos(spawnPos);
		enemy->SetPrevPos(spawnPos);
		enemy->SetUpReference();

		SceneManager::Instance().AddObject(enemy);
	}

	m_summonPositions.clear();
}

void Mage::CastTargetCircle()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	Math::Vector3 targetPos = spPlayer->GetPos();

	if (!spPlayer->IsGrounded())
	{
		targetPos.y = spPlayer->GetGroundYPos();
	}

	MageMagicCircleManager::Instance().CreateMagicCircle(
		targetPos,
		/*radius=*/2.5f,
		/*telegraphTime=*/0.8f,
		/*damage=*/m_parameter.GetParam().m_attackPow,
		"Tornado/Tornado.efkefc",
		0.4,
		1.0f,
		0,
		90);
}

void Mage::CastForwardSector()
{
	MageMagicSectorManager::Instance().CreateMagicSector(
		GetPos(),
		m_mWorld.Backward(),
		100,
		6.5f,
		0.8f,
		10,
		"Sword/Sword.efkefc",
		0.8f,
		1.5f,
		0,
		80
	);
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

void Mage::CastNovaCircle()
{
	MageMagicCircleManager::Instance().CreateMagicCircle(
		GetPos(),
		/*radius=*/6.0f,
		/*telegraphTime=*/1.2f,
		/*damage=*/m_parameter.GetParam().m_attackPow,
		"Salamander/Salamander.efkefc",
		1.2f,
		1.0f,
		0,
		45);
}

Math::Vector3 Mage::GetBeamEffectRotation()const
{
	// エフェクト側の初期正面とキャラクターの正面が180度ズレているため補正
	Math::Vector3 rotation = GetRotation();
	rotation.y += 180.0f;

	return rotation;
}

std::shared_ptr<MageBeam> Mage::FireBeam(const Math::Vector3& pos, const Math::Vector3& dir, float length, float width,const Math::Vector3&effectPos)
{
	return MageBeamManager::Instance().CreateBeam(
		pos,
		dir,
		length,
		width,
		/*damage=*/m_parameter.GetParam().m_attackPow,
		/*duration=*/3.0f,
		// TODO: 実在するレーザー系のエフェクトアセットに差し替える
		"Beam/Beam.efkefc",
		0.6f,
		1.0f,
		130,
		250,
		GetBeamEffectRotation(),
		effectPos);
}
