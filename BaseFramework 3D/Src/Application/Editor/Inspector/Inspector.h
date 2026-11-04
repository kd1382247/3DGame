#pragma once

#include"../EditorPanel/EditorPanel.h"

class StageObject;

class Inspector:public EditorPanel
{
public:

	Inspector() :EditorPanel("Inspector") {}
	~Inspector()override {}

protected:

	void OnDraw()override;

	ImGuiWindowFlags GetImGuiWindowFlags()const override;

private:


	void DrawCopyButton(const std::shared_ptr<KdGameObject>& obj);

	void DrawDeleteButton(const std::shared_ptr<KdGameObject>& obj);

	// ステージ削除の確認ダイアログ(ギミックも一緒に消えることを知らせる)
	void DrawDeleteStagePopup(const std::shared_ptr<StageObject>& stage);

	// オブジェクトを削除する
	void DeleteObject(const std::shared_ptr<KdGameObject>& obj);

};