#pragma once

class StageEditor
{
public:
	StageEditor() = default;
	~StageEditor() = default;

	void SetCurrentStageName(const std::string& name)
	{
		m_currentStageName = name;
	}
	const std::string& GetCurrentStageName() const
	{
		return m_currentStageName;
	}

	void Draw();

private:
	// 新規作成
	void CreateNewStage();
	void DrawNewStagePopup();

	// 現在編集対象のステージをクリア
	void ClearStage();

	// 保存
	bool SaveStage(const std::string& stageName);

	void RequestSaveStage();
	void DrawSaveStagePopup();
	void DrawOverwritePopup();

	// 読み込み
	bool LoadStage(const std::string& stageName);

	// 保存済みステージ一覧
	void RefreshStageList();
	void DrawOpenStagePopup();

	// ロード前の保存確認
	void RequestLoadStage(const std::string& stageName);
	void DrawLoadConfirmationPopup();

	std::filesystem::path GetStageFolder(const std::string& stageName) const;

	


private:

	std::string m_currentStageName;

	// Popupへの一時入力。確定するまで現在のステージ名は変更しない
	char m_newStageName[128]{};
	char m_saveStageName[128]{};
	bool m_closeSavePopup = false;


	// 保存済ステージ名
	std::vector<std::string>m_stageNames;

	// ユーザーがロードしようとしているステージ
	std::string m_pendingLoadStageName;

	// 保存確認Popupを次のフレームで開く
	bool m_requestLoadConfirmation = false;

	// 一覧Popupを閉じる
	bool m_closeOpenStagePopup = false;




};
