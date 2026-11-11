#pragma once

#include"json.hpp"

class KdGameObject;

class Component
{
public:

	Component() = default;
	virtual ~Component() = default;

	virtual void Update( KdGameObject& obj)           {}

	virtual void SaveData(nlohmann::json& json)const  {}
	virtual void LoadData(const nlohmann::json& json) {}

	virtual void DrawInspector(KdGameObject& obj)     {}
	
};