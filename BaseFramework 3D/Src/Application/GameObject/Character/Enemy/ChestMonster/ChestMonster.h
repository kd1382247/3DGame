#pragma once

#include"../EnemyBase.h"

#include"Animation/ChestMonsterAnimationType.h"
#include"Animation/ChestMonsterAnimation.h"
#include"Parameter/ChestMonsterParameter.h"
#include"../../StateMachine/StateMachine.h"


class ChestMonster :public EnemyBase
{
public:

	ChestMonster() {}
	~ChestMonster()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;


	void SetUpReference()override;

	bool IsSpawnEnemy() const { return m_isSpawnEnemy; }

	void SetIsSpawnEnemy(const bool flg) { m_isSpawnEnemy = flg; }

	void UpdateSpawnEnemy();

	void StartSpawnEnemy();
	void EndSpawnEnemy();

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void AnimFrame();

	void PlayAnimation(ChestMonsterAnimationType type);
	void RePlayAnimation(ChestMonsterAnimationType type);


	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void OnHit(const AttackInfo attackInfo) override;

private:

	enum class Enemes
	{
		Cactas,
		Mushroom,
		TurtleShell,
		Max
	};

	struct SpawnTiming
	{
		float spawnStart=0.0f;
		float spawnEnd = 0.0f;
	};

	void CreateEnemy(const std::string&enemyName);

	void UpdateAnimation();

	void DrawParameterInspector() override;

	void SetSpawnTiming();

	float m_animFrameCount = 0.0f;


	// ステートマシン
	StateMachine<ChestMonster> m_stateMachine;

	// アニメーションクラス
	ChestMonsterAnimation   m_animation;

	// パラメータクラス
	ChestMonsterParameter   m_parameter;

	Enemes m_enemes = {};

	bool  m_isSpawnEnemy = false;

	// スポーンの間隔
	float m_spawnCountDown = 0;
	float m_spawnInterval = 60*2;

	float m_spawnWait = 0.0f;

	bool m_spawnFlg = false;

	SpawnTiming m_spawnTiming;

	size_t      m_LayerNum = {};

	// 敵の種類数
	static constexpr size_t enemyLayerCount =
		static_cast<size_t>(Enemes::Max);

};