#include "StageObject.h"

#include"Collision/AABBCollision/AABBCollisionManager.h"
#include"Collision/OBBCollision/OBBCollisionManager.h"

#include"../PointLight/PointLightManager.h"

#include"../../System/StageLoder/StageLoder.h"
#include"../../System/StageDataManager/StageDataManager.h"
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

	// 前のモデルのデータを消す(ファイルが無かったときに、前のモデルが残らないようにする)
	m_spModel = nullptr;
	m_model = nullptr;

	// 本体のモデル(これが無いと表示できないので、無いときはエラーを出す)
	const std::filesystem::path renderPath = StageLoder::Instance().GetRenderModelPath(modelName);

	if (std::filesystem::exists(renderPath))
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData(renderPath.string());
	}
	else
	{
		KdDebugGUI::Instance().AddErrorLog("Stage model not found : %s\n", renderPath.string().c_str());
	}

	// 背景モデル(部屋や通路など、背景が要らないモデルは無くてもよい)
	const std::filesystem::path backGroundPath = StageLoder::Instance().GetBackGroundModelPath(modelName);

	if (std::filesystem::exists(backGroundPath))
	{
		m_model = std::make_shared<KdModelWork>();
		m_model->SetModelData(backGroundPath.string());
	}
}


void StageObject::ClearOwnerData()
{
	AABBCollisionManager::Instance().RemoveByOwner(this);
	OBBCollisionManager ::Instance().RemoveByOwner(this);
	WayPointManager     ::Instance().RemoveByOwner(this);
	PointLightObjectManager::Instance().RemoveByOwner(this);
}

void StageObject::PreDraw()
{
	// AABB/OBB/WayPointは回転を表現できない(またはStage自体は回転させない方針の)ため、
	// 位置と大きさだけをStageに追従させる
	AABBCollisionManager::Instance().SetStageTransform(this,GetPos(), GetScale());
	OBBCollisionManager::Instance().SetStageTransform(this,GetPos(), GetScale());
	WayPointManager::Instance().SetStageTransform(this,GetPos(), GetScale());
}

void StageObject::DrawInspectorHeader()
{
	DrawTransformInspector();
}

void StageObject::DrawInspector()
{
	if (ImGui::BeginCombo("StageModelList",m_stageModelName.c_str()))
	{

		for (const auto& name : StageLoder::Instance().GetAvailableModelNames())
		{
			if (ImGui::Selectable(name.c_str(),name==m_stageModelName))
			{

				if(name!=m_stageModelName)
				{
					LoadStageModel(name);

					// 前もモデルデータを消して、新しいモデルのデータに入れ替える
					ClearOwnerData();
					StageDataManager::Instance().LoadStageModelData(
						std::static_pointer_cast<StageObject>(shared_from_this()));

					EditorManager::Instance().MarkDirty();
				}
			}
		}
		ImGui::EndCombo();
	}

}

void StageObject::DrawLit()
{

	// モデルの読み込みに失敗していたら何も描かない
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.SetColorSphereEnable(true);

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

		KdShaderManager::Instance().m_StandardShader.SetColorSphereEnable(false);
	}

	// 背景モデルは無いモデルもある
	if (m_model)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
	}
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

void StageObject::Destroy()
{
	// この部屋が持っているデータも消す
	ClearOwnerData();

	// 編集するステージ指定されていたら解除する
	if (EditorManager::Instance().GetActiveStage().get() == this)
	{
		EditorManager::Instance().SetActiveStage(nullptr);
	}

	KdGameObject::Destroy();


}

nlohmann::json StageObject::SaveData()const
{
	nlohmann::json json = KdGameObject::SaveData();

	json["StageModel"] = m_stageModelName;

	return json;
}
