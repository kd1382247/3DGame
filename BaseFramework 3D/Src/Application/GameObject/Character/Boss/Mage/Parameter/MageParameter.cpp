#include "MageParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// エフェクト設定 → json(キーは「prefix + 項目名」。ファイル名は対象外)
	void WriteEffect(nlohmann::json& json, const std::string& prefix, const MageParameter::EffectParam& effect)
	{
		json[prefix + "Size"] = effect.m_size;
		json[prefix + "Speed"] = effect.m_speed;
		json[prefix + "StartFrame"] = effect.m_startFrame;
		json[prefix + "EndFrame"] = effect.m_endFrame;
		json[prefix + "RotationY"] = effect.m_rotationY;
		json[prefix + "ForwardOffset"] = effect.m_forwardOffset;
		json[prefix + "HeightOffset"] = effect.m_heightOffset;
	}

	// json → エフェクト設定(キーが無い項目は、デフォルト値のまま)
	void ReadEffect(const nlohmann::json& json, const std::string& prefix, MageParameter::EffectParam& effect)
	{
		ParameterJson::Read(json, (prefix + "Size").c_str(), effect.m_size);
		ParameterJson::Read(json, (prefix + "Speed").c_str(), effect.m_speed);
		ParameterJson::Read(json, (prefix + "StartFrame").c_str(), effect.m_startFrame);
		ParameterJson::Read(json, (prefix + "EndFrame").c_str(), effect.m_endFrame);
		ParameterJson::Read(json, (prefix + "RotationY").c_str(), effect.m_rotationY);
		ParameterJson::Read(json, (prefix + "ForwardOffset").c_str(), effect.m_forwardOffset);
		ParameterJson::Read(json, (prefix + "HeightOffset").c_str(), effect.m_heightOffset);
	}

	// Parameter → json
	nlohmann::json ToJson(const MageParameter::Parameter& param)
	{
		nlohmann::json json;

		//---- Status / Move / Attack(共通) ----
		json["MaxHP"] = param.m_maxHP;
		json["Scale"] = param.m_scale;
		json["MoveSpeed"] = param.m_moveSpeed;
		json["TurnSpeed"] = param.m_turnSpeed;
		json["ReachDistance"] = param.m_reachDistance;
		json["ReachDistanceMargin"] = param.m_reachDistanceMargin;
		json["JumpPower"] = param.m_jumpPow;
		json["AttackCooldown"] = param.m_attackCooldown;

		//---- TargetCircle ----
		const auto& targetCircle = param.m_targetCircle;
		json["TargetCircleCastDelay"] = targetCircle.m_castDelay;
		json["TargetCircleRadius"] = targetCircle.m_radius;
		json["TargetCircleTelegraph"] = targetCircle.m_telegraph;
		json["TargetCircleShotCount"] = targetCircle.m_shotCount;
		json["TargetCircleDamage"] = targetCircle.m_damage;
		WriteEffect(json, "TargetCircleEffect", targetCircle.m_effect);

		//---- ForwardSector ----
		const auto& forwardSector = param.m_forwardSector;
		json["ForwardSectorCastDelay"] = forwardSector.m_castDelay;
		json["ForwardSectorAngle"] = forwardSector.m_angle;
		json["ForwardSectorRadius"] = forwardSector.m_radius;
		json["ForwardSectorTelegraph"] = forwardSector.m_telegraph;
		json["ForwardSectorDamage"] = forwardSector.m_damage;
		WriteEffect(json, "ForwardSectorEffect", forwardSector.m_effect);

		//---- Bolt ----
		const auto& bolt = param.m_bolt;
		json["BoltCastDelay"] = bolt.m_castDelay;
		json["BoltShotCount"] = bolt.m_shotCount;
		json["BoltSpeed"] = bolt.m_speed;
		json["BoltRadius"] = bolt.m_radius;
		json["BoltDamage"] = bolt.m_damage;
		json["BoltKnockBack"] = bolt.m_knockBack;
		json["BoltLifeTime"] = bolt.m_lifeTime;
		json["BoltSpawnHeight"] = bolt.m_spawnHeight;
		json["BoltAimHeight"] = bolt.m_aimHeight;

		//---- NovaCircle ----
		const auto& novaCircle = param.m_novaCircle;
		json["NovaCircleCastDelay"] = novaCircle.m_castDelay;
		json["NovaCircleRadius"] = novaCircle.m_radius;
		json["NovaCircleTelegraph"] = novaCircle.m_telegraph;
		json["NovaCircleDamage"] = novaCircle.m_damage;
		WriteEffect(json, "NovaCircleEffect", novaCircle.m_effect);

		//---- Beam ----
		const auto& beam = param.m_beam;
		json["BeamCastDelay"] = beam.m_castDelay;
		json["BeamLength"] = beam.m_length;
		json["BeamWidth"] = beam.m_width;
		json["BeamForwardOffset"] = beam.m_forwardOffset;
		json["BeamDuration"] = beam.m_duration;
		json["BeamDamage"] = beam.m_damage;
		json["BeamHitInterval"] = beam.m_hitInterval;
		WriteEffect(json, "BeamWindupEffect", beam.m_windupEffect);
		WriteEffect(json, "BeamEffect", beam.m_beamEffect);

		//---- Summon ----
		const auto& summon = param.m_summon;
		json["SummonCastDelay"] = summon.m_castDelay;
		json["SummonDelay"] = summon.m_delay;
		json["SummonCount"] = summon.m_count;
		json["SummonRadius"] = summon.m_radius;

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, MageParameter::Parameter& param)
	{
		//---- Status / Move / Attack(共通) ----
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "Scale", param.m_scale);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "ReachDistanceMargin", param.m_reachDistanceMargin);
		ParameterJson::Read(json, "JumpPower", param.m_jumpPow);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);

		// 古いjsonには、全攻撃共通のダメージ(AttackPower)しか無い。
		// その場合は、この値を各攻撃のダメージの初期値として引き継ぐ(攻撃ごとのキーがあれば、後で上書きされる)
		if (json.is_object() && json.contains("AttackPower"))
		{
			const float legacyAttackPow = json["AttackPower"].get<float>();

			param.m_targetCircle.m_damage = legacyAttackPow;
			param.m_bolt.m_damage = legacyAttackPow;
			param.m_novaCircle.m_damage = legacyAttackPow;
			param.m_beam.m_damage = legacyAttackPow;
		}

		//---- TargetCircle ----
		auto& targetCircle = param.m_targetCircle;
		ParameterJson::Read(json, "TargetCircleCastDelay", targetCircle.m_castDelay);
		ParameterJson::Read(json, "TargetCircleRadius", targetCircle.m_radius);
		ParameterJson::Read(json, "TargetCircleTelegraph", targetCircle.m_telegraph);
		ParameterJson::Read(json, "TargetCircleShotCount", targetCircle.m_shotCount);
		ParameterJson::Read(json, "TargetCircleDamage", targetCircle.m_damage);
		ReadEffect(json, "TargetCircleEffect", targetCircle.m_effect);

		//---- ForwardSector ----
		auto& forwardSector = param.m_forwardSector;
		ParameterJson::Read(json, "ForwardSectorCastDelay", forwardSector.m_castDelay);
		ParameterJson::Read(json, "ForwardSectorAngle", forwardSector.m_angle);
		ParameterJson::Read(json, "ForwardSectorRadius", forwardSector.m_radius);
		ParameterJson::Read(json, "ForwardSectorTelegraph", forwardSector.m_telegraph);
		ParameterJson::Read(json, "ForwardSectorDamage", forwardSector.m_damage);
		ReadEffect(json, "ForwardSectorEffect", forwardSector.m_effect);

		//---- Bolt ----
		auto& bolt = param.m_bolt;
		ParameterJson::Read(json, "BoltCastDelay", bolt.m_castDelay);
		ParameterJson::Read(json, "BoltShotCount", bolt.m_shotCount);
		ParameterJson::Read(json, "BoltSpeed", bolt.m_speed);
		ParameterJson::Read(json, "BoltRadius", bolt.m_radius);
		ParameterJson::Read(json, "BoltDamage", bolt.m_damage);
		ParameterJson::Read(json, "BoltKnockBack", bolt.m_knockBack);
		ParameterJson::Read(json, "BoltLifeTime", bolt.m_lifeTime);
		ParameterJson::Read(json, "BoltSpawnHeight", bolt.m_spawnHeight);
		ParameterJson::Read(json, "BoltAimHeight", bolt.m_aimHeight);

		//---- NovaCircle ----
		auto& novaCircle = param.m_novaCircle;
		ParameterJson::Read(json, "NovaCircleCastDelay", novaCircle.m_castDelay);
		ParameterJson::Read(json, "NovaCircleRadius", novaCircle.m_radius);
		ParameterJson::Read(json, "NovaCircleTelegraph", novaCircle.m_telegraph);
		ParameterJson::Read(json, "NovaCircleDamage", novaCircle.m_damage);
		ReadEffect(json, "NovaCircleEffect", novaCircle.m_effect);

		//---- Beam ----
		auto& beam = param.m_beam;
		ParameterJson::Read(json, "BeamCastDelay", beam.m_castDelay);
		ParameterJson::Read(json, "BeamLength", beam.m_length);
		ParameterJson::Read(json, "BeamWidth", beam.m_width);
		ParameterJson::Read(json, "BeamForwardOffset", beam.m_forwardOffset);
		ParameterJson::Read(json, "BeamDuration", beam.m_duration);
		ParameterJson::Read(json, "BeamDamage", beam.m_damage);
		ParameterJson::Read(json, "BeamHitInterval", beam.m_hitInterval);
		ReadEffect(json, "BeamWindupEffect", beam.m_windupEffect);
		ReadEffect(json, "BeamEffect", beam.m_beamEffect);

		//---- Summon ----
		auto& summon = param.m_summon;
		ParameterJson::Read(json, "SummonCastDelay", summon.m_castDelay);
		ParameterJson::Read(json, "SummonDelay", summon.m_delay);
		ParameterJson::Read(json, "SummonCount", summon.m_count);
		ParameterJson::Read(json, "SummonRadius", summon.m_radius);
	}
}

void MageParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// エフェクト1つ分を描画する(攻撃のグループの中に入れ子で表示する)
	// showRotation: 向きの補正を表示するか / showOffset: 位置のずらしを表示するか
	void DrawEffect_(const char* label, MageParameter::EffectParam& effect, bool showRotation, bool showOffset)
	{
		if (InspectorUI::BeginGroup(label))
		{
			InspectorUI::Scale("Size", effect.m_size);
			InspectorUI::Scale("Speed", effect.m_speed);
			ImGui::DragInt("StartFrame [F]", &effect.m_startFrame, 1.0f, 0, 10000);
			ImGui::DragInt("EndFrame [F]", &effect.m_endFrame, 1.0f, 0, 10000);

			if (showRotation)
			{
				InspectorUI::Degree("RotationY", effect.m_rotationY);
			}

			if (showOffset)
			{
				InspectorUI::Meter("ForwardOffset", effect.m_forwardOffset);
				InspectorUI::Meter("HeightOffset", effect.m_heightOffset);
			}

			InspectorUI::EndGroup();
		}
	}

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
			InspectorUI::Seconds("AttackCooldown", param.m_attackCooldown);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("TargetCircle"))
		{
			auto& targetCircle = param.m_targetCircle;

			InspectorUI::Seconds("CastDelay", targetCircle.m_castDelay);
			InspectorUI::Meter("Radius", targetCircle.m_radius);
			InspectorUI::Seconds("Telegraph", targetCircle.m_telegraph);
			InspectorUI::Count("ShotCount", targetCircle.m_shotCount);
			InspectorUI::Damage("Damage", targetCircle.m_damage);
			DrawEffect_("Effect", targetCircle.m_effect, false, false);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("ForwardSector"))
		{
			auto& forwardSector = param.m_forwardSector;

			InspectorUI::Seconds("CastDelay", forwardSector.m_castDelay);
			InspectorUI::Degree("Angle", forwardSector.m_angle);
			InspectorUI::Meter("Radius", forwardSector.m_radius);
			InspectorUI::Seconds("Telegraph", forwardSector.m_telegraph);
			InspectorUI::Damage("Damage", forwardSector.m_damage);
			DrawEffect_("Effect", forwardSector.m_effect, true, false);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Bolt"))
		{
			auto& bolt = param.m_bolt;

			InspectorUI::Seconds("CastDelay", bolt.m_castDelay);
			InspectorUI::Count("ShotCount", bolt.m_shotCount);
			InspectorUI::MeterPerFrame("Speed", bolt.m_speed);
			InspectorUI::Meter("Radius", bolt.m_radius);
			InspectorUI::Damage("Damage", bolt.m_damage);
			InspectorUI::Power("KnockBack", bolt.m_knockBack);
			InspectorUI::Seconds("LifeTime", bolt.m_lifeTime);
			InspectorUI::Meter("SpawnHeight", bolt.m_spawnHeight);
			InspectorUI::Meter("AimHeight", bolt.m_aimHeight);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("NovaCircle"))
		{
			auto& novaCircle = param.m_novaCircle;

			InspectorUI::Seconds("CastDelay", novaCircle.m_castDelay);
			InspectorUI::Meter("Radius", novaCircle.m_radius);
			InspectorUI::Seconds("Telegraph", novaCircle.m_telegraph);
			InspectorUI::Damage("Damage", novaCircle.m_damage);
			DrawEffect_("Effect", novaCircle.m_effect, false, false);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Beam"))
		{
			auto& beam = param.m_beam;

			InspectorUI::Seconds("CastDelay", beam.m_castDelay);
			InspectorUI::Meter("Length", beam.m_length);
			InspectorUI::Meter("Width", beam.m_width);
			InspectorUI::Meter("ForwardOffset", beam.m_forwardOffset);
			InspectorUI::Seconds("Duration", beam.m_duration);
			InspectorUI::Damage("Damage", beam.m_damage);
			InspectorUI::Seconds("HitInterval", beam.m_hitInterval);
			DrawEffect_("WindupEffect", beam.m_windupEffect, true, true);
			DrawEffect_("BeamEffect", beam.m_beamEffect, true, true);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Summon"))
		{
			auto& summon = param.m_summon;

			InspectorUI::Seconds("CastDelay", summon.m_castDelay);
			InspectorUI::Seconds("SpawnDelay", summon.m_delay);
			InspectorUI::Count("Count", summon.m_count);
			InspectorUI::Meter("Radius", summon.m_radius);

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
