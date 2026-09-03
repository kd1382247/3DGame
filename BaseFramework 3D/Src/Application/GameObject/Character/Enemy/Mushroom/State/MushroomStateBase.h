#pragma once

class Mushroom;

class MushroomStateBase
{
public:

	MushroomStateBase(){}
	virtual ~MushroomStateBase(){}

	virtual void Enter(Mushroom&mushroom){}
	virtual void Update(Mushroom& mushroom) = 0;
	virtual void Exit(Mushroom&mushroom){}

};