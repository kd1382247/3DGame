#include "Box.h"

void Box::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelData>();
		m_spModel->Load("Asset/Models/Terrains/Box.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Box", m_spModel,KdCollider::TypeGround);

	}
}

void Box::Update()
{
	if (GetAsyncKeyState('T') & 0x8000)m_isExpired = true;
}

void Box::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel);
}
