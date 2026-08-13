#include "WallCollision.h"

#include"../../../../../System/CollisionManager/CollisionManager.h"

void WallCollision::Init()
{
	if (!m_spModel)
	{

		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Stage/Stage01/WallCollision.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("WallCollision", m_spModel, KdCollider::TypeBump);

		CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());

	}

}
