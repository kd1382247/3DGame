#pragma once

#include"../Hierarchy/HierarchyCategory.h"

class StageObject;
class StageOwnedObject;

struct StageOwnedTypeInfo
{
	// 引数なし　戻り値はStageOwnedObjectリストを返す
	std::function<std::vector<std::shared_ptr<StageOwnedObject>>()>getObjects;

	// 引数はステージ(対応したステージに作るため)
	// 戻り値は作られたオブジェクト
	std::function<std::shared_ptr<StageOwnedObject>(const std::shared_ptr<StageObject>& stageObj)>createObject;

	// デバッグ表示をしているのか
	std::function<bool()>isDebug;

	// デバッグ状態をセット
	std::function<void(bool flg)>setDebug;

	// オブジェクトのタイプ
	std::function<bool(const std::shared_ptr<StageOwnedObject>& ownedObj)>isType;
};


 const StageOwnedTypeInfo* FindStageOwnedTypeInfo(HierarchyCategory category);

 const std::map<HierarchyCategory, StageOwnedTypeInfo>& GetStageOwnedTypeInfoTable();