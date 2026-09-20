#include "StageLoder.h"

std::filesystem::path StageLoder::GetRenderModelPath(const std::string& modelName) const
{
	return GetModelFolder(modelName)/( modelName + ".gltf");
}

std::filesystem::path StageLoder::GetBackGroundModelPath(const std::string& modelName) const
{
	return GetModelFolder(modelName) / "BackGround.gltf";
}

std::filesystem::path StageLoder::GetGroundCollisionPath(const std::string& modelName) const
{
	return GetModelFolder(modelName) / "GroundCollision.gltf";
}

std::filesystem::path StageLoder::GetAIBlockCollisionPath(const std::string& modelName) const
{
	// 実際のモデルファイル名は"AIBlock.gltf"(AIBlockCollision.cppで使用しているファイル名に合わせている)
	return GetModelFolder(modelName) / "AIBlock.gltf";
}

std::filesystem::path StageLoder::GetOBBCollisionDataPath(const std::string& modelName) const
{
	return GetModelDataFolder(modelName) / "OBBCollisionData.json";
}

std::filesystem::path StageLoder::GetAABBCollisionDataPath(const std::string& modelName) const
{
	return GetModelDataFolder(modelName) / "AABBCollisionData.json";
}

std::filesystem::path StageLoder::GetWayPointDataPath(const std::string& modelName) const
{
	return GetModelDataFolder(modelName) / "WayPointData.json";
}

std::vector<std::string> StageLoder::GetAvailableModelNames() const
{
	
	std::vector<std::string> modelNames;

	for (const auto &entry : std::filesystem::directory_iterator(StageModelPath))
	{
		if (entry.is_directory())
		{
			modelNames.push_back(entry.path().filename().string());
		}
	}

	return modelNames;
}

std::filesystem::path StageLoder::GetModelFolder(const std::string& modelName) const
{
	return std::filesystem::path(StageModelPath)/modelName;
}

std::filesystem::path StageLoder::GetModelDataFolder(const std::string& modelName) const
{
	return std::filesystem::path(StageModelDataPath) / modelName;
}
