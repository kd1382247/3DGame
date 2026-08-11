#pragma once

class StageEditor
{
public:

	StageEditor(){}
	~StageEditor(){}
	
	void SetCurrentStageName(const std::string& name)
	{
		m_currentStageName = name;
	}
	const std::string& GetCurrentStageName()const
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

	// セーブ
	bool SaveStage();

	void RequestSaveStage();
	void DrawSaveStagePopup();
	void DrawOverwritePopup();

	// ロード
	bool LoadStage(const std::string& stageName);

	std::string m_currentStageName;

	// 新規作成時の一時入力
	char m_newStageName[128]{};


	char m_saveStageName[128]{};
	bool m_requestOverwrite = false;

	bool m_closeSavePopup = false;

};