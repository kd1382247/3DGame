#include "BeholderParameter.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../Editor/InspectorWidgets.h"
#include"../../../ParameterJson.h"

#include"json.hpp"
#include<fstream>


namespace
{
	// エフェクト設定 → json(キーは「prefix + 項目名」。ファイル名は対象外)
	void WriteEffect(nlohmann::json& json, const std::string& prefix, const BeholderParameter::EffectParam& effect)
	{
		json[prefix + "Size"] = InspectorUI::CeilToDigits(effect.m_size, 2);
		json[prefix + "Speed"] = InspectorUI::CeilToDigits(effect.m_speed, 2);
		json[prefix + "StartFrame"] = effect.m_startFrame;
		json[prefix + "EndFrame"] = effect.m_endFrame;
	}

	// json → エフェクト設定(キーが無い項目は、デフォルト値のまま)
	void ReadEffect(const nlohmann::json& json, const std::string& prefix, BeholderParameter::EffectParam& effect)
	{
		ParameterJson::Read(json, (prefix + "Size").c_str(), effect.m_size);
		ParameterJson::Read(json, (prefix + "Speed").c_str(), effect.m_speed);
		ParameterJson::Read(json, (prefix + "StartFrame").c_str(), effect.m_startFrame);
		ParameterJson::Read(json, (prefix + "EndFrame").c_str(), effect.m_endFrame);
	}

	// Parameter → json
	nlohmann::json ToJson(const BeholderParameter::Parameter& param)
	{
		nlohmann::json json;

		json["MaxHP"]               = param.m_maxHP;
		json["BumpSphereRadius"]    = InspectorUI::CeilToDigits(param.m_bumpSphereRadius, 2);
		json["BumpSphereOffsetY"]   = InspectorUI::CeilToDigits(param.m_bumpSphereOffsetY, 2);
		json["MoveSpeed"]           = InspectorUI::CeilToDigits(param.m_moveSpeed, 3);
		json["TurnSpeed"]           = InspectorUI::CeilToDigits(param.m_turnSpeed, 1);
		json["ReachDistance"]       = InspectorUI::CeilToDigits(param.m_reachDistance, 2);
		json["ReachDistanceMargin"] = InspectorUI::CeilToDigits(param.m_reachDistanceMargin, 2);
		json["GravityAcceleration"]           = InspectorUI::CeilToDigits(param.m_gravityAcceleration, 1);
		json["AttackCooldown"]      = InspectorUI::CeilToDigits(param.m_attackCooldown, 2);

		//---- SparkBullet ----
		const auto& spark = param.m_sparkBullet;
		json["SparkBulletVanishT"]              = InspectorUI::CeilToDigits(spark.m_vanishT, 2);
		json["SparkBulletCurveRate"]            = InspectorUI::CeilToDigits(spark.m_curveRate, 2);
		json["SparkBulletTrackingStopDistance"] = InspectorUI::CeilToDigits(spark.m_trackingStopDistance, 2);
		json["SparkBulletSpeed"]                = InspectorUI::CeilToDigits(spark.m_speed, 3);
		json["SparkBulletRadius"]               = InspectorUI::CeilToDigits(spark.m_radius, 2);
		json["SparkBulletReflectSpeed"]         = InspectorUI::CeilToDigits(spark.m_reflectSpeed, 3);
		json["SparkBulletDamage"]               = InspectorUI::CeilToDigits(spark.m_damage, 1);
		json["SparkBulletKnockBack"]            = InspectorUI::CeilToDigits(spark.m_knockBack, 3);
		WriteEffect(json, "SparkBulletEffect", spark.m_effect);

		//---- ChargeSparkBullet ----
		const auto& charge = param.m_chargeSparkBullet;
		json["ChargeSparkBulletChargeTime"]   = InspectorUI::CeilToDigits(charge.m_chargeTime, 2);
		json["ChargeSparkBulletRadius"]       = InspectorUI::CeilToDigits(charge.m_radius, 2);
		json["ChargeSparkBulletTargetSize"]   = InspectorUI::CeilToDigits(charge.m_targetSize, 2);
		json["ChargeSparkBulletSpeed"]        = InspectorUI::CeilToDigits(charge.m_speed, 3);
		json["ChargeSparkBulletReflectSpeed"] = InspectorUI::CeilToDigits(charge.m_reflectSpeed, 3);
		json["ChargeSparkBulletLifeTime"]     = InspectorUI::CeilToDigits(charge.m_lifeTime, 2);
		json["ChargeSparkBulletDamage"]       = InspectorUI::CeilToDigits(charge.m_damage, 1);
		json["ChargeSparkBulletKnockBack"]    = InspectorUI::CeilToDigits(charge.m_knockBack, 3);
		WriteEffect(json, "ChargeSparkBulletEffect", charge.m_effect);

		return json;
	}

