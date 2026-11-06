#pragma once

class StageObject;

class OBBCollision;

class OBBCollisionManager
{

public:


	std::shared_ptr<OBBCollision> CreateOBBCollision(const std::shared_ptr<StageObject>& owner);

	void RemoveByOwner(const StageObject* owner);

	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);


	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	std::vector<std::shared_ptr<OBBCollision>> GetOBBCollisionList() const;

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

private:

	bool m_isDebug = false;

private:

	OBBCollisionManager() = default;
	~OBBCollisionManager() = default;

public:

	static OBBCollisionManager& Instance()
	{
		static OBBCollisionManager instance;
		return instance;
	}

};
