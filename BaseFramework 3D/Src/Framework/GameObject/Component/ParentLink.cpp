#include "ParentLink.h"

#include"../../../Application/Editor/EditorManager.h"

void ParentLink::DrawInspector(KdGameObject& obj)
{
	// 親がいなければ表示しない
	if (!HasParent())
	{
		return;
	}

	// 親の名前を表示する
	DrawInspectorParentName();

	DrawInspectorLocalPos     (obj);
	DrawInspectorLocalScale   (obj);
	DrawInspectorLocalRotation(obj);
}

void ParentLink::Update(KdGameObject& obj)
{
	if (m_followMode == FollowMode::PlaceOnce)
	{
		if (!m_isPlaced)
		{
			// 一度だけ適用する
			m_isPlaced = Apply(obj);
		}
		return;
	}
	
	// 常に適用する
	Apply(obj);
}

void ParentLink::SetParent(const std::shared_ptr<KdGameObject>& parent)
{

	m_isCalcLocalWorld = false;
	m_isPlaced = false;

	if (!parent)
	{
		m_wpParent.reset();
		m_parentName.clear();
		return;
	}

	m_wpParent = parent;
	m_parentName = parent->GetObjectName();

}

std::shared_ptr<KdGameObject> ParentLink::GetParent() const
{
	
	auto spParent = m_wpParent.lock();


	if (spParent)
	{
		return spParent;
	}

	// 名前が空だったらnullptrを返す
	if (m_parentName.empty())
	{
		return nullptr;
	}

	// 名前から親を探す変数が空だったら
	if (!m_findParentName)
	{
		return nullptr;
	}

	// 親がいなかったら
	spParent = m_findParentName(m_parentName);

	if(spParent)
	{
		m_wpParent = spParent;
	}
	
	return spParent;

}

void ParentLink::SetParentKeepWorld(const std::shared_ptr<KdGameObject>& parent, const KdGameObject& obj)
{
	SetParent(parent);
	CalcLocalFromWorld(obj);
}

void ParentLink::SetParentNameKeepWorld(const std::string& parentName)
{
	m_parentName = parentName;

	m_wpParent.reset();

	// 逆算がまだなのでtrue
	m_isCalcLocalWorld = true;

	// 置いていない
	m_isPlaced = false;
}

bool ParentLink::IsOddQuarter(const KdGameObject& parent) const
{
	// 奇数の場合trueを返す
	if (parent.GetYawQuarterCount() % 2 != 0)
	{
		return true;
	}

	return false;
}

void ParentLink::DrawInspectorParentName()
{
	ImGui::Text("ParentName   : %s", m_parentName.c_str());
}

void ParentLink::DrawInspectorLocalPos(KdGameObject& obj)
{
	// 座標変更(親からのローカル位置)
	Math::Vector3 pos = GetLocalPos();

	if (ImGui::DragFloat3("LocalPos", &pos.x, 0.1f))
	{
		SetLocalPos(pos);
		Apply(obj);

		EditorManager::Instance().MarkDirty();
	}

}

