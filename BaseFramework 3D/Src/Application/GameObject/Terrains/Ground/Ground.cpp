#include "Ground.h"

#include"../../../System/CollisionManager/CollisionManager.h"

void Ground::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Terrains/Ground/Ground.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Ground", m_spModel, KdCollider::TypeGround);

		m_objectName = "Ground";
	}


	CollisionManager::Instance().RegisterObject(CollisionLayer::Ground, shared_from_this());
}

void Ground::Update()
{

}

void Ground::DrawLit()
{

	if (!m_spModel)return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel);

}
