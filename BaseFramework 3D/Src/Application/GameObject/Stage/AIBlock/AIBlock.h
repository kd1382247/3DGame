#pragma once

class AIBlock :public KdGameObject
{
public:

	AIBlock(){}
	~AIBlock()override{}

	void Init()override;
	void DrawDebug()override;

private:


	Math::Vector3 m_size = { 1.0f,1.0f,1.0f };

};