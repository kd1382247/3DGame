#pragma once

#include"../MessageWindow/MessageWindow.h"

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

	const bool IsEditStage()const { return m_isEditStage; }

	void Draw();

private:

	// 新規作成
	void CreateNewStage();

	// ポップアップをまとめて管理
	void DrawPopups();

	void NewStagePopup();
	void NewStageConfirmationPopup();

	// 現在編集対象のステージをクリア
	void ClearStage();

	// セーブのポップアップ
	void SaveStagePopup();
	// 上書きチェック
	void OverwritePopup();

	// 保存済みステージ一覧
	void RefreshStageList();
	void OpenStagePopup();

	// ロード前の保存確認
	void RequestLoadStage(const std::string& stageName);
	void LoadConfirmationPopup();

	// ロード成功後の処理
	void  LoadSucceeded(const std::string&stageName);

	// ロード失敗後の処理
	void LoadFailed();

	// ステージ名が被っていないか
	bool IsStageExists(const std::string& stageName) const;

	// メッセージウィンドウを表示
	bool HasSaveTarget() { return m_hasSaveTarget; }


private:

	// メッセージウィンドウクラス
	MessageWindow m_messageWindow;


	// 編集中のステージ名
	std::string m_currentStageName;

	// 保存済ステージ名リスト
	std::vector<std::string>m_stageNames;

	// ユーザーがロードしようとしているステージ名
	std::string m_pendingLoadStageName;

	// 編集対象のステージが存在するか
	bool m_isEditStage = false;

	// セーブする対象を判定、同じであれば上書きの確認はしない
	bool m_hasSaveTarget = false;



	// Popupへの一時入力。確定するまで現在のステージ名は変更しない
	char m_newStageName[128]{};
	char m_saveStageName[128]{};


	bool m_closeSavePopup = false;

	// 保存確認Popupを次のフレームで開く
	bool m_requestLoadConfirmation = false;

	// 保存確認Popupを次のフレームで開く
	bool m_requestNewStageConfirmation = false;

	// 一覧Popupを閉じる
	bool m_closeOpenStagePopup = false;


};
