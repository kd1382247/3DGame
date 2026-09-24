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

	// 攻撃パターンを抽選する(NovaCircleはHPが50%以下の時のみ選択対象に含まれる)
	MageAttackPattern SelectAttackPattern();

	// 攻撃終了時にクールダウンを再セットする
	void EndAttack();

	//================================
	// 各攻撃パターンの実行(各AttackStateから呼ばれる)
	//================================

	// 1. 敵を召喚(5体、ボスを囲むように配置)
	// 1-1. 出現座標を計算し、煙エフェクトを再生する
	void PrepareSummonPositions();
	// 1-2. 用意しておいた座標に敵を生成する
	void SpawnEnemies();
	// 2. プレイヤー位置に魔法攻撃(魔法円)
	void CastTargetCircle();
	// 3. 前方に扇形の範囲攻撃
	void CastForwardSector();
	// 4. プレイヤーへ単発魔法弾
	void FireBolt();
	// 5. ボス中心から半径数メートルの範囲攻撃(HP50%以下限定)
	void CastNovaCircle();

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

	// 召喚攻撃で使う、敵の出現予定座標(煙エフェクト再生時に計算し、敵生成時に消費する)
	std::vector<Math::Vector3> m_summonPositions;

};
