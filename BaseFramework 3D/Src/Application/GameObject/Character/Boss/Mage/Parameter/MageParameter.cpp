#include "MageParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// Parameter → json
	nlohmann::json ToJson(const MageParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"] = param.m_maxHP;
		json["Scale"] = param.m_scale;
		json["MoveSpeed"] = param.m_moveSpeed;
		json["TurnSpeed"] = param.m_turnSpeed;
		json["ReachDistance"] = param.m_reachDistance;
		json["ReachDistanceMargin"] = param.m_reachDistanceMargin;
		json["JumpPower"] = param.m_jumpPow;
		json["AttackPower"] = param.m_attackPow;
		json["AttackCooldown"] = param.m_attackCooldown;
		json["TargetCircleCastDelay"] = param.m_targetCircleCastDelay;
		json["TargetCircleRadius"] = param.m_targetCircleRadius;
		json["TargetCircleTelegraph"] = param.m_targetCircleTelegraph;
		json["TargetCircleShotCount"] = param.m_targetCircleShotCount;
		json["ForwardSectorCastDelay"] = param.m_forwardSectorCastDelay;
		json["ForwardSectorAngle"] = param.m_forwardSectorAngle;
		json["ForwardSectorRadius"] = param.m_forwardSectorRadius;
		json["ForwardSectorTelegraph"] = param.m_forwardSectorTelegraph;
		json["ForwardSectorDamage"] = param.m_forwardSectorDamage;
		json["BoltCastDelay"] = param.m_boltCastDelay;
		json["BoltShotCount"] = param.m_boltShotCount;
		json["BoltSpeed"] = param.m_boltSpeed;
		json["BoltRadius"] = param.m_boltRadius;
		json["BoltKnockBack"] = param.m_boltKnockBack;
		json["BoltLifeTime"] = param.m_boltLifeTime;
		json["BoltSpawnHeight"] = param.m_boltSpawnHeight;
		json["BoltAimHeight"] = param.m_boltAimHeight;
		json["NovaCircleCastDelay"] = param.m_novaCircleCastDelay;
		json["NovaCircleRadius"] = param.m_novaCircleRadius;
		json["NovaCircleTelegraph"] = param.m_novaCircleTelegraph;
		json["BeamCastDelay"] = param.m_beamCastDelay;
		json["BeamLength"] = param.m_beamLength;
		json["BeamWidth"] = param.m_beamWidth;
		json["BeamForwardOffset"] = param.m_beamForwardOffset;
		json["BeamDuration"] = param.m_beamDuration;
		json["SummonCastDelay"] = param.m_summonCastDelay;
		json["SummonDelay"] = param.m_summonDelay;
		json["SummonCount"] = param.m_summonCount;
		json["SummonRadius"] = param.m_summonRadius;

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, MageParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "Scale", param.m_scale);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "ReachDistanceMargin", param.m_reachDistanceMargin);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackPower", param.m_attackPow);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);
		ParameterJson::Read(json, "TargetCircleCastDelay", param.m_targetCircleCastDelay);
		ParameterJson::Read(json, "TargetCircleRadius", param.m_targetCircleRadius);
		ParameterJson::Read(json, "TargetCircleTelegraph", param.m_targetCircleTelegraph);
		ParameterJson::Read(json, "TargetCircleShotCount", param.m_targetCircleShotCount);
		ParameterJson::Read(json, "ForwardSectorCastDelay", param.m_forwardSectorCastDelay);
		ParameterJson::Read(json, "ForwardSectorAngle", param.m_forwardSectorAngle);
		ParameterJson::Read(json, "ForwardSectorRadius", param.m_forwardSectorRadius);
		ParameterJson::Read(json, "ForwardSectorTelegraph", param.m_forwardSectorTelegraph);
		ParameterJson::Read(json, "ForwardSectorDamage", param.m_forwardSectorDamage);
		ParameterJson::Read(json, "BoltCastDelay", param.m_boltCastDelay);
		ParameterJson::Read(json, "BoltShotCount", param.m_boltShotCount);
		ParameterJson::Read(json, "BoltSpeed", param.m_boltSpeed);
		ParameterJson::Read(json, "BoltRadius", param.m_boltRadius);
		ParameterJson::Read(json, "BoltKnockBack", param.m_boltKnockBack);
		ParameterJson::Read(json, "BoltLifeTime", param.m_boltLifeTime);
		ParameterJson::Read(json, "BoltSpawnHeight", param.m_boltSpawnHeight);
		ParameterJson::Read(json, "BoltAimHeight", param.m_boltAimHeight);
		ParameterJson::Read(json, "NovaCircleCastDelay", param.m_novaCircleCastDelay);
		ParameterJson::Read(json, "NovaCircleRadius", param.m_novaCircleRadius);
		ParameterJson::Read(json, "NovaCircleTelegraph", param.m_novaCircleTelegraph);
		ParameterJson::Read(json, "BeamCastDelay", param.m_beamCastDelay);
		ParameterJson::Read(json, "BeamLength", param.m_beamLength);
		ParameterJson::Read(json, "BeamWidth", param.m_beamWidth);
		ParameterJson::Read(json, "BeamForwardOffset", param.m_beamForwardOffset);
		ParameterJson::Read(json, "BeamDuration", param.m_beamDuration);
		ParameterJson::Read(json, "SummonCastDelay", param.m_summonCastDelay);
		ParameterJson::Read(json, "SummonDelay", param.m_summonDelay);
		ParameterJson::Read(json, "SummonCount", param.m_summonCount);
		ParameterJson::Read(json, "SummonRadius", param.m_summonRadius);
	}
}

void MageParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(MageParameter::Parameter& param)
	{
		if (InspectorUI::BeginGroup("Status"))
		{
			InspectorUI::HP("MaxHP", param.m_maxHP);
			InspectorUI::Scale("Scale", param.m_scale);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Move"))
		{
			InspectorUI::MeterPerFrame("MoveSpeed", param.m_moveSpeed);
			InspectorUI::DegreePerFrame("TurnSpeed", param.m_turnSpeed);
			InspectorUI::Meter("ReachDistance", param.m_reachDistance);
			InspectorUI::Meter("ReachDistanceMargin", param.m_reachDistanceMargin);
			InspectorUI::Power("JumpPower", param.m_jumpPow);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack"))
		{
			InspectorUI::Damage("AttackPower", param.m_attackPow);
			InspectorUI::Seconds("AttackCooldown", param.m_attackCooldown);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("TargetCircle"))
		{
			InspectorUI::Seconds("CastDelay", param.m_targetCircleCastDelay);
			InspectorUI::Meter("Radius", param.m_targetCircleRadius);
			InspectorUI::Seconds("Telegraph", param.m_targetCircleTelegraph);
			InspectorUI::Count("ShotCount", param.m_targetCircleShotCount);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("ForwardSector"))
		{
			InspectorUI::Seconds("CastDelay", param.m_forwardSectorCastDelay);
			InspectorUI::Degree("Angle", param.m_forwardSectorAngle);
			InspectorUI::Meter("Radius", param.m_forwardSectorRadius);
			InspectorUI::Seconds("Telegraph", param.m_forwardSectorTelegraph);
			InspectorUI::Damage("Damage", param.m_forwardSectorDamage);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Bolt"))
		{
			InspectorUI::Seconds("CastDelay", param.m_boltCastDelay);
			InspectorUI::Count("ShotCount", param.m_boltShotCount);
			InspectorUI::MeterPerFrame("Speed", param.m_boltSpeed);
			InspectorUI::Meter("Radius", param.m_boltRadius);
			InspectorUI::Power("KnockBack", param.m_boltKnockBack);
			InspectorUI::Seconds("LifeTime", param.m_boltLifeTime);
			InspectorUI::Meter("SpawnHeight", param.m_boltSpawnHeight);
			InspectorUI::Meter("AimHeight", param.m_boltAimHeight);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("NovaCircle"))
		{
			InspectorUI::Seconds("CastDelay", param.m_novaCircleCastDelay);
			InspectorUI::Meter("Radius", param.m_novaCircleRadius);
			InspectorUI::Seconds("Telegraph", param.m_novaCircleTelegraph);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Beam"))
		{
			InspectorUI::Seconds("CastDelay", param.m_beamCastDelay);
			InspectorUI::Meter("Length", param.m_beamLength);
			InspectorUI::Meter("Width", param.m_beamWidth);
			InspectorUI::Meter("ForwardOffset", param.m_beamForwardOffset);
			InspectorUI::Seconds("Duration", param.m_beamDuration);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Summon"))
		{
			InspectorUI::Seconds("CastDelay", param.m_summonCastDelay);
			InspectorUI::Seconds("SpawnDelay", param.m_summonDelay);
			InspectorUI::Count("Count", param.m_summonCount);
			InspectorUI::Meter("Radius", param.m_summonRadius);

			InspectorUI::EndGroup();
		}
	}
}

void MageParameter::DrawInspecter()
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

		DrawParameter_(m_param);
	}
}

void MageParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/Mage/Parameter/MageParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Mage parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Mage parameter save filed\n");

		return;
	}
}

void MageParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Mage/Parameter/MageParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("MageのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("MageのParameter.jsonを開けませんでした\n");
		return;
	}

	nlohmann::json paramJson;

	try
	{
		file >> paramJson;

		FromJson(paramJson, m_param);
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
