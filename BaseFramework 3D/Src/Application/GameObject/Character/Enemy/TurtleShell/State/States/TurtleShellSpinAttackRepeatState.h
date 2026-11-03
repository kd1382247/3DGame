#pragma once

class TurtleShell;

#include"../../../../StateMachine/StateBase.h"

class TurtleShellSpinAttackRepeatState :public StateBase<TurtleShell>
{
public:

	void OnStart(TurtleShell* turtleShell)override;
	void OnUpdate(TurtleShell* turtleShell)override;
	void OnExit(TurtleShell* turtleShell)override;


private:

	void UpdateEffect(TurtleShell* turtleShell);
	void PlayEffect(const Math::Vector3& pos);

	std::weak_ptr<KdEffekseerObject >m_wpEffect;

};

