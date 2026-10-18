#pragma once

#include"../../../../StateMachine/StateBase.h"

class Mage;

class MageDieState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	// 死亡処理を終えたか(Destroy後に、もう一度処理しないため)
	bool m_isFinished = false;

};
