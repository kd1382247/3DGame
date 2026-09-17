#pragma once

#include"StateBase.h"

template<typename OwnerType>
class StateMachine
{
public:
	StateMachine()
		:m_fnChangeState([]() {})
	{}

	void Start(OwnerType* owner)
	{
		m_pOwner = owner;
		m_fnChangeState = []() {};
	}

	template<typename StateType, typename...ArgType>
	void ChangeState(ArgType...args)
	{
		m_fnChangeState = [=]()
			{
				if (m_pOwner == nullptr) { return; }

				if (m_spNowState != nullptr)
				{
					m_spNowState->CallExit(m_pOwner);
					m_spNowState = nullptr;
				}

				m_spNowState = std::make_shared<StateType>(args...);
				if (m_spNowState == nullptr) { return; }

				m_spNowState->SetMachine(this);
				m_spNowState->CallStart(m_pOwner);
			};
	}

	void Update()
	{
		m_fnChangeState();
		m_fnChangeState = []() {};

		if (m_spNowState != nullptr)
		{
			m_spNowState->CallUpdate(m_pOwner);
		}
	}

private:
	OwnerType* m_pOwner = nullptr;
	std::shared_ptr<StateBase<OwnerType>> m_spNowState = nullptr;
	std::function<void()> m_fnChangeState;
};