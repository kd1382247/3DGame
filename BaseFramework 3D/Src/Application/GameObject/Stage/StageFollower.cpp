#include "StageFollower.h"

#include"StageObject.h"

#include"../../Editor/EditorManager.h"
#include"../../Scene/SceneManager.h"

void StageFollower::Update(KdGameObject& target)
{
	if (m_stageName.empty())
	{
		return;
	}

	auto spStage = m_wpStage.lock();

	// 見失っていたら、名前でステージを探し直す
	if (!spStage)
	{
		auto stage = FindStageByName();

		if (stage)
		{
			// 見つけた最初のフレームは、基準を覚えるだけで動かさない
			Link(stage);
			return;
		}

		// 見つからないときは、ログを1回だけ出す
		if (!m_warnedMissingStage)
		{
			KdDebugGUI::Instance().AddLog("StageFollower : stage not found : %s\n", m_stageName.c_str());

			m_warnedMissingStage = true;
		}

		return;
	}

	const Math::Vector3 stagePos = spStage->GetPos();
	const Math::Vector3 stageScale = spStage->GetScale();

	// 動いていなければ、何もしない
	if (stagePos == m_lastStagePos && stageScale == m_lastStageScale)
	{
		return;
	}

	// ステージの大きさが変わった割合(0で割らないようにする)
	Math::Vector3 ratio = Math::Vector3::One;

	if (m_lastStageScale.x != 0.0f) { ratio.x = stageScale.x / m_lastStageScale.x; }
	if (m_lastStageScale.y != 0.0f) { ratio.y = stageScale.y / m_lastStageScale.y; }
	if (m_lastStageScale.z != 0.0f) { ratio.z = stageScale.z / m_lastStageScale.z; }

	// 前のステージの位置から見た自分の位置(差)を、大きさの割合で伸び縮みさせて、新しいステージの位置に足す
	const Math::Vector3 offset = target.GetPos() - m_lastStagePos;

	target.SetPos(stagePos + offset * ratio);

	// 大きさが変わったときだけ、自分の大きさも同じ割合で変える
	if (ratio != Math::Vector3::One)
	{
		target.SetScale(target.GetScale() * ratio);
	}

	m_lastStagePos = stagePos;
	m_lastStageScale = stageScale;
}

void StageFollower::DrawInspector()
{
	// 持ち主のステージは、Hierarchyで選んでいるステージ(Active Stage)で作ったときに決まる
	ImGui::Text("Stage : %s", m_stageName.empty() ? "(None)" : m_stageName.c_str());
}

void StageFollower::SetStage(const std::shared_ptr<StageObject>& stage)
{
	if (!stage)
	{
		m_stageName.clear();
		m_wpStage.reset();
		m_warnedMissingStage = false;

		return;
	}

	m_stageName = stage->GetObjectName();

	Link(stage);
}

std::shared_ptr<StageObject> StageFollower::GetStage()
{
	if (m_stageName.empty())
	{
		return nullptr;
	}

	auto spStage = m_wpStage.lock();

	if (spStage)
	{
		return spStage;
	}

	// 見失っていたら、名前で探し直す
	spStage = FindStageByName();

	if (spStage)
	{
		Link(spStage);
	}

	return spStage;
}

void StageFollower::SaveData(nlohmann::json& json) const
{
	json["FollowStage"] = m_stageName;
}

void StageFollower::LoadData(const nlohmann::json& json)
{
	if (json.contains("FollowStage"))
	{
		m_stageName = json["FollowStage"].get<std::string>();
	}
}

void StageFollower::Link(const std::shared_ptr<StageObject>& stage)
{
	m_wpStage = stage;

	m_lastStagePos = stage->GetPos();
	m_lastStageScale = stage->GetScale();

	m_warnedMissingStage = false;
}

std::shared_ptr<StageObject> StageFollower::FindStageByName() const
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto stage = std::dynamic_pointer_cast<StageObject>(obj);

		if (stage && stage->GetObjectName() == m_stageName)
		{
			return stage;
		}
	}

	return nullptr;
}
