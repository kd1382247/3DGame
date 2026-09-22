#include "SlimeParameter.h"

#include"../../../../../Editor/EditorManager.h"

#include"json.hpp"

SlimeParameter::Parameter SlimeParameter::GetParam(const SlimeSize size) const
{
	if (size == SlimeSize::Large)
	{
		return m_paramLarge;
	}
	else
	{
		return m_paramSmall;
	}
}

void SlimeParameter::Init()
{
	LoadFromJson();
}

void SlimeParameter::DrawInspecter()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Large", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// HP
			if (ImGui::DragInt("MaxHP##Large", &m_paramLarge.m_maxHP, 1, 0))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 攻撃力
			if (ImGui::DragFloat("AttackPow##Large", &m_paramLarge.m_attackPower, 1.0f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 移動スピード
			if (ImGui::DragFloat("MoveSpeed##Large", &m_paramLarge.m_moveSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			//ジャンプパワー
			if (ImGui::DragFloat("JumpPow##Large", &m_paramLarge.m_jumpPow, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 回転速度
			if (ImGui::DragFloat("TurnSpeed##Large", &m_paramLarge.m_turnSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 拡大率
			if (ImGui::DragFloat("Scale##Large", &m_paramLarge.m_scale, 0.01f, 0.0001f))
			{
				EditorManager::Instance().MarkDirty();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Small", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// HP
			if (ImGui::DragInt("MaxHP##Small", &m_paramSmall.m_maxHP, 1, 0))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 攻撃力
			if (ImGui::DragFloat("AttackPow##Small", &m_paramSmall.m_attackPower, 1.0f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 移動スピード
			if (ImGui::DragFloat("MoveSpeed##Small", &m_paramSmall.m_moveSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			//ジャンプパワー
			if (ImGui::DragFloat("JumpPow##Small", &m_paramSmall.m_jumpPow, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 回転速度
			if (ImGui::DragFloat("TurnSpeed##Small", &m_paramSmall.m_turnSpeed, 0.01f, 0.0f))
			{
				EditorManager::Instance().MarkDirty();
			}

			// 拡大率
			if (ImGui::DragFloat("Scale##Small", &m_paramSmall.m_scale, 0.01f, 0.0001f))
			{
				EditorManager::Instance().MarkDirty();
			}

			ImGui::TreePop();
		}

		// セーブ
		if (ImGui::Button("SaveParameter"))
		{
			SaveToJson();
		}
	}
}

void SlimeParameter::SaveToJson()
{
	auto toJson = [](const Parameter& param)
	{
		nlohmann::json paramJson;

		paramJson["MaxHP"] = param.m_maxHP;
		paramJson["AttackPower"] = param.m_attackPower;
		paramJson["MoveSpeed"] = param.m_moveSpeed;
		paramJson["JumpPower"] = param.m_jumpPow;
		paramJson["TurnSpeed"] = param.m_turnSpeed;
		paramJson["Scale"] = param.m_scale;

		return paramJson;
	};

	nlohmann::json paramJson;

	paramJson["Large"] = toJson(m_paramLarge);
	paramJson["Small"] = toJson(m_paramSmall);

	std::ofstream file("Asset/Data/Enemy/Slime/Parameter/SlimeParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Slime parameter save filed\n");

		return;
	}

}

void SlimeParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Slime/Parameter/SlimeParameter.json");

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
		param.m_scale = json["Scale"].get<float>();
	};

	try
	{
		file >> paramJson;

		fromJson(paramJson["Large"], m_paramLarge);
		fromJson(paramJson["Small"], m_paramSmall);
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		return;
	}
}
