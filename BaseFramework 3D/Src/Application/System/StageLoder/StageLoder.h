#pragma once

class StageLoder
{
public:

	// レンダリング用メインモデルのパス
	std::filesystem::path GetRenderModelPath(const std::string& modelName) const;

	// 背景モデルのパス
	std::filesystem::path GetBackGroundModelPath(const std::string& modelName) const;

	// GroundCollisionメッシュのパス
	std::filesystem::path GetGroundCollisionPath(const std::string& modelName) const;

	// AIBlockCollisionメッシュのパス
	std::filesystem::path GetAIBlockCollisionPath(const std::string& modelName) const;

	// OBBCollisionデータ(json)のパス
	std::filesystem::path GetOBBCollisionDataPath(const std::string& modelName) const;

	// AABBCollisionデータ(json)のパス
	std::filesystem::path GetAABBCollisionDataPath(const std::string& modelName) const;

	// WayPointデータ(json)のパス
	std::filesystem::path GetWayPointDataPath(const std::string& modelName) const;

private:

	// モデル置き場のフォルダー "Asset/Models/Stage/<modelName>"
	std::filesystem::path GetModelFolder(const std::string& modelName) const;

	// モデル用データ置き場のフォルダー "Asset/Data/StageModel/<modelName>"
	std::filesystem::path GetModelDataFolder(const std::string& modelName) const;

private: // シングルトンパターン

	StageLoder() {}
	~StageLoder() {}

public:

	static StageLoder& Instance()
	{
		static StageLoder instance;
		return instance;
	}
};

#define StageModelDataPath "Asset/Data/StageModel"
#define StageModelPath "Asset/Models/Stage"