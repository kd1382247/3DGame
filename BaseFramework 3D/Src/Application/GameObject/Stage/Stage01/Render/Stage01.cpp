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

		SetPos({ 0.0f,0.0f,0.0f });

		SetObjectName("Stage01");

		SetObjectCategory(KdGameObject::ObjectCategory::Stage);

		// ステージの当たり判定クラス生成
		m_spGroundCollision = std::make_shared<GroundCollision>();
		m_spGroundCollision->Init();

		m_spWallCollision = std::make_shared<WallCollision>();
		m_spWallCollision->Init();

		m_spAIBlockCollision = std::make_shared<AIBlockCollision>();
		m_spAIBlockCollision->Init();

	}
}
