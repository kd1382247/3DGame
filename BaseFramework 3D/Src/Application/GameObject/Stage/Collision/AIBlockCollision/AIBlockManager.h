#pragma once

class StageObject;

class AIBlock;

// AIBlockの作成・保存・読み込みを行う
// AABBCollisionManagerと同じ作りで、ステージモデル単位で保存する
class AIBlockManager
{

public:

	std::shared_ptr<AIBlock> CreateAIBlock();

	void RemoveAIBlock(int id);

	// ownerが持ち主のブロックを全て消す(ステージの削除・ステージモデルの変更で呼ぶ)
	void RemoveByOwner(const StageObject* owner);

	std::shared_ptr<AIBlock> FindAIBlock(int id)const;

	void ClearAIBlockList();
	// ブロックを復元
	void RestoreAIBlockList();
	// バックアップをクリア
	void ClearBackup();

	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	const std::vector<std::shared_ptr<AIBlock>>& GetAIBlockList() const
	{
		return m_spAIBlockList;
	}

	// デバッグの表示切り替えフラグ
	bool IsDebug()const { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

	// デバッグ表示
	void DrawDebug();

	// Stage(親)のワールド位置・大きさを、管理している全ブロックに反映する
	void SetStageTransform(const StageObject* stageObject, const Math::Vector3& stagePos, const Math::Vector3& stageScale);

private:

	std::vector<std::shared_ptr<AIBlock>>m_spAIBlockList;

	std::vector<std::shared_ptr<AIBlock>>m_spBackupList;

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
