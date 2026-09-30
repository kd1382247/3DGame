#include "PlayerParameter.h"

#include"../../../../Editor/EditorManager.h"
#include"../../../../Editor/InspectorWidgets.h"
#include"../../ParameterJson.h"

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
		// 単位の説明(m / s / F / ° など)
		InspectorUI::UnitLegend();

		// セーブ
		if (ImGui::Button("SaveParameter"))
		{
			SaveToJson();
		}

		if (InspectorUI::BeginGroup("Body"))
		{
			InspectorUI::HP("MaxHP", m_body.m_maxHP);
			InspectorUI::DegreePerFrame("TurnSpeed", m_body.m_turnSpeed);
			InspectorUI::Acceleration("GravityAccel", m_body.m_gravityAcceleration);
			InspectorUI::Rate("BumpPushRate", m_body.m_bumpPushRate);
			InspectorUI::Degree("MaxSlopeAngle", m_body.m_maxWalkableSlopeAngle);
			InspectorUI::Meter("StepHeight", m_body.m_stepHeight);
			InspectorUI::Meter("GroundRayLength", m_body.m_groundRayLength);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Move"))
		{
			InspectorUI::MeterPerFrame("MoveSpeed", m_move.m_moveSpeed);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Jump"))
		{
			InspectorUI::Power("JumpPower", m_jump.m_jumpPow);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack"))
		{
			InspectorUI::Damage("AttackPower", m_attack.m_attackPower);
			InspectorUI::MeterPerFrame("AttackMoveSpeed", m_attack.m_attackMoveSpeed);
			InspectorUI::Meter("HitRadius", m_attack.m_hitRadius);
			InspectorUI::Meter("HitForwardOffset", m_attack.m_hitForwardOffset);
			InspectorUI::Power("KnockBackPower", m_attack.m_knockBackPower);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Special Move"))
		{
			InspectorUI::Damage("AttackPower", m_specialMove.m_attackPower);
			InspectorUI::MeterPerFrame("MoveSpeed", m_specialMove.m_moveSpeed);
			InspectorUI::Frame("HitCooldown", m_specialMove.m_hitCooldownDuration);
			InspectorUI::Meter("HitRadius", m_specialMove.m_hitRadius);
			InspectorUI::Power("KnockBackPower", m_specialMove.m_knockBackPower);

			InspectorUI::EndGroup();
		}
	}
}

void PlayerParameter::SaveToJson()
{
	nlohmann::json paramJson;

	paramJson["MaxHP"] = m_body.m_maxHP;
	paramJson["TurnSpeed"] = m_body.m_turnSpeed;
	paramJson["GravityAcceleration"] = m_body.m_gravityAcceleration;
	paramJson["BumpPushRate"] = m_body.m_bumpPushRate;
	paramJson["MaxWalkableSlopeAngle"] = m_body.m_maxWalkableSlopeAngle;
	paramJson["StepHeight"] = m_body.m_stepHeight;
	paramJson["GroundRayLength"] = m_body.m_groundRayLength;

	paramJson["MoveSpeed"] = m_move.m_moveSpeed;

	paramJson["JumpPower"] = m_jump.m_jumpPow;

	paramJson["AttackPower"] = m_attack.m_attackPower;
	paramJson["AttackMoveSpeed"] = m_attack.m_attackMoveSpeed;
	paramJson["AttackHitRadius"] = m_attack.m_hitRadius;
	paramJson["AttackHitForwardOffset"] = m_attack.m_hitForwardOffset;
	paramJson["AttackKnockBackPower"] = m_attack.m_knockBackPower;

	paramJson["SpecialAttackPower"] = m_specialMove.m_attackPower;
	paramJson["SpecialMoveSpeed"] = m_specialMove.m_moveSpeed;
	paramJson["SpecialHitCooldown"] = m_specialMove.m_hitCooldownDuration;
	paramJson["SpecialHitRadius"] = m_specialMove.m_hitRadius;
	paramJson["SpecialKnockBackPower"] = m_specialMove.m_knockBackPower;

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

		// 新しく追加した項目は、古いセーブデータには無い場合があるため
		// キーがある項目だけ読み込む(無ければデフォルト値のまま)
		ParameterJson::Read(paramJson, "MaxHP", m_body.m_maxHP);
		ParameterJson::Read(paramJson, "TurnSpeed", m_body.m_turnSpeed);
		ParameterJson::Read(paramJson, "GravityAcceleration", m_body.m_gravityAcceleration);
		ParameterJson::Read(paramJson, "BumpPushRate", m_body.m_bumpPushRate);
		ParameterJson::Read(paramJson, "MaxWalkableSlopeAngle", m_body.m_maxWalkableSlopeAngle);
		ParameterJson::Read(paramJson, "StepHeight", m_body.m_stepHeight);
		ParameterJson::Read(paramJson, "GroundRayLength", m_body.m_groundRayLength);

		ParameterJson::Read(paramJson, "MoveSpeed", m_move.m_moveSpeed);

		ParameterJson::Read(paramJson, "JumpPower", m_jump.m_jumpPow);

		ParameterJson::Read(paramJson, "AttackPower", m_attack.m_attackPower);
		ParameterJson::Read(paramJson, "AttackMoveSpeed", m_attack.m_attackMoveSpeed);
		ParameterJson::Read(paramJson, "AttackHitRadius", m_attack.m_hitRadius);
		ParameterJson::Read(paramJson, "AttackHitForwardOffset", m_attack.m_hitForwardOffset);
		ParameterJson::Read(paramJson, "AttackKnockBackPower", m_attack.m_knockBackPower);

		ParameterJson::Read(paramJson, "SpecialAttackPower", m_specialMove.m_attackPower);
		ParameterJson::Read(paramJson, "SpecialMoveSpeed", m_specialMove.m_moveSpeed);
		ParameterJson::Read(paramJson, "SpecialHitCooldown", m_specialMove.m_hitCooldownDuration);
		ParameterJson::Read(paramJson, "SpecialHitRadius", m_specialMove.m_hitRadius);
		ParameterJson::Read(paramJson, "SpecialKnockBackPower", m_specialMove.m_knockBackPower);
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
