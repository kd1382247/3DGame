#pragma once

class Box :public KdGameObject
{

public:
	Box(){}
	~Box()override{}

	void Init()override;
	void Update()override;
	void DrawLit()override;


private:

	std::shared_ptr<KdModelData>m_spModel;


};