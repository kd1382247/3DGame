#include "AIBlockManager.h"

#include"AIBlock.h"

#include"../../../../Editor/EditorManager.h"

std::shared_ptr<AIBlock> AIBlockManager::CreateAIBlock()
{
	const int id = FindAvailableID();

	std::shared_ptr<AIBlock> block = std::make_shared<AIBlock>();

	block->Init();

	block->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	block->SetObjectName("AIBlock_" + std::to_string(id));

	m_spAIBlockList.push_back(block);

	return block;
}

void AIBlockManager::RemoveAIBlock(int id)
{
	auto target = FindAIBlock(id);

	if (!target)
	{
		return;
	}

	auto it = std::find(m_spAIBlockList.begin(), m_spAIBlockList.end(), target);

	if (it == m_spAIBlockList.end())
	{
		return;
	}

	m_spAIBlockList.erase(it);
}

void AIBlockManager::RemoveByOwner(const StageObject* owner)
{
	std::erase_if(m_spAIBlockList,
		[owner](const std::shared_ptr<AIBlock>& block)
		{
			return block && block->GetOwner().get() == owner;
		}
	);
}

std::shared_ptr<AIBlock> AIBlockManager::FindAIBlock(int id)const
{
	for (const auto& block : m_spAIBlockList)
	{
		if (!block)
		{
			continue;
		}

		if (block->GetID() == id)
		{
			return block;
		}
	}

	return nullptr;
}

void AIBlockManager::ClearAIBlockList()
{
	m_spBackupList = std::move(m_spAIBlockList);
	m_spAIBlockList.clear();
}

void AIBlockManager::RestoreAIBlockList()
{
	m_spAIBlockList = std::move(m_spBackupList);
	m_spBackupList.clear();
}

void AIBlockManager::ClearBackup()
{
	m_spBackupList.clear();
}

bool AIBlockManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	nlohmann::json rootJson;
	rootJson["AIBlocks"] = nlohmann::json::array();

	for (const auto& block : m_spAIBlockList)
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
		blockJson["Position"]["x"] = pos.x;
		blockJson["Position"]["y"] = pos.y;
		blockJson["Position"]["z"] = pos.z;

		// 大きさ(Stageからのローカル大きさ)
		const auto& scale = block->GetLocalScale();
		blockJson["Scale"]["x"] = scale.x;
		blockJson["Scale"]["y"] = scale.y;
		blockJson["Scale"]["z"] = scale.z;

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
		auto block = std::make_shared<AIBlock>();

		block->Init();

		// ID(使われていない最小のID)
		block->SetID(FindAvailableID());

		// 名前
		block->SetObjectName("AIBlock_" + std::to_string(block->GetID()));

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

		block->SetOwner(owner);

		m_spAIBlockList.push_back(block);
	}

	return true;
}

int AIBlockManager::FindAvailableID() const
{
	int id = 0;

	while (FindAIBlock(id))
	{
		++id;
	}

	return id;
}

void AIBlockManager::DrawDebug()
{
	if (!IsDebug())
	{
		return;
	}

	for (const auto& block : m_spAIBlockList)
	{
		if (!block) { continue; }

		// 持ち主が違う場合表示しない
		if (block->GetOwner() != EditorManager::Instance().GetActiveStage())
		{
			continue;
		}

		block->DrawDebug();
	}
}

void AIBlockManager::SetStageTransform(const StageObject* stageObject, const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	for (const auto& block : m_spAIBlockList)
	{
		if (!block ||
			!block->ShouldFollow(stageObject))
		{
			continue;
		}

		block->SetStageTransform(stagePos, stageScale);
	}
}
