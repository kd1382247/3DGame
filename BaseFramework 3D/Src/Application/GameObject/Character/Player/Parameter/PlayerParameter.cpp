#include "PlayerParameter.h"

#include"../../../../Editor/EditorManager.h"

#include"json.hpp"
#include<fstream>


void PlayerParameter::Init()
{
	LoadFromJson();
}

void PlayerParameter::DrawInspecter()
{
	if (ImGui::CollapsingHeader("Parameter", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// HP
		if (ImGui::DragInt("MaxHP", &m_param.m_maxHP, 1, 0))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 攻撃力
		if (ImGui::DragFloat("AttackPow", &m_param.m_attackPower, 1.0f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 必殺技の攻撃力
		if (ImGui::DragFloat("SpecialAttackPow", &m_param.m_specialAttackPower, 1.0f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 移動スピード
		if (ImGui::DragFloat("MoveSpeed", &m_param.m_moveSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 必殺技の移動スピード
		if (ImGui::DragFloat("SpecialMoveSpeed", &m_param.m_specialMoveSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		//ジャンプパワー
		if (ImGui::DragFloat("JumpPow", &m_param.m_jumpPow, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 重力加速度
		if (ImGui::DragFloat("GravityAccel", &m_param.m_gravityAcceleration, 0.1f, 0.0f))
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

void PlayerParameter::SaveToJson()
{
	nlohmann::json paramJson;

	paramJson["MaxHP"] = m_param.m_maxHP;
	paramJson["AttackPower"] = m_param.m_attackPower;
	paramJson["SpecialAttackPower"] = m_param.m_specialAttackPower;
	paramJson["MoveSpeed"] = m_param.m_moveSpeed;
	paramJson["SpecialMoveSpeed"] = m_param.m_specialMoveSpeed;
	paramJson["JumpPower"] = m_param.m_jumpPow;
	paramJson["GravityAcceleration"] = m_param.m_gravityAcceleration;
	paramJson["TurnSpeed"] = m_param.m_turnSpeed;

	std::ofstream file("Asset/Data/Player/Parameter/PlayerParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Player parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Player parameter save filed\n");

		return;
	}
}

void PlayerParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Player/Parameter/PlayerParameter.json");

	// もしファイルを開けないとき
	if (!file.is_open())
	{
		OutputDebugStringA("ParameterData.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("ParameterData.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;

		m_param.m_maxHP = paramJson["MaxHP"].get<int>();
		m_param.m_attackPower = paramJson["AttackPower"].get<float>();
		m_param.m_moveSpeed = paramJson["MoveSpeed"].get<float>();
		m_param.m_jumpPow = paramJson["JumpPower"].get<float>();
		m_param.m_turnSpeed = paramJson["TurnSpeed"].get<float>();

		// 新しく追加した項目は、古いセーブデータには無い場合があるため
		// contains()で確認してから読み込む(無ければデフォルト値のまま)
		if (paramJson.contains("SpecialAttackPower"))
		{
			m_param.m_specialAttackPower = paramJson["SpecialAttackPower"].get<float>();
		}

		if (paramJson.contains("SpecialMoveSpeed"))
		{
			m_param.m_specialMoveSpeed = paramJson["SpecialMoveSpeed"].get<float>();
		}

		if (paramJson.contains("GravityAcceleration"))
		{
			m_param.m_gravityAcceleration = paramJson["GravityAcceleration"].get<float>();
		}
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("JSONの読み込みに失敗しました\n");
		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("JSONの読み込みに失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());
		return;
	}
}
