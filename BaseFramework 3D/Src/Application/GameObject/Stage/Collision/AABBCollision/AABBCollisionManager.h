#pragma once

class StageObject;

class AABBCollision;

class AABBCollisionManager
{

public:


	std::shared_ptr<AABBCollision> CreateAABBCollision(const std::shared_ptr<StageObject>& owner);

	void RemoveByOwner(const StageObject* owner);

	bool Save(const std::string& filePath,const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath,const std::shared_ptr<StageObject>&owner);


	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	std::vector<std::shared_ptr<AABBCollision>> GetAABBCollisionList() const;
	
	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

private:

	bool m_isDebug = false;

	
private:

	AABBCollisionManager() = default;
	~AABBCollisionManager() = default;

public:

	static AABBCollisionManager&Instance()
	{
		static AABBCollisionManager instance;
		return instance;
	}

};
