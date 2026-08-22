#pragma once

class WallCollision;

class WallCollisionManager
{

public:


	std::shared_ptr<WallCollision> CreateWallCollision();

	void RemoveWallCollision(int id);


	std::shared_ptr<WallCollision> FindWallCollision(int id)const;


	void ClearWallCollisionList();
	// ウェイポイントを復元
	void RestoreWallCollisionList();
	// バックアップをクリア
	void ClearBackup();


	bool Save(const std::string& filePath);

	bool Load(const std::string& filePath);


	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	const std::vector<std::shared_ptr<WallCollision>>& GetWallCollisionList() const
	{
		return m_spWallCollisionList;
	}

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetDebugFlg(const bool flg) { m_isDebug = flg; }

	// デバッグ表示
	void DrawDebug();

private:

	void Init();

	std::vector<std::shared_ptr<WallCollision>>m_spWallCollisionList;

	std::vector<std::shared_ptr<WallCollision>>m_spBackupList;

	bool m_isDebug = false;

private:

	WallCollisionManager(){}
	~WallCollisionManager(){}

public:

	static WallCollisionManager&Instance()
	{
		static WallCollisionManager instance;
		return instance;
	}

};