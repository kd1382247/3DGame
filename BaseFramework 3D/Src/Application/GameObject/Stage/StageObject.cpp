#include "StageObject.h"

#include"Collision/GroundCollision/GroundCollision.h"
#include"Collision/AIBlockCollision/AIBlockCollision.h"
#include"Collision/AABBCollision/AABBCollisionManager.h"
#include"Collision/OBBCollision/OBBCollisionManager.h"

#include"../../System/StageLoder/StageLoder.h"
#include"../../System/WayPointManager/WayPointManager.h"
#include"../../Editor/EditorManager.h"

void StageObject::Init()
{
	SetObjectName("StageObject");

	SetObjectCategory(KdGameObject::ObjectCategory::Stage);

	if (!m_spModel)
	{
		// この時点ではまだLoadData()が呼ばれていないため、デフォルトのモデル(m_stageModelName)を読み込む
		// セーブデータから読み込む場合は、この後LoadData()で正しいモデル名に差し替わる
		LoadStageModel(m_stageModelName);
	}
}

void StageObject::LoadStageModel(const std::string& modelName)
{
	m_stageModelName = modelName;

	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData(StageLoder::Instance().GetRenderModelPath(modelName).string());

	m_model = std::make_shared<KdModelWork>();
	m_model->SetModelData(StageLoder::Instance().GetBackGroundModelPath(modelName).string());

	// ステージの当たり判定クラス生成
	m_spGroundCollision = std::make_shared<GroundCollision>();
	m_spGroundCollision->SetModelPath(StageLoder::Instance().GetGroundCollisionPath(modelName));
	m_spGroundCollision->Init();
	m_spGroundCollision->SetMatrix(m_mWorld);

	m_spAIBlockCollision = std::make_shared<AIBlockCollision>();
	m_spAIBlockCollision->SetModelPath(StageLoder::Instance().GetAIBlockCollisionPath(modelName));
	m_spAIBlockCollision->Init();
	m_spAIBlockCollision->SetMatrix(m_mWorld);
}

void StageObject::PreDraw()
{
	// EditModeではUpdate()が呼ばれない(BaseScene::Update()参照)ため、
	// EditModeでもPlayModeでも必ず呼ばれるPreDraw()でStageへの追従処理を行う
	m_spGroundCollision->SetMatrix(m_mWorld);
	m_spAIBlockCollision->SetMatrix(m_mWorld);

	// AABB/OBB/WayPointは回転を表現できない(またはStage自体は回転させない方針の)ため、
	// 位置と大きさだけをStageに追従させる
	AABBCollisionManager::Instance().SetStageTransform(GetPos(), GetScale());
	OBBCollisionManager::Instance().SetStageTransform(GetPos(), GetScale());
	WayPointManager::Instance().SetStageTransform(GetPos(), GetScale());
}

void StageObject::DrawInspector()
{
	DrawTransformInspector();

	if (ImGui::BeginCombo("StageModelList",m_stageModelName.c_str()))
	{

		for (const auto& name : StageLoder::Instance().GetAvailableModelNames())
		{
			if (ImGui::Selectable(name.c_str(),name==m_stageModelName))
			{
				if (name != m_stageModelName)
				{
					LoadStageModel(name);
					EditorManager::Instance().MarkDirty();
				}
			}
		}
		ImGui::EndCombo();
	}

}

void StageObject::DrawLit()
{

	KdShaderManager::Instance().WriteCBColorEnable(true);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

	KdShaderManager::Instance().WriteCBColorEnable(false);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void StageObject::LoadData(const nlohmann::json& json)
{
	if (json.contains("StageModel"))
	{
		const std::string modelName = json["StageModel"].get<std::string>();

		// Init()で読み込んだデフォルトモデルと違う場合のみ読み直す(無駄な二重読込を避ける)
		if (modelName != m_stageModelName || !m_spModel)
		{
			LoadStageModel(modelName);
		}
	}
}

nlohmann::json StageObject::SaveData()const
{
	nlohmann::json json = KdGameObject::SaveData();

	json["StageModel"] = m_stageModelName;

	return json;
}
