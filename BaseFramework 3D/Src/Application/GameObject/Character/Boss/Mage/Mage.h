#pragma once

#include"../BossBase.h"

#include"Animation/MageAnimation.h"
#include"Parameter/MageParameter.h"
#include"../../StateMachine/StateMachine.h"


class Mage :public BossBase
{
public:

	Mage() {}
	~Mage()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void OnHit(const AttackInfo attackInfo)override;

	void PlayAnimation(MageAnimationType type);
	void RePlayAnimation(MageAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	bool IsAttack()const { return m_attackFlg; }

	// 攻撃パターンを抽選する(HP50%以下限定パターン・分身自身は除外して選ぶ)
	MageAttackPattern SelectAttackPattern();

	// 攻撃終了時にクールダウンを再セットする
	void EndAttack();

	// 分身かどうかの設定/取得
	void SetIsClone(bool flg) { m_isClone = flg; }
	bool IsClone()const { return m_isClone; }

	//================================
	// 各攻撃パターンの実行(各AttackStateから呼ばれる)
	//================================

	// 1. 敵を召喚
	void SummonEnemy();
	// 2. プレイヤー位置に魔法円
	void CastMagicCircle();
	// 3. 前方範囲魔法
	void ForwardAreaAttack();
	// 4. プレイヤーへ単発魔法弾
	void FireBolt();
	// 5. 全方位魔法
	void FireAllDirection();
	// 6. 分身
	void SpawnClones();

private:

	void UpdateAnimation();

	// 攻撃のクールダウン更新(0になったら攻撃フラグを立てる)
	void UpdateAttack();

	void DrawParameterInspector() override;

private:

	// アニメーションクラス
	MageAnimation   m_animation;

	// パラメータクラス
	MageParameter   m_parameter;

	// ステートマシン
	StateMachine<Mage> m_stateMachine;

	// 攻撃のクールダウン
	float m_attackCooldown = 0.0f;
	float m_attackCooldownDuration = 2.0f;

	bool m_attackFlg = false;

	// 分身(本物の代わりに攻撃するクローン)かどうか
	bool m_isClone = false;

};
