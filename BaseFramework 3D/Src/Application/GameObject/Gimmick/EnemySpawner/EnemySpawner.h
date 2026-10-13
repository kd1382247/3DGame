#pragma once

#include"../StageGimmick.h"
#include"../../Character/Enemy/SpawnableEnemies.h"

class Room;
class EnemyBase;

class EnemySpawner :public StageGimmick
{
public:
	
	struct Wave
	{
		std::set<std::string> m_enemyNames = {};
		int   m_spawnCount=0;
		float m_spawnInterval = 0.0f;
		float m_startDelay=0.0f;
	};

	EnemySpawner(){}
	~EnemySpawner(){}

	void Init()override;
	void Update()override;
	void DrawLit()override;

	// 固定表示(スクロールしない):名前・位置・回転・大きさ
	void DrawInspectorHeader()override;

	// スクロール側:スポナーのパラメータ
	void DrawInspector()override;

	// 最大ウェーブ数まで到達したら生成終了
	bool IsFinished()const;
	// 敵が全滅しているか
	bool IsAllEnemiesDead()const { return m_spawnEnemyList.empty(); }

	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json)override;

	std::string GetLinkRoomName()const { return m_linkedRoomName; }

private:

	void SpawnEnemy();

	void EraseEnemy();

	// 飛び出す方向を決める
	Math::Vector3 CreateLaunchDir()const;
	// 敵インスタンス生成
	void CreateEnemy(const std::string& className);

	// 名前から部屋を探す
	std::shared_ptr<Room> FindRoomByName(const std::string& name)const;

	// 紐づけた部屋を返す(無ければnullptr)
	std::shared_ptr<Room> GetLinkedRoom();

private:


	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	std::vector<Wave>m_waves;


	// 部屋の情報
	std::weak_ptr<Room>m_wpRoom;
	// スポナーがリンクしている部屋
	std::string m_linkedRoomName = {};
	bool m_warnedMissingRoom = false;

	// 何ウェーブか
	int m_currentWave = 0;
	float m_waveTimer = 0.0f;

	float m_waitTime = 0.0f;

	// 今のウェーブで生成した数
	int  m_spawnedInWave = 0;

	// スポーンした敵の配列
	// 全滅したかどうか
	std::vector<std::weak_ptr<EnemyBase>>m_spawnEnemyList;

};

