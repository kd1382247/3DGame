#pragma once

class StageObject;

class AIBlock;

// AIBlockの作成・保存・読み込みを行う
// AABBCollisionManagerと同じ作りで、ステージモデル単位で保存する
class AIBlockManager
{

public:

	std::shared_ptr<AIBlock> CreateAIBlock(const std::shared_ptr<StageObject>& owner);

	// ownerが持ち主のブロックを全て消す(ステージの削除・ステージモデルの変更で呼ぶ)
	void RemoveByOwner(const StageObject* owner);

	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	std::vector<std::shared_ptr<AIBlock>> GetAIBlockList() const;

	// デバッグの表示切り替えフラグ
	bool IsDebug()const { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

private:

	bool m_isDebug = false;

private:

	AIBlockManager() {}
	~AIBlockManager() {}

public:

	static AIBlockManager& Instance()
	{
		static AIBlockManager instance;
		return instance;
	}

};
