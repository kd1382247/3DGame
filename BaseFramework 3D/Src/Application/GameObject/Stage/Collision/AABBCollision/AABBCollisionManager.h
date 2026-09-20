#pragma once

class AABBCollision;

class AABBCollisionManager
{

public:


	std::shared_ptr<AABBCollision> CreateAABBCollision();

	void RemoveAABBCollision(int id);


	std::shared_ptr<AABBCollision> FindAABBCollision(int id)const;


	void ClearAABBCollisionList();
	// ウェイポイントを復元
	void RestoreAABBCollisionList();
	// バックアップをクリア
	void ClearBackup();


	bool Save(const std::string& filePath);

	bool Load(const std::string& filePath);


	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	const std::vector<std::shared_ptr<AABBCollision>>& GetAABBCollisionList() const
	{
		return m_spAABBCollisionList;
	}

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetDebugFlg(const bool flg) { m_isDebug = flg; }

	// デバッグ表示
	void DrawDebug();

	// Stage(親)のワールド位置・大きさを、管理している全AABBに反映する
	void SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale);

private:

	void Init();

	std::vector<std::shared_ptr<AABBCollision>>m_spAABBCollisionList;

	std::vector<std::shared_ptr<AABBCollision>>m_spBackupList;

	bool m_isDebug = false;

private:

	AABBCollisionManager(){}
	~AABBCollisionManager(){}

public:

	static AABBCollisionManager&Instance()
	{
		static AABBCollisionManager instance;
		return instance;
	}

};
