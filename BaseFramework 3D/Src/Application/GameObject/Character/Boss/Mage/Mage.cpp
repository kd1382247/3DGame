#include "Mage.h"

#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../Player/Player.h"
#include"../../Enemy/EnemyBase.h"

#include"../../../../../Framework/GameObject/KdGameObjectFactory.h"
#include"../../../../Scene/SceneManager.h"

#include"../../../EnergyBullet/EnergyBulletManager.h"
#include"MageMagicCircle/MageMagicCircleManager.h"
#include"MageMagicSector/MageMagicSectorManager.h"
#include"MageBeam/MageBeamManager.h"
#include"MageBeam/MageBeamDesc.h"

#include"../../../HPBar/BossHPBar/BossHPBar.h"

#include"State/States/MageNormalState.h"
#include"State/States/MageIntroState.h"
#include"State/States/MageDieState.h"

namespace
{
	// Parameterのエフェクト設定 → 魔法円のエフェクト設定
	MageMagicCircleEffect ToCircleEffect(const MageParameter::EffectParam& effect)
	{
		MageMagicCircleEffect result;

		result.fileName = effect.m_fileName;
		result.size = effect.m_size;
		result.speed = effect.m_speed;
		result.startFrame = static_cast<float>(effect.m_startFrame);
		result.endFrame = static_cast<float>(effect.m_endFrame);

		return result;
	}
}

void Mage::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Boss/Mage/Mage.gltf", "Mage",
			Math::Vector3(0.0f, 0.8f, 0.0f), 0.7f, "Mage");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータークラス初期化
		m_parameter.Init();

		const auto& param = m_parameter.GetParam();

		m_health.Init(param.m_maxHP);

		m_reachDistance = param.m_reachDistance;
		m_reachDistanceMargin = param.m_reachDistanceMargin;

		m_attackCooldownDuration = param.m_attackCooldown;
		m_attackCooldown = m_attackCooldownDuration;

		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<MageIntroState>();
	}

	BossBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());

	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(m_parameter.GetParam().m_scale);
}

void Mage::Update()
{
	if (!m_initPos)
	{
		m_roomCenter = GetPos();
		m_initPos = true;
	}

	UpdateGravity();

	UpdateAttack();

	m_stateMachine.Update();
}

