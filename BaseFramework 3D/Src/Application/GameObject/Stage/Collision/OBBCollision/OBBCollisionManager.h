#pragma once

class StageObject;

class OBBCollision;

class OBBCollisionManager
{

public:


	std::shared_ptr<OBBCollision> CreateOBBCollision();

	void RemoveOBBCollision(int id);

	void RemoveByOwner(const StageObject* owner);

	std::shared_ptr<OBBCollision> FindOBBCollision(int id)const;


	void ClearOBBCollisionList();
	// ウェイポイントを復元
	void RestoreOBBCollisionList();
	// バックアップをクリア
	void ClearBackup();


	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);


	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	const std::vector<std::shared_ptr<OBBCollision>>& GetOBBCollisionList() const
	{
		return m_spOBBCollisionList;
	}

	// デバッグの表示切り替えフラグ
	bool IsDebug() { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }

	// デバッグ表示
	void DrawDebug();

	// Stage(親)のワールド位置・大きさを、管理している全OBBに反映する
	void SetStageTransform(const StageObject*stageObject,const Math::Vector3& stagePos, const Math::Vector3& stageScale);

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
