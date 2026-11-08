
#include"StageOwnedTypeInfo.h"

#include"../../GameObject/Stage/StageOwnedObject/StageOwnedObject.h"

#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include"../../GameObject/Stage/Collision/AABBCollision/AABBCollision.h"

#include"../../GameObject/Stage/Collision/AIBlockCollision/AIBlockManager.h"
#include"../../GameObject/Stage/Collision/AIBlockCollision/AIBlock.h"

#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"
#include"../../GameObject/Stage/Collision/OBBCollision/OBBCollision.h"

#include"../../GameObject/Stage/PointLight/PointLightManager.h"
#include"../../GameObject/Stage/PointLight/PointLight.h"

#include"../../GameObject/Stage/WayPoint/WayPointManager.h"
#include"../../GameObject/Stage/WayPoint/WayPoint.h"


namespace
{

	// 表を作って返す
	std::map<HierarchyCategory, StageOwnedTypeInfo>BuildStageOwnedTypeTable()
	{

		std::map<HierarchyCategory, StageOwnedTypeInfo> table;

		// ===================AABB===================//
		{
			StageOwnedTypeInfo info;

			// AABB作成
			info.createObject = [](const std::shared_ptr<StageObject>& stageObj)
				{
					return AABBCollisionManager::Instance().CreateAABBCollision(stageObj);
				};

			// リストを作成
			info.getObjects = []()
				{
					std::vector<std::shared_ptr<StageOwnedObject>>list;

					for (const auto& aabb : AABBCollisionManager::Instance().GetAABBCollisionList())
					{
						if (!aabb || aabb->IsExpired())
						{
							continue;
						}

						list.push_back(aabb);
					}

					return list;
				};

			// デバッグをしているか
			info.isDebug = []()
				{
					return AABBCollisionManager::Instance().IsDebug();
				};

			// デバッグ状態をセット
			info.setDebug = [](bool flg)
				{
					AABBCollisionManager::Instance().SetIsDebug(flg);
				};

			// 引数がAABBなのかを調べる
			info.isType = [](const std::shared_ptr<StageOwnedObject>& ownedObj)
				{
					return std::dynamic_pointer_cast<AABBCollision>(ownedObj) != nullptr;
				};

			table[HierarchyCategory::AABB] = info;

		}

		// ===================OBB===================//
		{
			StageOwnedTypeInfo info;

			// OBB作成
			info.createObject = [](const std::shared_ptr<StageObject>& stageObj)
				{
					return OBBCollisionManager::Instance().CreateOBBCollision(stageObj);
				};

			// リストを作成
			info.getObjects = []()
				{
					std::vector<std::shared_ptr<StageOwnedObject>>list;

					for (const auto& obb : OBBCollisionManager::Instance().GetOBBCollisionList())
					{
						if (!obb || obb->IsExpired())
						{
							continue;
						}

						list.push_back(obb);
					}

					return list;
				};

			// デバッグをしているか
			info.isDebug = []()
				{
					return OBBCollisionManager::Instance().IsDebug();
				};

			// デバッグ状態をセット
			info.setDebug = [](bool flg) {OBBCollisionManager::Instance().SetIsDebug(flg); };

			// 引数がOBBなのかを調べる
			info.isType = [](const std::shared_ptr<StageOwnedObject>& ownedObj)
				{
					return std::dynamic_pointer_cast<OBBCollision>(ownedObj) != nullptr;
				};

			table[HierarchyCategory::OBB] = info;

		}

		// ===================AIBlock===================//
		{
			StageOwnedTypeInfo info;

			// AIBlock作成
			info.createObject = [](const std::shared_ptr<StageObject>& stageObj)
				{
					return AIBlockManager::Instance().CreateAIBlock(stageObj);
				};

			// リストを作成
			info.getObjects = []()
				{
					std::vector<std::shared_ptr<StageOwnedObject>>list;

					for (const auto& aiBlock : AIBlockManager::Instance().GetAIBlockList())
					{
						if (!aiBlock || aiBlock->IsExpired())
						{
							continue;
						}

						list.push_back(aiBlock);
					}

					return list;
				};

			// デバッグをしているか
			info.isDebug = []() {return AIBlockManager::Instance().IsDebug(); };

			// デバッグ状態をセット
			info.setDebug = [](bool flg) {AIBlockManager::Instance().SetIsDebug(flg); };

			// 引数がAIBlockなのかを調べる
			info.isType = [](const std::shared_ptr<StageOwnedObject>& ownedObj)
				{
					return std::dynamic_pointer_cast<AIBlock>(ownedObj) != nullptr;
				};

			table[HierarchyCategory::AIBlock] = info;

		}

		// ===================WayPoint===================//
		{
			StageOwnedTypeInfo info;

			// WayPoint作成
			info.createObject = [](const std::shared_ptr<StageObject>& stageObj)
				{
					return WayPointManager::Instance().CreateWayPoint(stageObj);
				};

			// リストを作成
			info.getObjects = []()
				{
					std::vector<std::shared_ptr<StageOwnedObject>>list;

					for (const auto& wayPoint : WayPointManager::Instance().GetWayPoints())
					{
						if (!wayPoint || wayPoint->IsExpired())
						{
							continue;
						}

						list.push_back(wayPoint);
					}

					return list;
				};

			// デバッグをしているか
			info.isDebug = []() {return WayPointManager::Instance().IsDebug();	};

			// デバッグ状態をセット
			info.setDebug = [](bool flg) {WayPointManager::Instance().SetIsDebug(flg); };

			// 引数がWayPointなのかを調べる
			info.isType = [](const std::shared_ptr<StageOwnedObject>& ownedObj)
				{
					return std::dynamic_pointer_cast<WayPoint>(ownedObj) != nullptr;
				};

			table[HierarchyCategory::WayPoint] = info;

		}

		// ===================PointLight===================//
		{
			StageOwnedTypeInfo info;

			// PointLight作成
			info.createObject = [](const std::shared_ptr<StageObject>& stageObj)
				{
					return PointLightObjectManager::Instance().CreatePointLight(stageObj);
				};

			// リストを作成
			info.getObjects = []()
				{
					std::vector<std::shared_ptr<StageOwnedObject>>list;

					for (const auto& light : PointLightObjectManager::Instance().GetPointLights())
					{
						if (!light || light->IsExpired())
						{
							continue;
						}

						list.push_back(light);
					}

					return list;
				};

			// デバッグをしているか
			info.isDebug = []() {return PointLightObjectManager::Instance().IsDebug(); };

			// デバッグ状態をセット
			info.setDebug = [](bool flg) {PointLightObjectManager::Instance().SetIsDebug(flg); };

			// 引数がPointLightなのかを調べる
			info.isType = [](const std::shared_ptr<StageOwnedObject>& ownedObj)
				{
					return std::dynamic_pointer_cast<PointLightObject>(ownedObj) != nullptr;
				};

			table[HierarchyCategory::PointLight] = info;
		}

		return table;
	}
	

}

const StageOwnedTypeInfo* FindStageOwnedTypeInfo(HierarchyCategory category)
{

	const auto& table = GetStageOwnedTypeInfoTable();

	auto it = table.find(category);

	if (it == table.end())
	{
		return nullptr;
	}
	else
	{
		// カテゴリのStageOwnedTypeInfoのアドレスを返す
		return &it->second;
	}

}

const std::map<HierarchyCategory, StageOwnedTypeInfo>& GetStageOwnedTypeInfoTable()
{

	// 表を一度だけ作る
	static const std::map<HierarchyCategory, StageOwnedTypeInfo> table = BuildStageOwnedTypeTable();

	return table;
}
