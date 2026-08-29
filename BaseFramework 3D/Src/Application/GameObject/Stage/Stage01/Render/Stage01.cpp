#include "Stage01.h"

#include"../Collision/GroundCollision/GroundCollision.h"
#include"../Collision/WallCollision/WallCollision.h"
#include"../Collision/AIBlockCollision/AIBlockCollision.h"

void Stage01::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Stage/Stage01/Stage01.gltf");

		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData("Asset/Models/Stage/Stage01/BackGround.gltf");

		SetObjectName("Stage01");

		SetObjectCategory(KdGameObject::ObjectCategory::Stage);

		// ステージの当たり判定クラス生成
		m_spGroundCollision = std::make_shared<GroundCollision>();
		m_spGroundCollision->Init();
		m_spGroundCollision->SetMatrix(m_mWorld);

		m_spWallCollision = std::make_shared<WallCollision>();
		m_spWallCollision->Init();
		m_spWallCollision->SetMatrix(m_mWorld);

		m_spAIBlockCollision = std::make_shared<AIBlockCollision>();
		m_spAIBlockCollision->Init();
		m_spAIBlockCollision->SetMatrix(m_mWorld);

	}
}

void Stage01::Update()
{
	m_spGroundCollision->SetMatrix(m_mWorld);
	m_spWallCollision->SetMatrix(m_mWorld);
	m_spAIBlockCollision->SetMatrix(m_mWorld);
}

void Stage01::DrawInspector()
{
	DrawTransformInspector();
}

void Stage01::DrawLit()
{
	StageBase::DrawLit();

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
