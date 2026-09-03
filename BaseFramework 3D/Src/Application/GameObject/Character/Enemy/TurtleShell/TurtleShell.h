#pragma once

#include"../EnemyBase.h"

#include"Animation/TurtleShellAnimationType.h"
#include"Animation/TurtleShellAnimation.h"
#include"Parameter/TurtleShellParameter.h"
#include"State/TurtleShellStateMachine.h"

class TurtleShell :public EnemyBase
{
public:

	TurtleShell() {}
	~TurtleShell()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void DrawDebug()override;

	void SetUpReference()override;


	template<class T>
	void ChangeState()
	{
		m_stateMachine.ChangeState(*this, std::make_unique<T>());
	}

	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }

	// パラメータのゲッター
	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }


	bool IsAnimationFinished()const { return m_animation.IsFinished(); }


	void PlayAnimation(TurtleShellAnimationType type);
	void RePlayAnimation(TurtleShellAnimationType type);

	// 攻撃

	void UpdateSpinAttackMove();
	bool SpinAttackRemaining();
	void StartSpinAttack();
	void EndSpinAttack();

	// スタン
	bool DizyyRemaining();
	void StartDizzy();
	void EndDizzy();


	void UpdateLaunch();

	void UpdateMove();

	// 攻撃判定
	void UpdateAttackCollision();

	void OnHit(const AttackInfo attackInfo)override;

private:


	void UpdateAttack();

	void HitCoolDownRemaining();

	void UpdateAnimation();


private:


	float m_spinAttackDuration = 0;
	float m_spinAttackRemaining = 0;

	float m_dizzyDuration = 0;
	float m_dizzyRemaining = 0;

	Math::Vector3          m_targetPos = {};

	// アニメーションクラス
	TurtleShellAnimation   m_animation;

	// パラメータクラス
	TurtleShellParameter   m_parameter;

	// ステートマシン
	TurtleShellStateMachine m_stateMachine;


	float m_hitCooldownDuration = 0.0f;
	float m_hitCooldownRemaining = 0.0f;

};