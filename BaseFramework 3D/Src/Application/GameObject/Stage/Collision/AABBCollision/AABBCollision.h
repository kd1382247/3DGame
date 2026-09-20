#pragma once

class AABBCollision :public KdGameObject
{

public:
	AABBCollision() {}
	~AABBCollision()override {}

	void Init()override;

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