	// json → Parameter(キーが無い項目は、デフォルト値のまま)
	void FromJson(const nlohmann::json& json, BeholderParameter::Parameter& param)
	{
		ParameterJson::Read(json, "MaxHP", param.m_maxHP);
		ParameterJson::Read(json, "BumpSphereRadius", param.m_bumpSphereRadius);
		ParameterJson::Read(json, "BumpSphereOffsetY", param.m_bumpSphereOffsetY);
		ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
		ParameterJson::Read(json, "TurnSpeed", param.m_turnSpeed);
		ParameterJson::Read(json, "ReachDistance", param.m_reachDistance);
		ParameterJson::Read(json, "ReachDistanceMargin", param.m_reachDistanceMargin);
		ParameterJson::Read(json, "GravityAcceleration", param.m_gravityAcceleration);
		ParameterJson::Read(json, "AttackCooldown", param.m_attackCooldown);

		// 古いjsonには、全攻撃共通のダメージ(AttackPower)しか無い。
		// その場合は、この値を弾のダメージの初期値として引き継ぐ(SparkBulletDamageのキーがあれば、後で上書きされる)
		if (json.is_object() && json.contains("AttackPower"))
		{
			param.m_sparkBullet.m_damage = json["AttackPower"].get<float>();
		}

		//---- SparkBullet ----
		auto& spark = param.m_sparkBullet;
		ParameterJson::Read(json, "SparkBulletVanishT", spark.m_vanishT);
		ParameterJson::Read(json, "SparkBulletCurveRate", spark.m_curveRate);
		ParameterJson::Read(json, "SparkBulletTrackingStopDistance", spark.m_trackingStopDistance);
		ParameterJson::Read(json, "SparkBulletSpeed", spark.m_speed);
		ParameterJson::Read(json, "SparkBulletRadius", spark.m_radius);
		ParameterJson::Read(json, "SparkBulletReflectSpeed", spark.m_reflectSpeed);
		ParameterJson::Read(json, "SparkBulletDamage", spark.m_damage);
		ParameterJson::Read(json, "SparkBulletKnockBack", spark.m_knockBack);
		ReadEffect(json, "SparkBulletEffect", spark.m_effect);

		//---- ChargeSparkBullet ----
		auto& charge = param.m_chargeSparkBullet;
		ParameterJson::Read(json, "ChargeSparkBulletChargeTime", charge.m_chargeTime);
		ParameterJson::Read(json, "ChargeSparkBulletRadius", charge.m_radius);
		ParameterJson::Read(json, "ChargeSparkBulletTargetSize", charge.m_targetSize);
		ParameterJson::Read(json, "ChargeSparkBulletSpeed", charge.m_speed);
		ParameterJson::Read(json, "ChargeSparkBulletReflectSpeed", charge.m_reflectSpeed);
		ParameterJson::Read(json, "ChargeSparkBulletLifeTime", charge.m_lifeTime);
		ParameterJson::Read(json, "ChargeSparkBulletDamage", charge.m_damage);
		ParameterJson::Read(json, "ChargeSparkBulletKnockBack", charge.m_knockBack);
		ReadEffect(json, "ChargeSparkBulletEffect", charge.m_effect);
	}
}

void BeholderParameter::Init()
{
	LoadFromJson();
}

