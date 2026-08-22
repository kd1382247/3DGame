#pragma once

#include"../../../StageBase.h"

class WallCollision :public StageBase
{

public:
	WallCollision() {}
	~WallCollision()override {}

	void Init()override;
	void DrawLit()override {}

	void DrawDebug()override;

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }


	DirectX::BoundingBox GetBox()const;


	int GetID() { return m_id; }
	void SetID(int id) { m_id = id; }

	void DrawInspector()override;

	void Destroy()override;


private:

	int m_id = -1;
};
