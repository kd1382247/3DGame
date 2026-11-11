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

	// ローカル位置を編集する
	DrawInspectorLocalPos(obj);

	if(m_followScale)
	{
		// ローカルの大きさを編集する
		DrawInspectorLocalScale(obj);
	}

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
	Math::Vector3 scale = GetLocalScale();

	// 大きさ変更(親からのローカル大きさ)
	if (ImGui::DragFloat3("LocalScale", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
		Apply(obj);

		EditorManager::Instance().MarkDirty();
	}
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

	// 親の座標と大きさ
	Math::Vector3 parentPos = spParent->GetPos();
	Math::Vector3 parentScale = spParent->GetScale();

	// 持ち主の座標と大きさ
	Math::Vector3 pos = obj.GetPos();
	Math::Vector3 scale = obj.GetScale();

	// ローカル座標 (ワールド位置 - 親の位置) / 親の大きさ
	Math::Vector3 localPos = {};

	if (parentScale.x != 0.0f) localPos.x = (pos.x - parentPos.x) / parentScale.x;
	if (parentScale.y != 0.0f) localPos.y = (pos.y - parentPos.y) / parentScale.y;
	if (parentScale.z != 0.0f) localPos.z = (pos.z - parentPos.z) / parentScale.z;

	SetLocalPos(localPos);

	if(m_followScale)
	{
		// ローカルの大きさ 持ち主の大きさ / 親の大きさ
		Math::Vector3 localScale = Math::Vector3::One;

		if (parentScale.x != 0.0f) localScale.x = scale.x / parentScale.x;
		if (parentScale.y != 0.0f) localScale.y = scale.y / parentScale.y;
		if (parentScale.z != 0.0f) localScale.z = scale.z / parentScale.z;


		SetLocalScale(localScale);
	}

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

	// 位置と大きさだけを親に追従させる
	obj.SetPos(spParent->GetPos() + m_localPos * spParent->GetScale());

	if(m_followScale)
	{
		obj.SetScale(m_localScale * spParent->GetScale());
	}

	return true;

}

