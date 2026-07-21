#pragma once

#include "../CharacterBase.h"

class Player : public CharacterBase
{
public:
	Player() {}
	~Player()			override {}

	void Init()			override;
	void Update()		override;

	std::string GetTypeName()const override
	{
		return "Player";
	}
};