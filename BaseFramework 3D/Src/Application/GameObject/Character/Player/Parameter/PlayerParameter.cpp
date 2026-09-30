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
		if (ImGui::DragInt("MaxHP", &m_body.m_maxHP, 1, 0))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 攻撃力
		if (ImGui::DragFloat("AttackPow", &m_attack.m_attackPower, 1.0f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 攻撃中の移動スピード
		if (ImGui::DragFloat("AttackMoveSpeed", &m_attack.m_attackMoveSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 必殺技の攻撃力
		if (ImGui::DragFloat("SpecialAttackPow", &m_specialMove.m_attackPower, 1.0f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 必殺技の多段ヒット間隔(フレーム)
		if (ImGui::DragFloat("SpecialHitCooldown", &m_specialMove.m_hitCooldownDuration, 0.1f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 移動スピード
		if (ImGui::DragFloat("MoveSpeed", &m_move.m_moveSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 必殺技の移動スピード
		if (ImGui::DragFloat("SpecialMoveSpeed", &m_specialMove.m_moveSpeed, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		//ジャンプパワー
		if (ImGui::DragFloat("JumpPow", &m_jump.m_jumpPow, 0.01f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 重力加速度
		if (ImGui::DragFloat("GravityAccel", &m_body.m_gravityAcceleration, 0.1f, 0.0f))
		{
			EditorManager::Instance().MarkDirty();
		}

		// 回転速度
		if (ImGui::DragFloat("TurnSpeed", &m_body.m_turnSpeed, 0.01f, 0.0f))
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

	paramJson["MaxHP"] = m_body.m_maxHP;
	paramJson["TurnSpeed"] = m_body.m_turnSpeed;
	paramJson["GravityAcceleration"] = m_body.m_gravityAcceleration;

	paramJson["MoveSpeed"] = m_move.m_moveSpeed;

	paramJson["JumpPower"] = m_jump.m_jumpPow;

	paramJson["AttackPower"] = m_attack.m_attackPower;
	paramJson["AttackMoveSpeed"] = m_attack.m_attackMoveSpeed;

	paramJson["SpecialAttackPower"] = m_specialMove.m_attackPower;
	paramJson["SpecialMoveSpeed"] = m_specialMove.m_moveSpeed;
	paramJson["SpecialHitCooldown"] = m_specialMove.m_hitCooldownDuration;

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

		m_body.m_maxHP = paramJson["MaxHP"].get<int>();
		m_body.m_turnSpeed = paramJson["TurnSpeed"].get<float>();

		m_move.m_moveSpeed = paramJson["MoveSpeed"].get<float>();

		m_jump.m_jumpPow = paramJson["JumpPower"].get<float>();

		m_attack.m_attackPower = paramJson["AttackPower"].get<float>();

		// 新しく追加した項目は、古いセーブデータには無い場合があるため
		// contains()で確認してから読み込む(無ければデフォルト値のまま)
		if (paramJson.contains("GravityAcceleration"))
		{
			m_body.m_gravityAcceleration = paramJson["GravityAcceleration"].get<float>();
		}

		if (paramJson.contains("AttackMoveSpeed"))
		{
			m_attack.m_attackMoveSpeed = paramJson["AttackMoveSpeed"].get<float>();
		}

		if (paramJson.contains("SpecialAttackPower"))
		{
			m_specialMove.m_attackPower = paramJson["SpecialAttackPower"].get<float>();
		}

		if (paramJson.contains("SpecialMoveSpeed"))
		{
			m_specialMove.m_moveSpeed = paramJson["SpecialMoveSpeed"].get<float>();
		}

		if (paramJson.contains("SpecialHitCooldown"))
		{
			m_specialMove.m_hitCooldownDuration = paramJson["SpecialHitCooldown"].get<float>();
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
