#pragma once

#include"../EnemyBase.h"

#include"Animation/ChestMonsterAnimationType.h"
#include"Animation/ChestMonsterAnimation.h"
#include"Parameter/ChestMonsterParameter.h"


class ChestMonster :public EnemyBase
{
public:

	ChestMonster() {}
	~ChestMonster()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	template<class T>
	void ChangeState()
	{
		m_stateMachine.ChangeState(*this, std::make_unique<T>());
	}


	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }


	void PlayAnimation(ChestMonsterAnimationType type);
	void RePlayAnimation(ChestMonsterAnimationType type);


	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void OnHit(const AttackInfo attackInfo) override;

private:


	void UpdateAnimation();



private:



	// アニメーションクラス
	ChestMonsterAnimation   m_animation;

	// パラメータクラス
	ChestMonsterParameter   m_parameter;

};