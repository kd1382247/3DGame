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
			InspectorUI::HP            ("MaxHP",           m_body.m_maxHP);
			InspectorUI::DegreePerFrame("TurnSpeed",       m_body.m_turnSpeed);
			InspectorUI::Acceleration  ("GravityAccel",    m_body.m_gravityAcceleration);
			InspectorUI::Rate          ("BumpPushRate",    m_body.m_bumpPushRate);
			InspectorUI::Degree        ("MaxSlopeAngle",   m_body.m_maxWalkableSlopeAngle);
			InspectorUI::Meter         ("StepHeight",      m_body.m_stepHeight);
			InspectorUI::Meter         ("GroundRayLength", m_body.m_groundRayLength);

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

			InspectorUI::MeterPerFrame("AttackMoveSpeed", m_attack.m_attackMoveSpeed);
			InspectorUI::Meter        ("HitRadius",       m_attack.m_hitRadius);
			InspectorUI::Meter        ("HitForwardOffset",m_attack.m_hitForwardOffset);

			for (int i = 0; i < kComboCount; i++)
			{
				std::string combo = "Combo"+std::to_string(i + 1);

				if (InspectorUI::BeginGroup(combo.c_str()))
				{
					HitParam& hit = m_attack.m_hitParam[i];

					InspectorUI::Damage ("AttackPower",      hit.m_attackPower);
					InspectorUI::Power  ("KnockBackPower",   hit.m_knockBackPower);
					InspectorUI::Seconds("HitStop",          hit.m_hitStop);
					InspectorUI::Seconds("KillHitStop",      hit.m_killHitStop);
					InspectorUI::Scale  ("KillSlowScale",    hit.m_killSlowScale);
					InspectorUI::Seconds("KillSlowDuration", hit.m_killSlowDuration);

					InspectorUI::EndGroup();
				}
			}

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Special Move"))
		{
			InspectorUI::Damage		  ("AttackPower",      m_specialMove.m_hitParam.m_attackPower);
			InspectorUI::Power		  ("KnockBackPower",   m_specialMove.m_hitParam.m_knockBackPower);
			InspectorUI::Seconds	  ("HitStop",          m_specialMove.m_hitParam.m_hitStop);
			InspectorUI::Seconds	  ("KillHitStop",      m_specialMove.m_hitParam.m_killHitStop);
			InspectorUI::Scale        ("KillSlowScale",    m_specialMove.m_hitParam.m_killSlowScale);
			InspectorUI::Seconds      ("KillSlowDuration", m_specialMove.m_hitParam.m_killSlowDuration);
			InspectorUI::MeterPerFrame("MoveSpeed",        m_specialMove.m_moveSpeed);
			InspectorUI::Frame		  ("HitCooldown",      m_specialMove.m_hitCooldownDuration);
			InspectorUI::Meter		  ("HitRadius",        m_specialMove.m_hitRadius);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Guard"))
		{
			InspectorUI::Seconds("GuardHitResetTime",         m_guard.m_guardHitResetTime);
			InspectorUI::Count  ("GuardBreakCount",           m_guard.m_guardBreakCount);
			InspectorUI::Rate   ("GuardKnockBackRate",        m_guard.m_guardKnockBackRate);

			if (InspectorUI::BeginGroup("Parry"))
			{
				InspectorUI::Seconds("GuardParryWindow",          m_parry.m_parryWindow);
				InspectorUI::Seconds("GuardParryHitStop",         m_parry.m_parryHitStop);
				InspectorUI::Scale  ("GuardParrySlowScale",       m_parry.m_parrySlowScale);
				InspectorUI::Seconds("GuardParrySlowDuration",    m_parry.m_parrySlowDuration);
				InspectorUI::Power  ("GuardParryKnockBackPower",  m_parry.m_parryKnockBackPower);
				InspectorUI::Meter  ("GuardParryKnockBackRadius", m_parry.m_parryKnockBackRadius);
				
				InspectorUI::EndGroup();
			}

			InspectorUI::EndGroup();
		}

	}
}

