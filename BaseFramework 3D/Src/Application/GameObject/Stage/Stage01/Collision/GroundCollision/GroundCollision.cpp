#include "GroundCollision.h"

#include"../../../../../System/CollisionManager/CollisionManager.h"

void GroundCollision::Init()
{
	if (!m_spModel)
	{

		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Stage/Stage01/GroundCollision.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("GroundCollision", m_spModel, KdCollider::TypeGround);

		CollisionManager::Instance().RegisterObject(CollisionLayer::Ground, shared_from_this());

	}

}