void ParentLink::DrawInspectorLocalScale(KdGameObject& obj)
{
	// 大きさを追従しない場合表示しない
	if (!m_followScale)
	{
		return;
	}

	Math::Vector3 scale = GetLocalScale();

	// 大きさ変更(親からのローカル大きさ)
	if (ImGui::DragFloat3("LocalScale", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
		Apply(obj);

		EditorManager::Instance().MarkDirty();
	}
}

void ParentLink::DrawInspectorLocalRotation(KdGameObject& obj)
{
	// 回転を追従しない場合表示しない
	if (!m_followRotation)
	{
		return;
	}

	Math::Vector3 rotation = GetLocalRotation();

	if (ImGui::DragFloat3("LocalRotation", &rotation.x))
	{
		SetLocalRotation(rotation);
		Apply(obj);

		EditorManager::Instance().MarkDirty();
	}
	
}

void ParentLink::FollowWorldPos(const KdGameObject& parent,KdGameObject& obj)
{
	// 位置と大きさだけを親に追従させる
	obj.SetPos(Math::Vector3::Transform(m_localPos, parent.GetMatrix()));
}

void ParentLink::FollowWorldScale(const KdGameObject& parent,KdGameObject& obj)
{
	if (!m_followScale)
	{
		return;
	}

	Math::Vector3 worldScale = m_localScale * parent.GetScale();

	if (IsOddQuarter(parent) && m_swapScaleOnOddQuarter)
	{
		// 大きさを入れ替える
		std::swap(worldScale.x, worldScale.z);
	}

	obj.SetScale(worldScale);

}

void ParentLink::FollowWorldRotation(const KdGameObject& parent,KdGameObject& obj)
{
	if (m_followRotation)
	{
		obj.SetRotation({
			m_localRotation.x,
			m_localRotation.y + parent.GetRotation().y,
			m_localRotation.z });
	}

}

void ParentLink::FollowLocalPos(const KdGameObject& parent, Math::Vector3 pos)
{
	// ローカル座標
	Math::Vector3 localPos = {};

	Math::Matrix invParent = parent.GetMatrix().Invert();

	localPos = Math::Vector3::Transform(pos, invParent);

	SetLocalPos(localPos);
}

void ParentLink::FollowLocalScale(const KdGameObject& parent, Math::Vector3 scale)
{
	if (!m_followScale)
	{
		return;
	}

	// 親の大きさ
	Math::Vector3 parentScale = parent.GetScale();

	// ローカルの大きさ 持ち主の大きさ / 親の大きさ
	Math::Vector3 localScale = Math::Vector3::One;

	if (m_swapScaleOnOddQuarter && IsOddQuarter(parent))
	{
		// 大きさを入れ替える
		std::swap(scale.x, scale.z);
	}

	if (parentScale.x != 0.0f) localScale.x = scale.x / parentScale.x;
	if (parentScale.y != 0.0f) localScale.y = scale.y / parentScale.y;
	if (parentScale.z != 0.0f) localScale.z = scale.z / parentScale.z;


	SetLocalScale(localScale);
}

void ParentLink::FollowLocalRotation(const KdGameObject& parent, Math::Vector3 rotation)
{
	if (!m_followRotation)
	{
		return;
	}
	Math::Vector3 localRotation = rotation;

	localRotation.y -= parent.GetRotation().y;

	SetLocalRotation(localRotation);
}

void ParentLink::SaveData(nlohmann::json& json) const
{
	// 親の名前を保存
	json["ParentName"] = m_parentName;

	// ローカル座標を保存
	const Math::Vector3& pos = GetLocalPos();

	json["LocalPos"] =
	{
		{ "x",pos.x },
		{ "y",pos.y },
		{ "z",pos.z }
	};

	// ローカルの大きさを保存
	const Math::Vector3& scale = GetLocalScale();
	json["LocalScale"] =
	{
		{ "x",scale.x },
		{ "y",scale.y },
		{ "z",scale.z }
	};

	// ローカルの回転を保存
	const Math::Vector3& rotation = GetLocalRotation();
	json["LocalRotation"] =
	{
		{ "x",rotation.x },
		{ "y",rotation.y },
		{ "z",rotation.z }
	};
}


void ParentLink::LoadData(const nlohmann::json & json)
{
	// 親の名前をセット
	if (json.contains("ParentName"))
	{
		m_parentName = json["ParentName"].get<std::string>();
		m_wpParent.reset();
		m_isCalcLocalWorld = false;
		m_isPlaced = false;
	}
	else
	{
		if (json.contains("FollowStage"))
		{
			SetParentNameKeepWorld(json["FollowStage"].get<std::string>());
		}
	}

	// ローカル座標をセット
	if (json.contains("LocalPos"))
	{		
		Math::Vector3 localPos = {};

		localPos.x = json["LocalPos"]["x"].get<float>();
		localPos.y = json["LocalPos"]["y"].get<float>();
		localPos.z = json["LocalPos"]["z"].get<float>();

		SetLocalPos(localPos);
	}
	
	// ローカルの大きさをセット
	if (json.contains("LocalScale"))
	{
		Math::Vector3 localScale = {};

		localScale.x = json["LocalScale"]["x"].get<float>();
		localScale.y = json["LocalScale"]["y"].get<float>();
		localScale.z = json["LocalScale"]["z"].get<float>();

		SetLocalScale(localScale);
	}

	// ローカルの回転をセット
	if (json.contains("LocalRotation"))
	{
		Math::Vector3 localRotation = {};

		localRotation.x = json["LocalRotation"]["x"].get<float>();
		localRotation.y = json["LocalRotation"]["y"].get<float>();
		localRotation.z = json["LocalRotation"]["z"].get<float>();

		SetLocalRotation(localRotation);	
	}
}

void ParentLink::SetParentResolver(const std::function<ParentResolver>& findFunk)
{
	m_findParentName = findFunk;
}

void ParentLink::CalcLocalFromWorld(const KdGameObject& obj)
{
	auto spParent = GetParent();
	if (!spParent)
	{
		return;
	}

	FollowLocalPos     (*spParent, obj.GetPos());
	FollowLocalScale   (*spParent, obj.GetScale());
	FollowLocalRotation(*spParent, obj.GetRotation());

	m_isCalcLocalWorld = false;
}

bool ParentLink::Apply(KdGameObject& obj)
{
	auto spParent = GetParent();

	if (!spParent)
	{
		return false; 
	}

	if (m_isCalcLocalWorld)
	{
		CalcLocalFromWorld(obj);
	}

	
	FollowWorldPos     (*spParent, obj);
	FollowWorldScale   (*spParent, obj);
	FollowWorldRotation(*spParent, obj);
	

	return true;

}

