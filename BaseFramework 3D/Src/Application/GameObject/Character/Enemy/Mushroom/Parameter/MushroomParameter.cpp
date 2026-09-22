#include "MushroomParameter.h"

#include"../../../../../Editor/EditorManager.h"

#include"json.hpp"
#include<fstream>


MushroomParameter::Parameter MushroomParameter::GetParam(const MushroomType type) const
{
	if (type == MushroomType::Angry)
	{
		return m_paramAngry;
	}
	else
	{
		return m_paramSmile;
	}
}

void MushroomParameter::Init()
{
	LoadFromJson();
}

void MushroomParameter::DrawInspecter()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Smile", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// HP
			if (ImGui::DragInt("MaxHP##Smile", &m_paramSmile.m_maxHP, 1, 0))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 攻撃力
			if (ImGui::DragFloat("AttackPow##Smile", &m_paramSmile.m_attackPower, 1.0f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 移動スピード
			if (ImGui::DragFloat("MoveSpeed##Smile", &m_paramSmile.m_moveSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			//ジャンプパワー
			if (ImGui::DragFloat("JumpPow##Smile", &m_paramSmile.m_jumpPow, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 回転速度
			if (ImGui::DragFloat("TurnSpeed##Smile", &m_paramSmile.m_turnSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Angry", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// HP
			if (ImGui::DragInt("MaxHP##Angry", &m_paramAngry.m_maxHP, 1, 0))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 攻撃力
			if (ImGui::DragFloat("AttackPow##Angry", &m_paramAngry.m_attackPower, 1.0f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 移動スピード
			if (ImGui::DragFloat("MoveSpeed##Angry", &m_paramAngry.m_moveSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			//ジャンプパワー
			if (ImGui::DragFloat("JumpPow##Angry", &m_paramAngry.m_jumpPow, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 回転速度
			if (ImGui::DragFloat("TurnSpeed##Angry", &m_paramAngry.m_turnSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			ImGui::TreePop();
		}

		// セーブ
		if (ImGui::Button("SaveToJson"))
		{
			SaveToJson();
		}
	}
}

void MushroomParameter::SaveToJson()
{
	auto toJson = [](const Parameter& param)
	{
		nlohmann::json paramJson;

		paramJson["MaxHP"] = param.m_maxHP;
		paramJson["AttackPower"] = param.m_attackPower;
		paramJson["MoveSpeed"] = param.m_moveSpeed;
		paramJson["JumpPower"] = param.m_jumpPow;
		paramJson["TurnSpeed"] = param.m_turnSpeed;

		return paramJson;
	};

	nlohmann::json paramJson;

	paramJson["Smile"] = toJson(m_paramSmile);
	paramJson["Angry"] = toJson(m_paramAngry);

	std::ofstream file("Asset/Data/Enemy/Mushroom/Parameter/MushroomParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Mushroom parameter save filed\n");

		return;
	}

}

void MushroomParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Mushroom/Parameter/MushroomParameter.json");

	// もしファイルを開けないとき
	if (!file.is_open())
	{
		OutputDebugStringA("ParameterData.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	auto fromJson = [](const nlohmann::json& json, Parameter& param)
	{
		param.m_maxHP = json["MaxHP"].get<int>();
		param.m_attackPower = json["AttackPower"].get<float>();
		param.m_moveSpeed = json["MoveSpeed"].get<float>();
		param.m_jumpPow = json["JumpPower"].get<float>();
		param.m_turnSpeed = json["TurnSpeed"].get<float>();
	};

	try
	{
		file >> paramJson;

		fromJson(paramJson["Smile"], m_paramSmile);
		fromJson(paramJson["Angry"], m_paramAngry);
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		return;
	}
}
