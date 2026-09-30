#pragma once

#include"../Parameter/PlayerParameter.h"
#include"../PlayerActionTiming.h"

// 必殺技(突進)の状態を持つクラス
// Playerのことは知らない。カメラの向きなど必要な情報は引数で受け取る
class PlayerSpecialMove
{
public:

	// パラメータの参照を受け取る(使う側が持つ)
	void Init(const PlayerParameter::SpecialMoveParam& param) { m_pParam = &param; }

	// 突進する方向をセットする(必殺技の開始時に、カメラの前方向などを渡す)
	void SetMoveDir(const Math::Vector3& dir) { m_specialMoveDir = dir; }

	// 突進の1フレームぶんの移動量
	Math::Vector3 CalcMoveVector(const float deltaTime) const;

	// 攻撃判定・トレイルのフレーム区間
	const PlayerActionTiming& GetTiming() const;

	float GetAttackPower() const { return m_pParam->m_attackPower; }

	// 多段ヒットの間隔(フレーム数)
	float GetHitCooldownDuration() const { return m_pParam->m_hitCooldownDuration; }

private:

	// パラメータ(Playerが持つPlayerParameterの中身を参照する)
	const PlayerParameter::SpecialMoveParam* m_pParam = nullptr;

	Math::Vector3   m_specialMoveDir = {};

};
