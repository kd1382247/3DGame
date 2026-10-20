#pragma once

class Mage;
class MageMagicCircle;

#include"../../../../StateMachine/StateBase.h"
#include"../../ColorSphereIndicator/ColorSphereIndicator.h"
#include"../../Parameter/MageParameter.h"

// 5. ボス中心から半径数メートルの範囲攻撃(HP50%以下限定)
class MageNovaCircleState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	// 敵が詠唱する
	void UpdateCast(Mage* mage);

	// 攻撃開始
	void StartActive(Mage* mage);
	void UpdateActive(Mage* mage);

	enum class Phase
	{
		Cast,
		Active
	};

	float m_castTimer = 0.0f;

	// 生成した魔法円への参照(これが無くなる/終了したらNormalStateへ戻る)
	std::weak_ptr<MageMagicCircle> m_wpMagicCircle;

	Phase m_phase = Phase::Cast;

	MageParameter::NovaCircle m_param;
	Math::Vector3             m_centerPos = {};

	ColorSphereIndicator m_rangeIndicator;
	ColorSphereIndicator m_telegraphIndicator;

};
