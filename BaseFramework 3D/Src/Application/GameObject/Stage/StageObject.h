#pragma once

class GroundCollision;
class AIBlockCollision;

class StageObject :public KdGameObject
{

public:
	StageObject(){}
	~StageObject()override{}

	void Init()override;
	void Update()override;
	void DrawInspector()override;
	void DrawLit()override;


private:



	std::shared_ptr<KdModelWork>m_model = nullptr;

	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	std::shared_ptr<GroundCollision> m_spGroundCollision=nullptr;
	std::shared_ptr<AIBlockCollision>m_spAIBlockCollision = nullptr;


};