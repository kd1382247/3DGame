#include "ChestMonster.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../../System/TimeManager/TimeManager.h"
#include"../../../../../Framework/Effekseer/KdEffekseerManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"

#include"../../../FlyText/FlyTextManager.h"

#include"State/States/ChestMonsterNormalState.h"
#include"State/States/ChestMonsterDieState.h"
#include"State/States/ChestMonsterDamageState.h"

void ChestMonster::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Enemy/ChestMonster/ChestMonster.gltf", "ChestMonster",
			Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, "ChestMonster");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータクラス初期化
		m_parameter.Init();

		m_hp = m_parameter.GetParam().m_maxHP;


		// ノーマルステートで初期化
		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<ChestMonsterNormalState>();

		m_spawnCountDown = m_spawnInterval;
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(2.0f);
	m_bumpPushRate = 0.0f;
}

void ChestMonster::Update()
{

	m_stateMachine.Update();

	if (!IsSpawnEnemy())
	{
		m_spawnCountDown -= 60 * m_deltaTime;

		if (m_spawnCountDown <= 0)
		{
			SetIsSpawnEnemy(true);
		}
	}

}

void ChestMonster::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void ChestMonster::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 3.0f, 0.0f));
}

void ChestMonster::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void ChestMonster::UpdateSpawnEnemy()
{


	if (m_animFrameCount <= m_spawnTiming.spawnStart || m_animFrameCount >= m_spawnTiming.spawnEnd)
	{
		return;
	}

	m_spawnWait -= 60 * m_deltaTime;

	if (m_spawnWait <= 0)
	{
		m_spawnFlg = true;
		m_spawnWait = 0;
	}

	if (!m_spawnFlg)
	{
		return;
	}


	m_LayerNum = KdRandom::GetInt(0, enemyLayerCount - 1);

	switch (static_cast<Enemes>(m_LayerNum))
	{
	case Enemes::Cactas:
		CreateEnemy("Slime");
		break;
	case Enemes::Mushroom:
		CreateEnemy("Mushroom");
		break;
	case Enemes::TurtleShell:
		CreateEnemy("TurtleShell");
		break;
	}

	m_spawnWait = 8.0f;

	m_spawnFlg = false;

}

void ChestMonster::StartSpawnEnemy()
{
	m_spawnFlg = true;

	SetSpawnTiming();
}

void ChestMonster::EndSpawnEnemy()
{
	m_spawnCountDown = m_spawnInterval;
	SetIsSpawnEnemy(false);

}

void ChestMonster::AnimFrame()
{
	m_animFrameCount += m_deltaTime * 60.0f;
}

void ChestMonster::PlayAnimation(ChestMonsterAnimationType type)
{
	m_animation.Play(type);
}

void ChestMonster::RePlayAnimation(ChestMonsterAnimationType type)
{
	m_animation.RePlay(type);
}

void ChestMonster::OnHit(const AttackInfo attackInfo)
{

	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
		m_stateMachine.ChangeState<ChestMonsterDieState>();
	}
	else
	{
		if(!IsSpawnEnemy())
		{
			m_stateMachine.ChangeState<ChestMonsterDamageState>();
			RePlayAnimation(ChestMonsterAnimationType::GetHit);
		}
	}

	TimeManager::Instance().StartHitStop(0.1);

	PlayHitEffect();

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos(), m_flyTextPath);

	AddKnockBack(attackInfo.knockBackDir, attackInfo.knockBackPower);
}


void ChestMonster::CreateEnemy(const std::string& enemyName)
{
	auto obj = KdGameObjectFactory::Instance().CreateGameObject(enemyName);

	auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

	enemy->Init();
	enemy->SetPos(GetPos()+Math::Vector3(0,2,0));
	enemy->SetPrevPos(GetPos() + Math::Vector3(0, 2, 0));
	enemy->SetRotation(GetRotation());


	float power = 0.2;

	Math::Vector3 launchDir = m_mWorld.Backward();

	launchDir *= 0.04;
	// 飛び出す方向をセット
	enemy->Launch(launchDir, power);

	// 関連付け
	enemy->SetUpReference();

	SceneManager::Instance().AddObject(enemy);
}

void ChestMonster::UpdateAnimation()
{

	m_animation.Update(m_deltaTime);

}

void ChestMonster::SetSpawnTiming()
{
	m_spawnTiming.spawnStart=40;
	m_spawnTiming.spawnEnd=60;

	m_animFrameCount = 0.0f;
}
