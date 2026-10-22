#pragma once

#include"../StageGimmick.h"
#include"../../Character/Enemy/SpawnableEnemies.h"
#include"EnemySpawnerRequest.h"

class Room;
class EnemyBase;
class CharacterBase;

class CameraBase;

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

	void SetUpReference()override;

	void DrawDebug()override;

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

	// 当たり判定の球(クリック選択・デバッグ表示)の半径
	// キャラを押し戻す当たり判定(縦向きの円柱)の半径にも使う
	static constexpr float kColliderRadius  = 1.0f;

	float GetColliderRadius() const { return kColliderRadius; }

	// 当たり判定の球の中心(ワールド座標)
	Math::Vector3 GetColliderCenter() const
	{
		return GetPos();
	}

	// このキャラを押し戻すか(CollisionManagerが見る)
	// このスポナーから出てきたばかりの敵は、外に出るまで押し戻さない(中から出られなくなるため)
	bool IsBlockingCharacter(const CharacterBase* character) const;

	// 生成の要求を受け取る
	void RequestSpawn(const EnemySpawnerRequest& request);

	// 生成の要求を受けれるかどうか
	bool CanAcceptRequest();

	// 生成した敵を、このスポナーの生存リストに登録する
	// (スポナーが出した敵のほか、その敵から生まれた敵(スライムの分裂など)も登録する)
	void RegisterSpawnedEnemy(const std::shared_ptr<EnemyBase>& enemy);

private:

	void UpdateSinCurve(Math::Matrix& worldMat);

	// 生成前の回転を始める
	void StartSpin();

	// 回転を進める(最初は速く、lerpで少しずつゆっくり止まる)
	void UpdateSpin();

	void SpawnEnemy();

	void EraseEnemy();

	// スポナーから出終わった敵を、「出てきたばかり」のリストから外す
	void UpdateEmergingEnemies();

	void UpdateRequest(const std::shared_ptr<Room>& room);

	std::string PickEnemyName(const std::set<std::string>& names)const;

	void PlayIntroEffect();

	// 飛び出す方向を決める
	Math::Vector3 CreateLaunchDir()const;
	// 敵インスタンス生成
	void CreateEnemy(const std::string& className);

	// 名前から部屋を探す
	std::shared_ptr<Room> FindRoomByName(const std::string& name)const;

	// 紐づけた部屋を返す(無ければnullptr)
	std::shared_ptr<Room> GetLinkedRoom();

private:

	void UpdateCameraFade();

	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	std::vector<Wave>m_waves;

	std::weak_ptr<CameraBase>m_wpCamera;

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

	// 出てきたばかりで、まだスポナーの当たり判定から出ていない敵
	std::vector<std::weak_ptr<EnemyBase>>m_emergingEnemies;

	std::weak_ptr<KdEffekseerObject>m_wpEffekseerObj;

	// 生成を知らせるエフェクト
	bool m_spawnIntroEffect = false;

	bool m_isSpawnIntor = true;


	// 要求で出す敵の種類
	std::set<std::string>m_summonEnemyNames;

	// リクエストの残り数
	int m_requestRemaining = 0;

	float m_requestInterval = 0.2f;
	float m_requestTimer = 0.0f;


	// 要求で出した敵が、同時に生きていられる数の上限
	int m_maxAlive = 10;

	// サインカーブの頻度
	const float m_sinCurveFrequency = 0.4f;
	float       m_sinCurveTime = 0.0f;

	// 生成前の回転(見た目だけ。GetPos()や当たり判定は変えない)
	static constexpr float kSpinDuration = 2.0f; // 回り始めてから止まるまでの時間(秒)
	static constexpr float kSpinTurns    = 3.0f; // 止まるまでに回る回数

	bool  m_isSpinning = false;
	float m_spinTimer  = 0.0f;
	float m_spinAngle  = 0.0f; // 今の回転角(ラジアン)

	// ウェーブ側:今回の生成の前に、回転を始めたか
	bool  m_spinTriggered = false;


	float m_fadeRate = 0.0f;
};

