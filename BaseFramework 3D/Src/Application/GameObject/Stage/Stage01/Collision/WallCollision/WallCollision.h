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


	void SetScale(const Math::Vector3& scale)override { m_scale = scale; }
	Math::Vector3 GetScale()const override { return m_scale; }


	DirectX::BoundingBox GetBox()const;


	int GetID() { return m_id; }
	void SetID(int id) { m_id = id; }

	void DrawInspector()override;

	void Destroy()override;


private:

	Math::Vector3 m_scale = { 1,1,1 };

	int m_id = -1;
};
