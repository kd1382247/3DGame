#pragma once

template<typename OwnerType>
class StateMachine;

// 全ての状態の基底となるクラス
template<typename OwnerType>
class StateBase
{
protected:
	friend class StateMachine<OwnerType>;

	virtual void OnStart(OwnerType* owner) {}
	virtual void OnUpdate(OwnerType* owner) {}
	virtual void OnExit(OwnerType* owner) {}

private:

	void SetMachine(StateMachine<OwnerType>* machine)
	{
		m_pMachine = machine;
	}

	void CallStart(OwnerType* owner)
	{
		if (m_pMachine == nullptr || owner == nullptr) { return; }
		OnStart(owner);
	}

	void CallUpdate(OwnerType* owner)
	{
		if (m_pMachine == nullptr || owner == nullptr) { return; }
		OnUpdate(owner);
	}

	void CallExit(OwnerType* owner)
	{
		if (m_pMachine == nullptr || owner == nullptr) { return; }
		OnExit(owner);
	}

protected:

	StateMachine<OwnerType>* m_pMachine = nullptr;
	OwnerType* m_pOwner = nullptr;
};