namespace
{
	// エフェクト1つ分を描画する(攻撃のグループの中に入れ子で表示する)
	void DrawEffect_(const char* label, BeholderParameter::EffectParam& effect)
	{
		if (InspectorUI::BeginGroup(label))
		{
			InspectorUI::Scale("Size", effect.m_size);
			InspectorUI::Scale("Speed", effect.m_speed);
			ImGui::DragInt("StartFrame [F]", &effect.m_startFrame, 1.0f, 0, 10000);
			ImGui::DragInt("EndFrame [F]", &effect.m_endFrame, 1.0f, 0, 10000);

			InspectorUI::EndGroup();
		}
	}

	// 1つ分のパラメータをグループごとに描画する
	void DrawParameter_(BeholderParameter::Parameter& param)
	{
		if (InspectorUI::BeginGroup("Status"))
		{
			InspectorUI::HP("MaxHP", param.m_maxHP);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Collision"))
		{
			InspectorUI::Meter("BumpSphereRadius", param.m_bumpSphereRadius);
			InspectorUI::Meter("BumpSphereOffsetY", param.m_bumpSphereOffsetY);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Move"))
		{
			InspectorUI::MeterPerFrame("MoveSpeed", param.m_moveSpeed);
			InspectorUI::DegreePerFrame("TurnSpeed", param.m_turnSpeed);
			InspectorUI::Meter("ReachDistance", param.m_reachDistance);
			InspectorUI::Meter("ReachDistanceMargin", param.m_reachDistanceMargin);
			InspectorUI::Acceleration("GravityAccel", param.m_gravityAcceleration);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("Attack"))
		{
			InspectorUI::Seconds("AttackCooldown", param.m_attackCooldown);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("SparkBullet"))
		{
			auto& spark = param.m_sparkBullet;

			ImGui::DragFloat("VanishT", &spark.m_vanishT, 0.01f, 0.1f, 5.0f);
			InspectorUI::Scale("CurveRate", spark.m_curveRate);
			InspectorUI::Meter("TrackingStopDistance", spark.m_trackingStopDistance);
			InspectorUI::MeterPerFrame("Speed", spark.m_speed);
			InspectorUI::Meter("Radius", spark.m_radius);
			InspectorUI::MeterPerFrame("ReflectSpeed", spark.m_reflectSpeed);
			InspectorUI::Damage("Damage", spark.m_damage);
			InspectorUI::Power("KnockBack", spark.m_knockBack);
			DrawEffect_("Effect", spark.m_effect);

			InspectorUI::EndGroup();
		}

		if (InspectorUI::BeginGroup("ChargeSparkBullet"))
		{
			auto& charge = param.m_chargeSparkBullet;

			ImGui::PushID("ChargeSparkBullet");

			InspectorUI::Seconds("ChargeTime", charge.m_chargeTime);
			InspectorUI::Meter("Radius", charge.m_radius);
			InspectorUI::Scale("TargetSize", charge.m_targetSize);
			InspectorUI::MeterPerFrame("Speed", charge.m_speed);
			InspectorUI::MeterPerFrame("ReflectSpeed", charge.m_reflectSpeed);
			InspectorUI::Seconds("LifeTime", charge.m_lifeTime);
			InspectorUI::Damage("Damage", charge.m_damage);
			InspectorUI::Power("KnockBack", charge.m_knockBack);
			DrawEffect_("Effect", charge.m_effect);

			ImGui::PopID();

			InspectorUI::EndGroup();
		}
	}
}

void BeholderParameter::DrawInspecter()
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

void BeholderParameter::SaveToJson()
{
	nlohmann::json paramJson = ToJson(m_param);

	std::ofstream file("Asset/Data/Enemy/Beholder/Parameter/BeholderParameter.json");

	if (file.is_open())
	{
		file << paramJson.dump(4);
	}
	else
	{
		OutputDebugStringA("Beholder parameter save filed\n");
		KdDebugGUI::Instance().AddErrorLog("Beholder parameter save filed\n");

		return;
	}
}

void BeholderParameter::LoadFromJson()
{

	std::fstream file("Asset/Data/Enemy/Beholder/Parameter/BeholderParameter.json");

	// もしファイルを開けないとき(デフォルト値のまま動く)
	if (!file.is_open())
	{
		OutputDebugStringA("BeholderのParameter.jsonを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("BeholderのParameter.jsonを開けませんでした\n");
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
