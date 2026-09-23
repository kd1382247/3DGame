#pragma once

#include"../EnemyBase.h"

#include"Animation/BombAnimation.h"
#include"State/BombState.h"
#include"Parameter/BombParameter.h"

#include"../../StateMachine/StateMachine.h"


class Bomb :public EnemyBase
{
public:

	Bomb() {}
	~Bomb()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void SetUpReference()override;

	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }

	int GetMaxHP()const override { return m_parameter.GetParam().m_maxHP; }

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(BombAnimationType type);
	void RePlayAnimation(BombAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void UpdateLaunch();

	void OnHit(const AttackInfo attackInfo)override;

	// ためモーション(AttackCharge)の開始・更新・終了判定
	// (プレイヤーに到達してもすぐ爆発せず、少し猶予を持たせるための処理)
	void StartCharge();
	void UpdateCharge();
	bool IsChargeFinished()const { return m_chargeTimer <= 0.0f; }

	// 爆発する(専用のExplosionオブジェクトを生成して当たり判定を任せ、自分は消える)
	void Explode();

	// 爆発範囲の表示(ステージの色を変えるエフェクト)の表示/非表示
	// 当たり判定と同じ座標・半径を使うことで、見た目と実際の範囲を一致させる
	void CreateExplosionRange();
	void UpdateExplosionRange();
	void HideExplosionRange();

private:

	void UpdateAnimation();

	void PlayWalkAnimation() override;
	void PlayIdleAnimation() override;

	void DrawParameterInspector() override;

private:

	// アニメーションクラス
	BombAnimation   m_animation;

	// パラメータクラス
	BombParameter   m_parameter;

	// ステートマシン
	StateMachine<Bomb> m_stateMachine;

	// 爆発の半径
	float m_explosionRadius = 3.0f;

	// ためモーションの残り時間(秒)
	float m_chargeTimer = 0.0f;

	// ためモーションの長さ(秒)
	const float m_chargeDuration = 1.5f;

	// カラースフィアのスロット番号保存用
	int m_colorSphereHandle = -1;
};
