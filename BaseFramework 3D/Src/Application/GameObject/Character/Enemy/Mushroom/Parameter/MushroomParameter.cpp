#include "MushroomParameter.h"

#include"json.hpp"
#include<fstream>


void MushroomParameter::Init()
{
	LoadFromJson();
}

void MushroomParameter::DrawInspecter()
{
	// HP
	if (ImGui::DragInt("MaxHP", &m_param.m_maxHP, 1,0));
	
	// 攻撃力
	if (ImGui::DragFloat("AttackPow", &m_param.m_attackPow, 1.0f,0.0f));

	// 移動スピード
	if (ImGui::DragFloat("MoveSpeed", &m_param.m_moveSpeed, 0.01f,0.0f));

	//ジャンプパワー
	if (ImGui::DragFloat("JumpPow", &m_param.m_jumpPow, 0.01f,0.0f));

	// 回転速度
	if (ImGui::DragFloat("TurnSpeed", &m_param.m_turnSpeed, 0.01f,0.0f));

	// セーブ
	if(ImGui::Button("SaveToJson"))
	{
		SaveToJson();
	}
}

void MushroomParameter::SaveToJson()
{
	nlohmann::json paramJson;

	paramJson["MaxHP"] = m_param.m_maxHP;
	paramJson["AttackPower"] = m_param.m_attackPow;
	paramJson["MoveSpeed"] = m_param.m_moveSpeed;
	paramJson["JumpPower"] = m_param.m_jumpPow;
	paramJson["TurnSpeed"] = m_param.m_turnSpeed;

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
