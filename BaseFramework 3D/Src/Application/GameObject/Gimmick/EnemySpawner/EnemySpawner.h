#pragma once

#include"../EnemySpawner/Parameter/EnemySpawnerParameter.h"

class EnemySpawner:public KdGameObject
{
public:

	enum class Enemes
	{
		Cactas,
		Mushroom,
		Slime,
		TurtleShell,
		Max
	};

	EnemySpawner(){}
	~EnemySpawner(){}

	void Init()override;
	void Update()override;
	void DrawLit()override;

	void DrawInspector()override;


private:

	void SpawnEnemy();

	// 飛び出す方向を決める
	Math::Vector3 CreateLaunchDir()const;
	// 敵インスタンス生成
	void CreateEnemy(const std::string& className);


private:

	// パラメータクラス
	EnemySpawnerParameter m_parameter;


	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	// スポーンする数
	int       m_spawnCnt = 0;

	// スポーンの間隔
	float m_spawnCountDown = 0;
	float m_spawnInterval = 0;

	float m_maxHP = 0;
	float m_hp = 0;

	size_t      m_LayerNum = {};


	// 敵の種類数
	static constexpr size_t enemyLayerCount =
		static_cast<size_t>(Enemes::Max);


};

