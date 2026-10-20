#pragma once

#include"../BossBase.h"

#include"MageBeam/MageBeamDesc.h"
#include"Animation/MageAnimation.h"
#include"Parameter/MageParameter.h"
#include"../../StateMachine/StateMachine.h"

class MageBeam;
class MageMagicCircle;
class MageMagicSector;

class Mage :public BossBase
{
public:

	Mage() {}
	~Mage()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void SetUpReference()override;

	// 登場演出を始める
	void StartIntro()override { m_introRequested = true; }

	// HPバーを生成
	void CreateHPBar();

	float GetTurnSpeed()const override { return m_parameter.GetParam().m_turnSpeed; }

	float GetMoveSpeed()const override { return m_parameter.GetParam().m_moveSpeed; }

	// 各攻撃ステートが、詠唱時間・ビームの大きさなどを読むために使う
	const MageParameter::Parameter& GetParam()const { return m_parameter.GetParam(); }

	void OnHit(const AttackInfo attackInfo)override;

	void PlayAnimation(MageAnimationType type);
	void RePlayAnimation(MageAnimationType type);

	bool IsAnimationFinished()const { return m_animation.IsFinished(); }

	bool IsAttack()const { return m_attackFlg; }

	// 攻撃パターンを抽選する(NovaCircleはHPが50%以下の時のみ選択対象に含まれる)
	MageAttackPattern SelectAttackPattern();

	// 攻撃終了時にクールダウンを再セットする
	void EndAttack();

	// 出ている攻撃(魔法円・扇形・ビーム・弾)を全部消す(倒された時に呼ぶ)
	void CancelAllAttacks();

	//================================
	// 各攻撃パターンの実行(各AttackStateから呼ばれる)
	//================================

	// 1. 敵を召喚(敵の生成は、EnemySpawnerに合図を出して任せる)
	void RequestSummon();
	// 2. プレイヤー位置に魔法攻撃(魔法円)
	std::shared_ptr<MageMagicCircle> CastTargetCircle();
	// 3. 前方に扇形の範囲攻撃
	std::shared_ptr<MageMagicSector> CastForwardSector();
	// 4. プレイヤーへ単発魔法弾
	void FireBolt();
	// 5. ボス中心から半径数メートルの範囲攻撃(HP50%以下限定)
	std::shared_ptr<MageMagicCircle> CastNovaCircle();
	// 6. レーザーを放つ(生成したMageBeamを返す)
	std::shared_ptr<MageBeam> FireBeam(const MageParameter::Beam& param);

	// ビームの発射座標・方向を求める(予告表示とビーム生成で同じ値を使うために、共通の関数にしている)
	void GetBeamPose(const MageParameter::Beam& param, Math::Vector3& outPos, Math::Vector3& outDir)const;

	// エフェクト再生用の回転値を取得(エフェクト側の初期正面とのズレを補正済み)
	Math::Vector3 GetEffectRotation(const float value)const;

	// 攻撃用エフェクトの再生座標を取得(ボスの位置から、前方・上へのずらしを足した位置)
	Math::Vector3 GetAttackEffectPos(const MageParameter::EffectParam& effect)const;

	// 攻撃用エフェクトを、Parameterの設定どおりに再生する
	std::weak_ptr<KdEffekseerObject> PlayAttackEffect(const MageParameter::EffectParam& effect)const;

	
private:

	void UpdateAnimation();

	// 攻撃のクールダウン更新(0になったら攻撃フラグを立てる)
	void UpdateAttack();

	void DrawParameterInspector() override;

	void PlayWalkAnimation()override;
	void PlayIdleAnimation()override;

private:

	// アニメーションクラス
	MageAnimation   m_animation;

	// パラメータクラス
	MageParameter   m_parameter;

	// ステートマシン
	StateMachine<Mage> m_stateMachine;

	// 攻撃のクールダウン
	float m_attackCooldown = 0.0f;
	// クールダウンの長さ(Parameterの値をInitでセットする)
	float m_attackCooldownDuration = 0.0f;

	bool m_attackFlg = false;



};
