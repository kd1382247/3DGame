#pragma once

#include"../../../StageBase.h"

class OBBCollision :public StageBase
{

public:

	enum class OBBCollisionType
	{
		Solid,
		Walkable
	};

	OBBCollision() {}
	~OBBCollision()override {}

	void Init()override;
	void DrawLit()override {}

	void DrawDebug()override;

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }

	void SetCollisionType(OBBCollisionType type) { m_collisionType = type; }
	OBBCollisionType GetCollisionType()const     { return m_collisionType; }


	DirectX::BoundingOrientedBox GetBox()const;


	int GetID() { return m_id; }
	void SetID(int id) { m_id = id; }

	void DrawInspector()override;

	void Destroy()override;



private:

	int m_id = -1;

	OBBCollisionType m_collisionType = OBBCollisionType::Walkable;
};
