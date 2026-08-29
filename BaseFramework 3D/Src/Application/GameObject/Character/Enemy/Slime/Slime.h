#pragma once

#include"../EnemyBase.h"

#include"Animation/SlimeAnimation.h"
#include"State/SlimeState.h"
#include"Parameter/SlimeParameter.h"

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

	void DrawInspector()override;

	void DrawDebug()override;

	void SetSlimeSize(const SlimeSize size) { m_slimeSize = size; };
	SlimeSize GetSlimeSize()const { return m_slimeSize; }

private:

	// スライムを分裂
	void Split();

	void UpdateLaunch();

	void UpdateMove();
	void UpdateAttack();

	void UpdateActionState();

	void UpdateAnimation();

	void ChangeActionState(SlimeActionState  nextState);
	void ExitState(SlimeActionState _state);
	void EnterState(SlimeActionState _state);



	void SetAttackTiming();

	// 攻撃判定
	void UpdateAttackCollision();

	
private:

	SlimeActionState m_actionState = SlimeActionState::Normal;
	SlimeMoveState   m_moveState = SlimeMoveState::Idle;

	SlimeSize        m_slimeSize = SlimeSize::Large;

	// アニメーションクラス
	SlimeAnimation   m_animation;

	// パラメータクラス
	SlimeParameter   m_parameter;


	static const int spawnNum = 4;

	Math::Vector3    m_launchDir[spawnNum] =
	{
		{Math::Vector3::Backward},
		{Math::Vector3::Forward},
		{Math::Vector3::Right},
		{Math::Vector3::Left}
	};

};