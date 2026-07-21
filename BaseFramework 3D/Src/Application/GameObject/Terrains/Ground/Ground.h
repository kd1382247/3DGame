#pragma once

class Ground :public KdGameObject
{
public:

	Ground ()                  {}
	~Ground()       override   {}

	void Init()     override;
	void Update()   override;
	void DrawLit()  override;

	std::string GetTypeName()const override
	{
		return "Ground";
	}

private:

	std::shared_ptr<KdModelWork>m_spModel = nullptr;


};