void PlayerParameter::SaveToJson()
{
	nlohmann::json paramJson;


	//--------------------------------------------------------
	//  キャラの基本パラメータ
	//--------------------------------------------------------
	paramJson["MaxHP"]                 = m_body.m_maxHP;
	paramJson["TurnSpeed"]             = m_body.m_turnSpeed;
	paramJson["GravityAcceleration"]   = m_body.m_gravityAcceleration;
	paramJson["BumpPushRate"]          = m_body.m_bumpPushRate;
	paramJson["MaxWalkableSlopeAngle"] = m_body.m_maxWalkableSlopeAngle;
	paramJson["StepHeight"]            = m_body.m_stepHeight;
	paramJson["GroundRayLength"]       = m_body.m_groundRayLength;

	paramJson["MoveSpeed"]             = m_move.m_moveSpeed;

	paramJson["JumpPower"]             = m_jump.m_jumpPow;

	//--------------------------------------------------------
	//  攻撃パラメータ
	//--------------------------------------------------------
	for (int i = 0; i < kComboCount; i++)
	{
		HitParam& hit = m_attack.m_hitParam[i];

		std::string attackNum = "Attack" + std::to_string(i + 1);
		std::string paramName = attackNum + "Power";
	
		paramJson[paramName] = hit.m_attackPower;

		paramName = attackNum + "KnockBack";
		paramJson[paramName] = hit.m_knockBackPower;

		paramName = attackNum + "HitStop";
		paramJson[paramName] = hit.m_hitStop;

		paramName = attackNum + "KillHitStop";
		paramJson[paramName] = hit.m_killHitStop;

		paramName = attackNum + "KillSlowScale";
		paramJson[paramName] = hit.m_killSlowScale;

		paramName = attackNum + "KillSlowDuration";
		paramJson[paramName] = hit.m_killSlowDuration;

	}

	paramJson["AttackMoveSpeed"]         = m_attack.m_attackMoveSpeed;
	paramJson["AttackHitRadius"]         = m_attack.m_hitRadius;
	paramJson["AttackHitForwardOffset"]  = m_attack.m_hitForwardOffset;

	//--------------------------------------------------------
	//  必殺技パラメータ
	//--------------------------------------------------------

	paramJson["SpecialAttackPower"]      = m_specialMove.m_hitParam.m_attackPower;
	paramJson["SpecialKnockBackPower"]   = m_specialMove.m_hitParam.m_knockBackPower;
	paramJson["SpecialHitStop"]          = m_specialMove.m_hitParam.m_hitStop;
	paramJson["SpecialKillHitStop"]      = m_specialMove.m_hitParam.m_killHitStop;
	paramJson["SpecialKillSlowScale"]    = m_specialMove.m_hitParam.m_killSlowScale;
	paramJson["SpecialKillSlowDuration"] = m_specialMove.m_hitParam.m_killSlowDuration;
	paramJson["SpecialMoveSpeed"]        = m_specialMove.m_moveSpeed;
	paramJson["SpecialHitCooldown"]      = m_specialMove.m_hitCooldownDuration;
	paramJson["SpecialHitRadius"]        = m_specialMove.m_hitRadius;


	//--------------------------------------------------------
	//  ガードパラメータ
	//--------------------------------------------------------
	paramJson["GuardHitResetTime"]         = m_guard.m_guardHitResetTime;
	paramJson["GuardBreakCount"]           = m_guard.m_guardBreakCount;
	paramJson["GuardKnockBackRate"]        = m_guard.m_guardKnockBackRate;


	//--------------------------------------------------------
	//  パリィパラメータ
	//--------------------------------------------------------
	paramJson["ParryWindow"]          = m_parry.m_parryWindow;
	paramJson["ParryHitStop"]         = m_parry.m_parryHitStop;
	paramJson["ParrySlowScale"]       = m_parry.m_parrySlowScale;
	paramJson["ParrySlowDuration"]    = m_parry.m_parrySlowDuration;
	paramJson["ParryKnockBackPower"]  = m_parry.m_parryKnockBackPower;
	paramJson["ParryKnockBackRadius"] = m_parry.m_parryKnockBackRadius;



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

		//--------------------------------------------------------
		//  キャラの基本パラメータ
		//--------------------------------------------------------
		ParameterJson::Read(paramJson, "MaxHP",                 m_body.m_maxHP);
		ParameterJson::Read(paramJson, "TurnSpeed",             m_body.m_turnSpeed);
		ParameterJson::Read(paramJson, "GravityAcceleration",   m_body.m_gravityAcceleration);
		ParameterJson::Read(paramJson, "BumpPushRate",          m_body.m_bumpPushRate);
		ParameterJson::Read(paramJson, "MaxWalkableSlopeAngle", m_body.m_maxWalkableSlopeAngle);
		ParameterJson::Read(paramJson, "StepHeight",            m_body.m_stepHeight);
		ParameterJson::Read(paramJson, "GroundRayLength",       m_body.m_groundRayLength);

		ParameterJson::Read(paramJson, "MoveSpeed",             m_move.m_moveSpeed);

		ParameterJson::Read(paramJson, "JumpPower",             m_jump.m_jumpPow);


		//--------------------------------------------------------
		//  攻撃パラメータ
		//--------------------------------------------------------
		for (int i = 0; i < kComboCount; i++)
		{
			HitParam& hit = m_attack.m_hitParam[i];

			std::string attackNum = "Attack" + std::to_string(i + 1);
			std::string paramName = attackNum + "Power";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_attackPower);

			paramName = attackNum + "KnockBack";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_knockBackPower);

			paramName = attackNum + "HitStop";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_hitStop);

			paramName = attackNum + "KillHitStop";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_killHitStop);

			paramName = attackNum + "KillSlowScale";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_killSlowScale);

			paramName = attackNum + "KillSlowDuration";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_killSlowDuration);
		}

		ParameterJson::Read(paramJson, "AttackMoveSpeed",         m_attack.m_attackMoveSpeed);
		ParameterJson::Read(paramJson, "AttackHitRadius",         m_attack.m_hitRadius);
		ParameterJson::Read(paramJson, "AttackHitForwardOffset",  m_attack.m_hitForwardOffset);


		//--------------------------------------------------------
		//  必殺技パラメータ
		//--------------------------------------------------------
		ParameterJson::Read(paramJson, "SpecialAttackPower",      m_specialMove.m_hitParam.m_attackPower);
		ParameterJson::Read(paramJson, "SpecialKnockBackPower",   m_specialMove.m_hitParam.m_knockBackPower);
		ParameterJson::Read(paramJson, "SpecialHitStop",          m_specialMove.m_hitParam.m_hitStop);
		ParameterJson::Read(paramJson, "SpecialKillHitStop",      m_specialMove.m_hitParam.m_killHitStop);
		ParameterJson::Read(paramJson, "SpecialKillSlowScale",    m_specialMove.m_hitParam.m_killSlowScale);
		ParameterJson::Read(paramJson, "SpecialKillSlowDuration", m_specialMove.m_hitParam.m_killSlowDuration);
		ParameterJson::Read(paramJson, "SpecialMoveSpeed",        m_specialMove.m_moveSpeed);
		ParameterJson::Read(paramJson, "SpecialHitCooldown",      m_specialMove.m_hitCooldownDuration);
		ParameterJson::Read(paramJson, "SpecialHitRadius",        m_specialMove.m_hitRadius);


		//--------------------------------------------------------
		//  ガードパラメータ
		//--------------------------------------------------------
		ParameterJson::Read(paramJson, "GuardHitResetTime",         m_guard.m_guardHitResetTime);
		ParameterJson::Read(paramJson, "GuardBreakCount",           m_guard.m_guardBreakCount);
		ParameterJson::Read(paramJson, "GuardKnockBackRate",        m_guard.m_guardKnockBackRate);


		//--------------------------------------------------------
		//  ガードパラメータ
		//--------------------------------------------------------
		ParameterJson::Read(paramJson, "ParryWindow",          m_parry.m_parryWindow);
		ParameterJson::Read(paramJson, "ParryHitStop",         m_parry.m_parryHitStop);
		ParameterJson::Read(paramJson, "ParrySlowScale",       m_parry.m_parrySlowScale);
		ParameterJson::Read(paramJson, "ParrySlowDuration",    m_parry.m_parrySlowDuration);
		ParameterJson::Read(paramJson, "ParryKnockBackPower",  m_parry.m_parryKnockBackPower);
		ParameterJson::Read(paramJson, "ParryKnockBackRadius", m_parry.m_parryKnockBackRadius);

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
