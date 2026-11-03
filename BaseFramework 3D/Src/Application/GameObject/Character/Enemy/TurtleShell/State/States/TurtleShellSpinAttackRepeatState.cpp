#include "TurtleShellSpinAttackRepeatState.h"

#include"../../TurtleShell.h"

#include"TurtleShellDizzyState.h"

void TurtleShellSpinAttackRepeatState::OnStart(TurtleShell* turtleShell)
{
	turtleShell->PlayAnimation(TurtleShellAnimationType::SpinAttackRPT);
}

void TurtleShellSpinAttackRepeatState::OnUpdate(TurtleShell * turtleShell)
{

	UpdateEffect(turtleShell);

	turtleShell->UpdateSpinAttackMove();

	// 当たり判定
	turtleShell->UpdateAttackCollision();


	if (turtleShell->SpinAttackRemaining())
	{

		if (auto spEffect=m_wpEffect.lock())
		{
			spEffect->StopEffect();
		}
		
		m_pMachine->ChangeState<TurtleShellDizzyState>();
		return;
	}
}

void TurtleShellSpinAttackRepeatState::OnExit(TurtleShell * turtleShell)
{
	turtleShell->EndSpinAttack();


	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

}

void TurtleShellSpinAttackRepeatState::UpdateEffect(TurtleShell* turtleShell)
{
	// (区間の再生が終わっている場合は、こちらで再生し直してループさせる)
	auto spEffect = m_wpEffect.lock();

	if (!spEffect || !spEffect->IsPlaying())
	{
		Math::Vector3 effectPos = turtleShell->GetPos();
		PlayEffect(effectPos);
		spEffect = m_wpEffect.lock();
	}

	if (spEffect)
	{
		spEffect->SetPos(turtleShell->GetPos());
	}
}

void TurtleShellSpinAttackRepeatState::PlayEffect(const Math::Vector3& pos)
{

	m_wpEffect = KdEffekseerManager::GetInstance().Play("Enemy/Spin/Spin.efkefc", pos, 0.5f, 1.0f, false, 50.0f, 120.0f);

}
