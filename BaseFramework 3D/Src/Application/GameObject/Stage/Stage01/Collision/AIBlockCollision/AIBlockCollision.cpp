#include "AIBlockCollision.h"

#include"../../../../../System/CollisionManager/CollisionManager.h"

void AIBlockCollision::Init()
{
	if (!m_spModel)
	{

		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Stage/Stage01/AIBlock.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("AIBlockCollision", m_spModel, KdCollider::TypeSight);

		CollisionManager::Instance().RegisterObject(CollisionLayer::AIBlock, shared_from_this());

	}

}
