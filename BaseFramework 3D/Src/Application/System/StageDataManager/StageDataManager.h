#pragma once

class StageDataManager
{

public:

	bool Save(const std::string& stageName);
	bool Load(const std::string& stageName);

	bool SaveTemporary();
	bool LoadTemporary();



private:

	std::filesystem::path GetStageFolder(const std::string& stageName) const;

	bool SaveToFolder(const std::filesystem::path& folder);
	bool LoadFromFolder(const std::filesystem::path& folder);


private:

	StageDataManager(){}
	~StageDataManager(){}

public:

	static StageDataManager& Instance()
	{
		static StageDataManager instance;
		return instance;
	}

};