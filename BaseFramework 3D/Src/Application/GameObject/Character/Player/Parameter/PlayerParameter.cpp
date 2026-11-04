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
			InspectorUI::Meter         ("BumpSphereRadius",  m_body.m_bumpSphereRadius);
			InspectorUI::Meter         ("BumpSphereOffsetY", m_body.m_bumpSphereOffsetY);

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
					InspectorUI::Scale  ("SlowScale",        hit.m_SlowScale);
					InspectorUI::Seconds("SlowDuration",     hit.m_SlowDuration);

					InspectorUI::EndGroup();
				}
			}

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("SpinAttack"))
		{
			InspectorUI::Damage		  ("AttackPower",      m_spinAttack.m_hitParam.m_attackPower);
			InspectorUI::Power		  ("KnockBackPower",   m_spinAttack.m_hitParam.m_knockBackPower);
			InspectorUI::Seconds	  ("HitStop",          m_spinAttack.m_hitParam.m_hitStop);
			InspectorUI::Seconds	  ("KillHitStop",      m_spinAttack.m_hitParam.m_killHitStop);
			InspectorUI::Scale        ("SlowScale",        m_spinAttack.m_hitParam.m_SlowScale);
			InspectorUI::Seconds      ("SlowDuration",     m_spinAttack.m_hitParam.m_SlowDuration);
			InspectorUI::MeterPerFrame("MoveSpeed",        m_spinAttack.m_moveSpeed);
			InspectorUI::Frame		  ("HitCooldown",      m_spinAttack.m_hitCooldownDuration);
			InspectorUI::Meter		  ("HitRadius",        m_spinAttack.m_hitRadius);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Guard"))
		{
			InspectorUI::Seconds("HitResetTime",         m_guard.m_guardHitResetTime);
			InspectorUI::Count  ("BreakCount",           m_guard.m_guardBreakCount);
			InspectorUI::Rate   ("KnockBackRate",        m_guard.m_guardKnockBackRate);
			InspectorUI::Degree ("GuardAngle",           m_guard.m_guardAngle);

			if (InspectorUI::BeginGroup("Parry"))
			{
				InspectorUI::Seconds("ParryWindow",     m_parry.m_parryWindow);
				InspectorUI::Seconds("HitStop",         m_parry.m_parryHitStop);
				InspectorUI::Scale  ("SlowScale",       m_parry.m_parrySlowScale);
				InspectorUI::Seconds("SlowDuration",    m_parry.m_parrySlowDuration);
				InspectorUI::Power  ("KnockBackPower",  m_parry.m_parryKnockBackPower);
				InspectorUI::Meter  ("KnockBackRadius", m_parry.m_parryKnockBackRadius);
				
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
	paramJson["TurnSpeed"]             = InspectorUI::CeilToDigits(m_body.m_turnSpeed, 1);
	paramJson["GravityAcceleration"]   = InspectorUI::CeilToDigits(m_body.m_gravityAcceleration, 1);
	paramJson["BumpPushRate"]          = InspectorUI::CeilToDigits(m_body.m_bumpPushRate, 2);
	paramJson["MaxWalkableSlopeAngle"] = InspectorUI::CeilToDigits(m_body.m_maxWalkableSlopeAngle, 1);
	paramJson["StepHeight"]            = InspectorUI::CeilToDigits(m_body.m_stepHeight, 2);
	paramJson["GroundRayLength"]       = InspectorUI::CeilToDigits(m_body.m_groundRayLength, 2);
	paramJson["BumpSphereRadius"]      = InspectorUI::CeilToDigits(m_body.m_bumpSphereRadius, 2);
	paramJson["BumpSphereOffsetY"]     = InspectorUI::CeilToDigits(m_body.m_bumpSphereOffsetY, 2);

	paramJson["MoveSpeed"]             = InspectorUI::CeilToDigits(m_move.m_moveSpeed, 3);

	paramJson["JumpPower"]             = InspectorUI::CeilToDigits(m_jump.m_jumpPow, 3);

	//--------------------------------------------------------
	//  攻撃パラメータ
	//--------------------------------------------------------
	for (int i = 0; i < kComboCount; i++)
	{
		HitParam& hit = m_attack.m_hitParam[i];

		std::string attackNum = "Attack" + std::to_string(i + 1);
		std::string paramName = attackNum + "Power";
	
		paramJson[paramName] = InspectorUI::CeilToDigits(hit.m_attackPower, 1);

		paramName = attackNum + "KnockBack";
		paramJson[paramName] = InspectorUI::CeilToDigits(hit.m_knockBackPower, 3);

		paramName = attackNum + "HitStop";
		paramJson[paramName] = InspectorUI::CeilToDigits(hit.m_hitStop, 2);

		paramName = attackNum + "KillHitStop";
		paramJson[paramName] = InspectorUI::CeilToDigits(hit.m_killHitStop, 2);

		paramName = attackNum + "SlowScale";
		paramJson[paramName] = InspectorUI::CeilToDigits(hit.m_SlowScale, 2);

		paramName = attackNum + "SlowDuration";
		paramJson[paramName] = InspectorUI::CeilToDigits(hit.m_SlowDuration, 2);

	}

	paramJson["AttackMoveSpeed"]         = InspectorUI::CeilToDigits(m_attack.m_attackMoveSpeed, 3);
	paramJson["AttackHitRadius"]         = InspectorUI::CeilToDigits(m_attack.m_hitRadius, 2);
	paramJson["AttackHitForwardOffset"]  = InspectorUI::CeilToDigits(m_attack.m_hitForwardOffset, 2);

	//--------------------------------------------------------
	//  回転切パラメータ
	//--------------------------------------------------------

	paramJson["SpinAttackPower"]            = InspectorUI::CeilToDigits(m_spinAttack.m_hitParam.m_attackPower, 1);
	paramJson["SpinAttackKnockBackPower"]   = InspectorUI::CeilToDigits(m_spinAttack.m_hitParam.m_knockBackPower, 3);
	paramJson["SpinAttackHitStop"]          = InspectorUI::CeilToDigits(m_spinAttack.m_hitParam.m_hitStop, 2);
	paramJson["SpinAttackKillHitStop"]      = InspectorUI::CeilToDigits(m_spinAttack.m_hitParam.m_killHitStop, 2);
	paramJson["SpinAttackSlowScale"]        = InspectorUI::CeilToDigits(m_spinAttack.m_hitParam.m_SlowScale, 2);
	paramJson["SpinAttackSlowDuration"]     = InspectorUI::CeilToDigits(m_spinAttack.m_hitParam.m_SlowDuration, 2);
	paramJson["SpinAttackMoveSpeed"]        = InspectorUI::CeilToDigits(m_spinAttack.m_moveSpeed, 3);
	paramJson["SpinAttackHitCooldown"]      = InspectorUI::CeilToDigits(m_spinAttack.m_hitCooldownDuration, 1);
	paramJson["SpinAttackHitRadius"]        = InspectorUI::CeilToDigits(m_spinAttack.m_hitRadius, 2);


	//--------------------------------------------------------
	//  ガードパラメータ
	//--------------------------------------------------------
	paramJson["GuardHitResetTime"]         = InspectorUI::CeilToDigits(m_guard.m_guardHitResetTime, 2);
	paramJson["GuardBreakCount"]           = m_guard.m_guardBreakCount;
	paramJson["GuardKnockBackRate"]        = InspectorUI::CeilToDigits(m_guard.m_guardKnockBackRate, 2);
	paramJson["GuardAngle"]                = InspectorUI::CeilToDigits(m_guard.m_guardAngle, 1);

	//--------------------------------------------------------
	//  パリィパラメータ
	//--------------------------------------------------------
	paramJson["ParryWindow"]          = InspectorUI::CeilToDigits(m_parry.m_parryWindow, 2);
	paramJson["ParryHitStop"]         = InspectorUI::CeilToDigits(m_parry.m_parryHitStop, 2);
	paramJson["ParrySlowScale"]       = InspectorUI::CeilToDigits(m_parry.m_parrySlowScale, 2);
	paramJson["ParrySlowDuration"]    = InspectorUI::CeilToDigits(m_parry.m_parrySlowDuration, 2);
	paramJson["ParryKnockBackPower"]  = InspectorUI::CeilToDigits(m_parry.m_parryKnockBackPower, 3);
	paramJson["ParryKnockBackRadius"] = InspectorUI::CeilToDigits(m_parry.m_parryKnockBackRadius, 2);



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
		ParameterJson::Read(paramJson, "BumpSphereRadius",      m_body.m_bumpSphereRadius);
		ParameterJson::Read(paramJson, "BumpSphereOffsetY",     m_body.m_bumpSphereOffsetY);

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

			paramName = attackNum + "SlowScale";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_SlowScale);

			paramName = attackNum + "SlowDuration";
			ParameterJson::Read(paramJson, paramName.c_str(), hit.m_SlowDuration);
		}

		ParameterJson::Read(paramJson, "AttackMoveSpeed",         m_attack.m_attackMoveSpeed);
		ParameterJson::Read(paramJson, "AttackHitRadius",         m_attack.m_hitRadius);
		ParameterJson::Read(paramJson, "AttackHitForwardOffset",  m_attack.m_hitForwardOffset);


		//--------------------------------------------------------
		//  必殺技パラメータ
		//--------------------------------------------------------
		ParameterJson::Read(paramJson, "SpinAttackPower",            m_spinAttack.m_hitParam.m_attackPower);
		ParameterJson::Read(paramJson, "SpinAttackKnockBackPower",   m_spinAttack.m_hitParam.m_knockBackPower);
		ParameterJson::Read(paramJson, "SpinAttackHitStop",          m_spinAttack.m_hitParam.m_hitStop);
		ParameterJson::Read(paramJson, "SpinAttackKillHitStop",      m_spinAttack.m_hitParam.m_killHitStop);
		ParameterJson::Read(paramJson, "SpinAttackSlowScale",        m_spinAttack.m_hitParam.m_SlowScale);
		ParameterJson::Read(paramJson, "SpinAttackSlowDuration",     m_spinAttack.m_hitParam.m_SlowDuration);
		ParameterJson::Read(paramJson, "SpinAttackMoveSpeed",        m_spinAttack.m_moveSpeed);
		ParameterJson::Read(paramJson, "SpinAttackHitCooldown",      m_spinAttack.m_hitCooldownDuration);
		ParameterJson::Read(paramJson, "SpinAttackHitRadius",        m_spinAttack.m_hitRadius);


		//--------------------------------------------------------
		//  ガードパラメータ
		//--------------------------------------------------------
		ParameterJson::Read(paramJson, "GuardHitResetTime",         m_guard.m_guardHitResetTime);
		ParameterJson::Read(paramJson, "GuardBreakCount",           m_guard.m_guardBreakCount);
		ParameterJson::Read(paramJson, "GuardKnockBackRate",        m_guard.m_guardKnockBackRate);
		ParameterJson::Read(paramJson, "GuardAngle",                m_guard.m_guardAngle);

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