void Mage::PostUpdate()
{
	BossBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void Mage::SetUpReference()
{
	BossBase::SetUpReference();
}

void Mage::CreateHPBar()
{
	std::shared_ptr<BossHPBar>hpBar = std::make_shared<BossHPBar>();

	hpBar->SetNamePrateParam(GetFactoryClassName(), 227, 50);
	hpBar->Init();
	hpBar->SetBoss(std::dynamic_pointer_cast<BossBase>(shared_from_this()));

	SceneManager::Instance().AddObject(hpBar);

}

void Mage::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void Mage::PlayWalkAnimation()
{
	PlayAnimation(MageAnimationType::Walk);
}

void Mage::PlayIdleAnimation()
{
	PlayAnimation(MageAnimationType::Idle);
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
	// 倒された後は攻撃を受けない
	if (m_health.IsDead())
	{
		return;
	}

	// 倒された時は、撃破ヒットストップが入って、やられ状態へ切り替わる
	if (ApplyDamage(attackInfo))
	{
		m_stateMachine.ChangeState<MageDieState>();
	}
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

void Mage::CancelAllAttacks()
{
	MageMagicCircleManager::Instance().CancelAll();
	MageMagicSectorManager::Instance().CancelAll();
	MageBeamManager::Instance().CancelAll();
	EnergyBulletManager::Instance().CancelAll();

	// 召喚の準備中だった座標も捨てる
	m_summonPositions.clear();
}

MageAttackPattern Mage::SelectAttackPattern()
{
	std::vector<float> weights(static_cast<size_t>(MageAttackPattern::Num), 1.0f);

	// HP50%を超えている間は、後半専用パターンを除外する
	if (!IsSecondPhase())
	{
		weights[static_cast<size_t>(MageAttackPattern::NovaCircle)] = 0.0f;
		weights[static_cast<size_t>(MageAttackPattern::Beam)] = 0.0f;
	}

	// プレイヤーに到達していない場合は近接系の攻撃は抽選されない
	if (!HasReachedTarget())
	{
		weights[static_cast<size_t>(MageAttackPattern::ForwardSector)] = 0.0f;
		weights[static_cast<size_t>(MageAttackPattern::NovaCircle)] = 0.0f;
	}
	else
	{
		weights[static_cast<size_t>(MageAttackPattern::Bolt)] = 0.0f;
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
	// 呼び出す敵の数と、ボスを囲む円の半径(Parameterで調整できる)
	const int enemyCount = m_parameter.GetParam().m_summon.m_count;
	const float offsetRadius = m_parameter.GetParam().m_summon.m_radius;

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

std::shared_ptr<MageMagicCircle> Mage::CastTargetCircle()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return nullptr;
	}

	Math::Vector3 targetPos = spPlayer->GetPos();

	if (!spPlayer->IsGrounded())
	{
		targetPos.y = spPlayer->GetGroundYPos();
	}

	const auto& param = m_parameter.GetParam().m_targetCircle;

	MageMagicCircleDesc desc;

	desc.pos = targetPos;
	desc.radius = param.m_radius;
	desc.telegraphTime = param.m_telegraph;
	desc.damage = param.m_damage;
	desc.effect = ToCircleEffect(param.m_effect);

	return MageMagicCircleManager::Instance().CreateMagicCircle(desc);
}

std::shared_ptr<MageMagicSector> Mage::CastForwardSector()
{
	const auto& param = m_parameter.GetParam().m_forwardSector;

	MageMagicSectorDesc desc;

	desc.pos = GetPos();
	desc.dir = m_mWorld.Backward();
	desc.angleDeg = param.m_angle;
	desc.radius = param.m_radius;
	desc.telegraphTime = param.m_telegraph;
	desc.damage = param.m_damage;

	const auto& effect = param.m_effect;

	desc.effect.fileName = effect.m_fileName;
	desc.effect.size = effect.m_size;
	desc.effect.speed = effect.m_speed;
	desc.effect.startFrame = static_cast<float>(effect.m_startFrame);
	desc.effect.endFrame = static_cast<float>(effect.m_endFrame);
	desc.effect.rotation = GetEffectRotation(effect.m_rotationY);

	return MageMagicSectorManager::Instance().CreateMagicSector(desc);
}

void Mage::FireBolt()
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	const auto& param = m_parameter.GetParam().m_bolt;

	Math::Vector3 spawnPos = GetPos() + Math::Vector3(0.0f, param.m_spawnHeight, 0.0f);

	Math::Vector3 dir = (spPlayer->GetPos() + Math::Vector3(0.0f, param.m_aimHeight, 0.0f)) - spawnPos;

	if (dir.LengthSquared() > 0.000001f)
	{
		dir.Normalize();
	}

	EnergyBulletManager::Instance().CreateEnergyBullet(
		std::dynamic_pointer_cast<CharacterBase>(shared_from_this()),
		spawnPos,
		dir,
		param.m_speed,
		param.m_radius,
		param.m_damage,
		param.m_knockBack,
		param.m_lifeTime);
}

std::shared_ptr<MageMagicCircle> Mage::CastNovaCircle()
{
	const auto& param = m_parameter.GetParam().m_novaCircle;

	MageMagicCircleDesc desc;

	desc.pos = GetPos();
	desc.radius = param.m_radius;
	desc.telegraphTime = param.m_telegraph;
	desc.damage = param.m_damage;
	desc.effect = ToCircleEffect(param.m_effect);

	return MageMagicCircleManager::Instance().CreateMagicCircle(desc);
}

Math::Vector3 Mage::GetEffectRotation(const float value)const
{
	// エフェクト側の初期正面とキャラクターの正面が180度ズレているため補正
	Math::Vector3 rotation = GetRotation();
	rotation.y += value;

	return rotation;
}

std::shared_ptr<MageBeam> Mage::FireBeam(const MageParameter::Beam& param)
{
	MageBeamDesc desc;

	GetBeamPose(param, desc.pos, desc.dir);

	desc.length = param.m_length;
	desc.width = param.m_width;
	desc.damage = param.m_damage;
	desc.duration = param.m_duration;
	desc.hitInterval = param.m_hitInterval;

	const auto& effect = param.m_beamEffect;

	desc.effect.fileName = effect.m_fileName;
	desc.effect.size = effect.m_size;
	desc.effect.speed = effect.m_speed;
	desc.effect.startFrame = static_cast<float>(effect.m_startFrame);
	desc.effect.endFrame = static_cast<float>(effect.m_endFrame);
	desc.effect.rotation = GetEffectRotation(effect.m_rotationY);
	desc.effect.effectPos = GetAttackEffectPos(effect);

	return MageBeamManager::Instance().CreateBeam(desc);
}

void Mage::GetBeamPose(const MageParameter::Beam& param, Math::Vector3& outPos, Math::Vector3& outDir)const
{
	// ボス中心ではなく、ボスの前方に押し出した位置を基準にする
	outDir = m_mWorld.Backward();
	outPos = GetPos() + outDir * param.m_forwardOffset;
}

Math::Vector3 Mage::GetAttackEffectPos(const MageParameter::EffectParam& effect)const
{
	return (GetPos() + Math::Vector3(0.0f, effect.m_heightOffset, 0.0f)) + m_mWorld.Backward() * effect.m_forwardOffset;
}

std::weak_ptr<KdEffekseerObject> Mage::PlayAttackEffect(const MageParameter::EffectParam& effect)const
{
	return KdEffekseerManager::GetInstance().Play(
		effect.m_fileName,
		GetAttackEffectPos(effect),
		effect.m_size,
		effect.m_speed,
		false,
		effect.m_startFrame,
		effect.m_endFrame,
		GetEffectRotation(effect.m_rotationY));
}
