#pragma once

class Mage;

#include"../../../../StateMachine/StateBase.h"

// 3. 敵(ボス)前方への扇形範囲攻撃
class MageForwardSectorState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	float m_castTimer = 0.0f;
	bool  m_hasCast = false;

};
