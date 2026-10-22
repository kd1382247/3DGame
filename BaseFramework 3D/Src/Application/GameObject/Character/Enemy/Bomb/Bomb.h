#pragma once

#include"../EnemyBase.h"

#include"Animation/BombAnimation.h"
#include"State/BombState.h"
#include"Parameter/BombParameter.h"

#include"../../StateMachine/StateMachine.h"

// Mageの魔法円と同じ、攻撃範囲・予告の表示に使うカラースフィア
#include"../../Boss/Mage/ColorSphereIndicator/ColorSphereIndicator.h"


class Bomb :public EnemyBase
{
public:

	Bomb() {}
	~Bomb()override { HideExplosionRange(); }

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void SetUpReference()override;

	bool IsAttack()const { return m_attackFlg; }
	bool IsLaunch()const { return m_launchFlg; }


	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }
	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	void PlayAnimation(BombAnimationType type);
	void RePlayAnimation(BombAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	void OnHit(const AttackInfo attackInfo)override;


	// ためモーション(AttackCharge)の開始・更新・終了判定
	// (プレイヤーに到達してもすぐ爆発せず、少し猶予を持たせるための処理)
	void StartCharge();
	void UpdateCharge();
	bool IsChargeFinished()const { return m_chargeTimer <= 0.0f; }

	// 爆発する(専用のExplosionオブジェクトを生成して当たり判定を任せ、自分は消える)
	void Explode();

	// 爆発範囲・予告の表示(ステージの色を変えるエフェクト)の表示/非表示
	// 当たり判定と同じ座標・半径を使うことで、見た目と実際の範囲を一致させる
	// ・範囲の円 : ためている間と爆発のモーションが終わるまで、爆発の半径のまま表示する
	// ・予告の円 : ための進み具合に合わせて、範囲いっぱいまで広がっていく(ため終わったら消える)
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

	// 爆発の半径(Parameterの値をInitでセットする)
	float m_explosionRadius = 0.0f;

	// ためモーションの残り時間(秒)
	float m_chargeTimer = 0.0f;

	// ためモーションの長さ(秒)(Parameterの値をInitでセットする)
	float m_chargeDuration = 0.0f;

	// 爆発範囲用のカラースフィア(ため始めてから、爆発のモーションが終わるまで表示する)
	ColorSphereIndicator m_rangeIndicator;

	// 爆発予告用のカラースフィア(ためている間だけ、範囲いっぱいまで広がっていく)
	ColorSphereIndicator m_telegraphIndicator;
};
