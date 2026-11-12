#include "StageObject.h"

#include"Collision/AABBCollision/AABBCollisionManager.h"
#include"Collision/OBBCollision/OBBCollisionManager.h"
#include"Collision/AIBlockCollision/AIBlockManager.h"
#include"../Stage/PointLight/PointLightManager.h"
#include"../Stage/WayPoint/WayPointManager.h"

#include"../../System/StageLoder/StageLoder.h"
#include"../../System/StageDataManager/StageDataManager.h"
#include"../../Editor/EditorManager.h"
#include"../../Scene/SceneManager.h"

#include"../Gimmick/StageGimmick.h"
#include"../Character/CharacterBase.h"
#include"../../../Framework/GameObject/Component/ParentLink.h"

#include"../../Editor/StageOwnedTypeInfo/StageOwnedTypeInfo.h"

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
	for (const auto& table : GetStageOwnedTypeInfoTable())
	{
		table.second.clearOwnerData(this);
	}
}

void StageObject::DrawInspectorHeader()
{
	DrawPositionInspector();
	DrawScaleInspector();
	DrawYawInspector();

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

std::vector<std::shared_ptr<StageGimmick>> StageObject::GetOwnedGimmicks()const
{
	std::vector<std::shared_ptr<StageGimmick>> gimmicks;

	// ギミックはステージの名前で持ち主を覚えているので、名前で探す
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto gimmick = std::dynamic_pointer_cast<StageGimmick>(obj);

		if (gimmick &&
			!gimmick->IsExpired() &&
			gimmick->GetParentName() == GetObjectName())
		{
			gimmicks.push_back(gimmick);
		}
	}

	return gimmicks;
}

std::vector<std::shared_ptr<CharacterBase>> StageObject::GetOwnedCharacters() const
{

	std::vector<std::shared_ptr<CharacterBase>> characters;

	// キャラクターはステージの名前で持ち主を覚えているので、名前で探す
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto character = std::dynamic_pointer_cast<CharacterBase>(obj);

		if (!character||character->IsExpired())
		{
			continue;
		}

		if (auto parentObj = character->GetComponent<ParentLink>())
		{
			if(parentObj->GetParentName()==GetObjectName())
			{
				characters.push_back(character);
			}
		}
	}

	return characters;
}

void StageObject::DrawYawInspector()
{

	// 回転数を取得
	int quarterCount = GetYawQuarterCount();

	// どれだけ動かすのか
	int rotateValue = 0;

	// 回転角度を表示
	ImGui::Text("Rotate %d", quarterCount * 90);


	// 2つのボタンの合計幅を、Position / Scale の入力欄と同じ幅にそろえる
	const float buttonWidth =
		(ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x) * 0.5f;


	if (ImGui::Button("-90##RotYMinus", ImVec2(buttonWidth, 0.0f)))
	{
		rotateValue = -1;
	}

	ImGui::SameLine();

	if (ImGui::Button("+90##RotYPlus", ImVec2(buttonWidth, 0.0f)))
	{
		rotateValue = 1;
	}


	if (rotateValue != 0)
	{
		quarterCount += rotateValue;
		// 0～3に収める
		quarterCount = ((quarterCount % 4) + 4) % 4;

		SetRotation(Math::Vector3(0.0f, quarterCount * 90.0f, 0.0f));

		EditorManager::Instance().MarkDirty();
	}

}

void StageObject::Destroy()
{
	// この部屋が持っているデータも消す
	ClearOwnerData();

	// この部屋のギミックも消す(残ると、持ち主のいないまま動いたり、Hierarchyから選べなくなったりする)
	for (const auto& gimmick : GetOwnedGimmicks())
	{
		gimmick->Destroy();
	}

	// この部屋のキャラクターも消す
	for (const auto& character : GetOwnedCharacters())
	{
		character->Destroy();
	}

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
