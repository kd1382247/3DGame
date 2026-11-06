#include "AIBlockManager.h"

#include"AIBlock.h"


#include"../../../../Scene/SceneManager.h"
#include"../../../../Editor/InspectorWidgets.h"

std::shared_ptr<AIBlock> AIBlockManager::CreateAIBlock(const std::shared_ptr<StageObject>& owner)
{
	const int id = FindAvailableID();

	std::shared_ptr<AIBlock> block = std::make_shared<AIBlock>();

	block->Init();

	block->SetID(id);

	block->SetOwner(owner);

	// オブジェクトの名前をセット 後ろにID
	block->SetObjectName("AIBlock_" + std::to_string(id));

	SceneManager::Instance().AddObject(block);

	return block;
}

void AIBlockManager::RemoveByOwner(const StageObject* owner)
{
	for (const auto& aiBlock : GetAIBlockList())
	{
		if (aiBlock->GetOwner().get() == owner)
		{
			aiBlock->Destroy();
		}
	}
}

bool AIBlockManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	nlohmann::json rootJson;
	rootJson["AIBlocks"] = nlohmann::json::array();

	for (const auto& block : GetAIBlockList())
	{
		if (!block ||
			block->GetOwner() != owner)
		{
			continue;
		}

		nlohmann::json blockJson;

		blockJson["ID"] = block->GetID();

		blockJson["Name"] = block->GetObjectName();

		// 座標(Stageからのローカル位置)
		const auto& pos = block->GetLocalPos();
		blockJson["Position"]["x"] = InspectorUI::CeilToDigits(pos.x, 3);
		blockJson["Position"]["y"] = InspectorUI::CeilToDigits(pos.y, 3);
		blockJson["Position"]["z"] = InspectorUI::CeilToDigits(pos.z, 3);

		// 大きさ(Stageからのローカル大きさ)
		const auto& scale = block->GetLocalScale();
		blockJson["Scale"]["x"] = InspectorUI::CeilToDigits(scale.x, 3);
		blockJson["Scale"]["y"] = InspectorUI::CeilToDigits(scale.y, 3);
		blockJson["Scale"]["z"] = InspectorUI::CeilToDigits(scale.z, 3);

		rootJson["AIBlocks"].push_back(blockJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("AIBlockDataの保存に失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("AIBlockDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool AIBlockManager::Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("AIBlockDataを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("AIBlockDataを開けませんでした\n");

		return false;
	}

	nlohmann::json rootJson;

	try
	{
		file >> rootJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("AIBlockDataの読み込みに失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("AIBlockDataの読み込みに失敗しました\n");

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	if (!rootJson.contains("AIBlocks") ||
		!rootJson["AIBlocks"].is_array())
	{
		OutputDebugStringA("AIBlocks配列がありません\n");
		KdDebugGUI::Instance().AddErrorLog("AIBlocks配列がありません\n");

		return false;
	}

	// Jsonに保存されてる情報でAIBlockを生成
	for (const auto& blockJson : rootJson["AIBlocks"])
	{
		auto block = CreateAIBlock(owner);

		// 座標(Stageからのローカル位置)
		block->SetLocalPos({
			blockJson["Position"]["x"].get<float>(),
			blockJson["Position"]["y"].get<float>(),
			blockJson["Position"]["z"].get<float>()
			});

		// 大きさ(Stageからのローカル大きさ)
		block->SetLocalScale({
			blockJson["Scale"]["x"].get<float>(),
			blockJson["Scale"]["y"].get<float>(),
			blockJson["Scale"]["z"].get<float>()
			});
	}

	return true;
}

int AIBlockManager::FindAvailableID() const
{
	
	const auto aiBlockList = GetAIBlockList();

	int id = 0;

	// 未使用のIDが見つかるまでループを回す
	while (true)
	{
		bool exists = false;

		for (const auto& aiBlock : aiBlockList)
		{
			// 今のIDがすでに使われていたらループを抜ける
			if (aiBlock->GetID() == id)
			{
				exists = true;
				break;
			}
		}

		// 未使用のIDが見つかったら処理終了
		if (!exists)
		{
			return id;
		}

		// IDを加算
		++id;
	}
}

std::vector<std::shared_ptr<AIBlock>> AIBlockManager::GetAIBlockList() const
{

	std::vector<std::shared_ptr<AIBlock>> aiBlockList;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto aiBlock = std::dynamic_pointer_cast<AIBlock>(obj);

		if (!aiBlock || aiBlock->IsExpired())
		{
			continue;
		}

		aiBlockList.push_back(aiBlock);
	}

	return aiBlockList;
}