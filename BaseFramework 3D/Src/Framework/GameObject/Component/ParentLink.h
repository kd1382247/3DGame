#pragma once

#include"Component.h"

class ParentLink:public Component
{
public:

	
	void DrawInspector()override;

	void Update(KdGameObject& obj)override;

	// 親を決める
	void SetParent(const std::shared_ptr<KdGameObject>& parent);

	// 親を返す
	std::shared_ptr<KdGameObject>GetParent()const;


	const Math::Vector3& GetLocalPos()const        { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos)     { m_localPos = pos; }

	const Math::Vector3& GetLocalScale()const      { return m_localScale; }
	void SetLocalScale(const Math::Vector3& scale) { m_localScale = scale; }


	void SaveData(nlohmann::json& json)const override;
	void LoadData(const nlohmann::json& json) override;


	using ParentResolver = std::shared_ptr<KdGameObject>(const std::string&);
	static void SetParentResolver(const std::function<ParentResolver>& findFunk);


private:


	static inline std::function<ParentResolver> m_findParentName;

	
	// 親の参照
	mutable std::weak_ptr<KdGameObject>m_wpParent;

	// 親の名前
	std::string                m_parentName = {};

	// ローカルの位置と大きさ
	Math::Vector3              m_localPos   = {};
	Math::Vector3              m_localScale = Math::Vector3::One;

};