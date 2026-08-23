#pragma once

class CombatCollsionManager
{
public:


	



private:





private:

	CombatCollsionManager(){}
	~CombatCollsionManager(){}

public:

	static CombatCollsionManager& Instance()
	{
		static CombatCollsionManager instance;
		return instance;
	}

};