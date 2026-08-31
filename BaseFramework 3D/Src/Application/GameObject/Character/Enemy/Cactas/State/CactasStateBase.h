#pragma once

class Cactas;

class CactasStateBase
{
public:

	CactasStateBase(){}
	virtual ~CactasStateBase(){}

	virtual void Enter(Cactas&cactas){}
	virtual void Update(Cactas& cactas) = 0;
	virtual void Exit(Cactas&cactas){}

};