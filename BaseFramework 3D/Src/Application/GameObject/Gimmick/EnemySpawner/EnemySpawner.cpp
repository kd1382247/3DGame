#include "Pch.h"
#include "EnemySpawner.h"

#include"../../../Scene/SceneManager.h"

#include"../../Character/Enemy/EnemyBase.h"

#include"../../../System/ReferenceManager/ReferenceManager.h"
#include"../../../../Framework/GameObject/KdGameObjectFactory.h"

#include"../../../System/CollisionManager/CollisionManager.h"

void EnemySpawner::Init()
{

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Gimmick/EnemySpawner.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("EnemySpawner",m_spModel,KdCollider::TypeEvent);

		CollisionManager::Instance().RegisterObject(CollisionLayer::Gimmick,shared_from_this());


		m_parameter.Init();
		m_spawnInterval=m_parameter.GetParameter().m_spawnInterval;
		m_spawnCnt = m_parameter.GetParameter().m_spawnCount;
	}

	SetObjectName("EnemySpawner");
	SetObjectCategory(KdGameObject::ObjectCategory::Gimmick);

	SetPos({ 0.0f,0.0f,0.0f });

}

void EnemySpawner::Update()
{
	m_spawnCountDown--;

	if (m_spawnCountDown <= 0)
	{
		SpawnEnemy();
		m_spawnCountDown=60.0f*m_spawnInterval;
	}
}

void EnemySpawner::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void EnemySpawner::DrawInspector()
{
	KdGameObject::DrawBasicInspecter();

	m_parameter.DrawInspector();
}

void EnemySpawner::SpawnEnemy()
{
	
	for (int i = 0; i < m_spawnCnt; i++)
	{
		m_LayerNum = KdRandom::GetInt(0, enemyLayerCount - 1);

		switch (static_cast<Enemes>( m_LayerNum))
		{
		case Enemes::Cactas:
			CreateEnemy("Cactas");
			break;
		case Enemes::Mushroom:
			CreateEnemy("Mushroom");
			break;
		case Enemes::Slime:
			CreateEnemy("Slime");
			break;
		case Enemes::TurtleShell:
			CreateEnemy("TurtleShell");
			break;
		}
	}
}

Math::Vector3 EnemySpawner::CreateLaunchDir() const
{
	
	float power = KdRandom::GetFloat(0.05f,0.08f);
	
	Math::Vector3 randomDir;

	randomDir.x = KdRandom::GetFloat(-1.0f,1.0f);
	randomDir.z = KdRandom::GetFloat(-1.0f,1.0f);
	randomDir.y = 0;

	randomDir.Normalize();

	return randomDir*power;

}

void EnemySpawner::CreateEnemy(const std::string&enemyName)
{

	auto obj = KdGameObjectFactory::Instance().CreateGameObject(enemyName);

	auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

	enemy->Init();
	enemy->SetPos(GetPos());

	float power = KdRandom::GetFloat(0.3f,0.5f);
	Math::Vector3 launchDir = CreateLaunchDir();
	// 飛び出す方向をセット
	enemy->Launch(launchDir,power);

	// 関連付け
	enemy->SetUpReference();

	SceneManager::Instance().AddObject(enemy);
}


