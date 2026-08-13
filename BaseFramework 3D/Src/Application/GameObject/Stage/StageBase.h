#pragma once

class StageBase : public KdGameObject
{
public:
	StageBase() {}
	virtual ~StageBase()	override {}

	void Init()				override;
	void Update()			override;
	void DrawLit()			override;

protected:

	std::shared_ptr<KdModelWork> m_spModel = nullptr;

};