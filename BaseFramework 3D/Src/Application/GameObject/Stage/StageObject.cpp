#include "StageObject.h"

#include"Collision/GroundCollision/GroundCollision.h"
#include"Collision/AIBlockCollision/AIBlockCollision.h"

void StageObject::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Stage/Stage01/Stage01.gltf");

		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData("Asset/Models/Stage/Stage01/BackGround.gltf");

		SetObjectName("StageObject");

		SetObjectCategory(KdGameObject::ObjectCategory::Stage);

		// ステージの当たり判定クラス生成
		m_spGroundCollision = std::make_shared<GroundCollision>();
		m_spGroundCollision->Init();
		m_spGroundCollision->SetMatrix(m_mWorld);

		m_spAIBlockCollision = std::make_shared<AIBlockCollision>();
		m_spAIBlockCollision->Init();
		m_spAIBlockCollision->SetMatrix(m_mWorld);

	}
}

void StageObject::Update()
{
	m_spGroundCollision->SetMatrix(m_mWorld);
	m_spAIBlockCollision->SetMatrix(m_mWorld);
}

void StageObject::DrawInspector()
{
	DrawTransformInspector();
}

void StageObject::DrawLit()
{

	KdShaderManager::Instance().WriteCBColorEnable(true);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

	KdShaderManager::Instance().WriteCBColorEnable(false);


	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
