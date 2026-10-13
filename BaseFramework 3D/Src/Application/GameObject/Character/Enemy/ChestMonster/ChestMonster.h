#pragma once

#include"../EnemyBase.h"

#include"Animation/ChestMonsterAnimationType.h"
#include"Animation/ChestMonsterAnimation.h"
#include"Parameter/ChestMonsterParameter.h"
#include"../../StateMachine/StateMachine.h"
#include"../../../Gimmick/Room/RoomLink.h"


class ChestMonster :public EnemyBase
{
public:

	ChestMonster() {}
	~ChestMonster()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspectorHeader()override;

	void SetUpReference()override;

	bool IsSpawnEnemy() const { return m_isSpawnEnemy; }

	void SetIsSpawnEnemy(const bool flg) { m_isSpawnEnemy = flg; }

	// 敵を召喚してよい数か(自分が出した敵の数と、全体の敵の数の、両方の上限を調べる)
	bool IsUnderEnemyLimit();

	void UpdateSpawnEnemy();

	void StartSpawnEnemy();
	void EndSpawnEnemy();


	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void AnimFrame();

	void PlayAnimation(ChestMonsterAnimationType type);
	void RePlayAnimation(ChestMonsterAnimationType type);


	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void OnHit(const AttackInfo attackInfo) override;

	// 1体ごとの個別パラメータをステージデータに保存/読み込みする
	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json)override;


private:

	struct SpawnTiming
	{
		float spawnStart=0.0f;
		float spawnEnd = 0.0f;
	};

	void CreateEnemy(const std::string&enemyName);

	void UpdateAnimation();

	void DrawParameterInspector() override;

	void SetSpawnTiming();

	// Parameterの値を、このクラスのメンバーへ反映する(Init後・LoadData後に呼ぶ)
	void ApplyParameter();

	// 紐づけた部屋が戦闘中か(部屋に紐づけていないときは、常にtrue)
	bool IsInActiveRoom();

	// 倒された敵を、自分が出した敵のリストから消す
	void EraseSpawnedEnemy();

	// 全体の生きている敵の数(ChestMonster本体は数えない)
	int CountTotalEnemies()const;

	float m_animFrameCount = 0.0f;


	// ステートマシン
	StateMachine<ChestMonster> m_stateMachine;

	// アニメーションクラス
	ChestMonsterAnimation   m_animation;

	// パラメータクラス
	ChestMonsterParameter   m_parameter;

	bool  m_isSpawnEnemy = false;

	// スポーンの間隔(秒)。間隔はParameterの値をInitでセットする
	float m_spawnCountDown = 0;
	float m_spawnInterval = 0;

	float m_spawnWait = 0.0f;

	bool m_spawnFlg = false;

	SpawnTiming m_spawnTiming;

	// 対応する部屋(戦闘中だけ召喚する)
	RoomLink m_roomLink;

	// 自分が出した敵のリスト(同時に生きている数を数える)
	std::vector<std::weak_ptr<EnemyBase>> m_spawnedEnemies;

};