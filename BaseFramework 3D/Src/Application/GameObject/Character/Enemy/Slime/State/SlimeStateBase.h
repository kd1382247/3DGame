#pragma once

class Slime;

class SlimeStateBase
{
public:

	SlimeStateBase() {}
	virtual ~SlimeStateBase() {}

	virtual void Enter(Slime& slime) {}
	virtual void Update(Slime& slime) = 0;
	virtual void Exit(Slime& slime) {}

};