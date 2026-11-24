#pragma once

#include"json.hpp"

class KdGameObject;

class ComponentBase
{
public:

	ComponentBase() = default;
	virtual ~ComponentBase() = default;

	virtual void PreUpdate(KdGameObject& obj)         {}
	virtual void Update( KdGameObject& obj)           {}
	virtual void PostUpdate( KdGameObject& obj)       {}

	virtual void SaveData(nlohmann::json& json)const  {}
	virtual void LoadData(const nlohmann::json& json) {}

	virtual void DrawInspector(KdGameObject& obj)     {}
	
};