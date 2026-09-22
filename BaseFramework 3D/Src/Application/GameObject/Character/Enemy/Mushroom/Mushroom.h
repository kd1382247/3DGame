#pragma once

#include"../EnemyBase.h"

#include"Animation/MushroomAnimationType.h"
#include"Animation/MushroomAnimation.h"
#include"Parameter/MushroomParameter.h"
#include"../../StateMachine/StateMachine.h"


class Mushroom :public EnemyBase
{
public:

	// パラメータをどのタイプで引くか(定義はMushroomParameter側)
	using MushroomType = MushroomParameter::MushroomType;

	Mushroom() {}
	~Mushroom()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawDebug()override;

	void SetUpReference()override;

	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }


	// パラメータのゲッター
	int GetMaxHP()const override { return m_parameter.GetParam(m_mushroomType).m_maxHP; }
	float GetTurnSpeed()const override { return m_parameter.GetParam(m_mushroomType).m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam(m_mushroomType).m_moveSpeed; }

	void PlayAnimation(MushroomAnimationType type);
	void RePlayAnimation(MushroomAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }


	void StartAttack();
	void EndAttack();

	void UpdateLaunch();

	// 抽選で決まったタイプ(Smile/Angry)を取得
	MushroomType GetMushroomType()const { return m_mushroomType; }

	void OnHit(const AttackInfo attackInfo)override;

	// 攻撃判定
	void UpdateAttackCollision();

private:

	void UpdateAnimation();

	void PlayWalkAnimation() override;
	void PlayIdleAnimation() override;

	void DrawParameterInspector() override;

	void SetAttackTiming();

	// 一定確率でAngryタイプを抽選する
	void LotteryMushroomType();

private:

	// 抽選で決まったMushroomのタイプ
	MushroomType m_mushroomType = MushroomType::Smile;

	// アニメーションクラス
	MushroomAnimation   m_animation;

	// パラメータクラス
	MushroomParameter   m_parameter;

	// ステートマシン
	StateMachine<Mushroom> m_stateMachine;

};
