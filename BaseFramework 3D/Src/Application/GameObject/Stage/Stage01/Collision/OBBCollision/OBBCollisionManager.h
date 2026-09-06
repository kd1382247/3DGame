#pragma once

class OBBCollision;

class OBBCollisionManager
{

public:


	std::shared_ptr<OBBCollision> CreateOBBCollision();

	void RemoveOBBCollision(int id);

	std::shared_ptr<OBBCollision> FindOBBCollision(int id)const;


	void ClearOBBCollisionList();
	// ウェイポイントを復元
	void RestoreOBBCollisionList();
	// バックアップをクリア
	void ClearBackup();


	bool Save(const std::string& filePath);

	bool Load(const std::string& filePath);


	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	const std::vector<std::shared_ptr<OBBCollision>>& GetOBBCollisionList() const
	{
		return m_spOBBCollisionList;
	}

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetDebugFlg(const bool flg) { m_isDebug = flg; }

	// デバッグ表示
	void DrawDebug();

private:

	void Init();

	std::vector<std::shared_ptr<OBBCollision>>m_spOBBCollisionList;

	std::vector<std::shared_ptr<OBBCollision>>m_spBackupList;

	bool m_isDebug = false;

private:

	OBBCollisionManager() {}
	~OBBCollisionManager() {}

public:

	static OBBCollisionManager& Instance()
	{
		static OBBCollisionManager instance;
		return instance;
	}

};