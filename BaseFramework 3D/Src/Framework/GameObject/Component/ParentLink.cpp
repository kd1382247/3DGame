#include "ParentLink.h"

void ParentLink::DrawInspector()
{
	ImGui::Text("%s",m_parentName.c_str());
}

void ParentLink::Update(KdGameObject& obj)
{
	auto spParent = GetParent();

	if (!spParent)
	{
		return;
	}

	// 位置と大きさだけを親に追従させる
	obj.SetPos(spParent->GetPos() + m_localPos * spParent->GetScale());
	obj.SetScale(m_localScale * spParent->GetScale());

}

void ParentLink::SetParent(const std::shared_ptr<KdGameObject>& parent)
{
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

