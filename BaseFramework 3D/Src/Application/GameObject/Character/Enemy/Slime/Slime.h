#pragma once

#include"../EnemyBase.h"

#include"Animation/SlimeAnimationType.h"
#include"Animation/SlimeAnimation.h"
#include"Parameter/SlimeParameter.h"
#include"../../StateMachine/StateMachine.h"


class Slime :public EnemyBase
{
public:

	enum class SlimeSize
	{
		Large,
		Small
	};

	Slime() {}
	~Slime()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;
	void SetUpReference()override;

	void DrawDebug()override;


	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }

	// パラメータのゲッター
	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }


	bool IsAnimationFinished()const { return m_animation.IsFinished(); }


	void PlayAnimation(SlimeAnimationType type);
	void RePlayAnimation(SlimeAnimationType type);

	void StartAttack();
	void EndAttack();

	void SetSlimeSize(const SlimeSize size) { m_slimeSize = size; };
	SlimeSize GetSlimeSize()const { return m_slimeSize; }

	void UpdateLaunch();

	// スライムを分裂
	void Split();
	// 攻撃判定
	void UpdateAttackCollision();

	void OnHit(const AttackInfo attackInfo)override;

private:

	void UpdateAnimation();

	void PlayWalkAnimation() override;
	void PlayIdleAnimation() override;

	void DrawParameterInspector() override;

	void SetAttackTiming();

private:

	SlimeSize        m_slimeSize = SlimeSize::Large;

	// アニメーションクラス
	SlimeAnimation   m_animation;

	// パラメータクラス
	SlimeParameter   m_parameter;

	// ステートマシン
	StateMachine<Slime> m_stateMachine;


	static const int spawnNum = 4;

	Math::Vector3    m_launchDir[spawnNum] =
	{
		{Math::Vector3::Backward},
		{Math::Vector3::Forward},
		{Math::Vector3::Right},
		{Math::Vector3::Left}
	};

};