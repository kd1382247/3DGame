#include "Box.h"

#include"../../System/CollisionManager/CollisionManager.h"

void Box::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/Box.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Box", m_spModel,KdCollider::TypeGround|KdCollider::TypeBump);

	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Ground, shared_from_this());
	CollisionManager::Instance().RegisterObject(CollisionLayer::Bump, shared_from_this());

}

void Box::Update()
{
	if (GetAsyncKeyState('T') & 0x8000)m_isExpired = true;
}

void Box::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel);
}
