#pragma once

#include"../../StageBase.h"

class GroundCollision;
class WallCollision;
class AIBlockCollision;

class Stage01 :public StageBase
{

public:
	Stage01(){}
	~Stage01()override{}

	void Init()override;

private:


	std::shared_ptr<GroundCollision> m_spGroundCollision=nullptr;
	std::shared_ptr<WallCollision>   m_spWallCollision=nullptr;
	std::shared_ptr<AIBlockCollision>m_spAIBlockCollision = nullptr;


};