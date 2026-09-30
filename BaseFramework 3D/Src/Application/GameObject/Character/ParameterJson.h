#pragma once

#include "json.hpp"

//==================================================================
// Parameterクラスのjson読み込み用の小さな部品
//
// 新しく追加した項目は、古いjson(まだその項目が無いもの)にも対応できるよう、
// キーが存在する時だけ読み込む(無ければ、構造体に書いてあるデフォルト値のまま)
//
//   ParameterJson::Read(json, "MoveSpeed", param.m_moveSpeed);
//==================================================================
namespace ParameterJson
{
	template<class T>
	inline void Read(const nlohmann::json& json, const char* key, T& value)
	{
		if (json.is_object() && json.contains(key))
		{
			value = json[key].get<T>();
		}
	}
}
