#include "TurtleShellParameter.h"

#include"../../../../../Editor/EditorManager.h"

#include"json.hpp"
#include<fstream>


void TurtleShellParameter::Init()
{
	LoadFromJson();
}

void TurtleShellParameter::DrawInspecter()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// HP
		if (ImGui::DragInt("MaxHP", &m_param.m_maxHP, 1, 0))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 攻撃力
		if (ImGui::DragFloat("AttackPow", &m_param.m_attackPow, 1.0f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 移動スピード
		if (ImGui::DragFloat("MoveSpeed", &m_param.m_moveSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		//ジャンプパワー
		if (ImGui::DragFloat("JumpPow", &m_param.m_jumpPow, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 回転速度
		if (ImGui::DragFloat("TurnSpeed", &m_param.m_turnSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}


		// セーブ
		if (ImGui::Button("SaveParameter"))
		{
			SaveToJson();
		}
	}
}

void TurtleShellParameter::SaveToJson()
{
	nlohmann::json paramJson;

	paramJson["MaxHP"] = m_param.m_maxHP;
	paramJson["AttackPower"] = m_param.m_attackPow;
	paramJson["MoveSpeed"] = m_param.m_moveSpeed;
	paramJson["JumpPower"] = m_param.m_jumpPow;
	paramJson["TurnSpeed"] = m_param.m_turnSpeed;

	std::ofstream file("Asset/Data/Enemy/TurtleShell/Parameter/TurtleShellParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("TurtleShell parameter save filed\n");

		return;
	}

}

void TurtleShellParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/TurtleShell/Parameter/TurtleShellParameter.json");

	// もしファイルを開けないとき
	if (!file.is_open())
	{
		OutputDebugStringA("ParameterData.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;

		m_param.m_maxHP = paramJson["MaxHP"].get<int>();
		m_param.m_attackPow = paramJson["AttackPower"].get<float>();
		m_param.m_moveSpeed = paramJson["MoveSpeed"].get<float>();
		m_param.m_jumpPow = paramJson["JumpPower"].get<float>();
		m_param.m_turnSpeed = paramJson["TurnSpeed"].get<float>();
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		return;
	}
}
