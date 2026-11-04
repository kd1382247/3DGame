#pragma once

class EditorPanel
{
public:

	EditorPanel(const std::string& windowName) :m_windowName(windowName) {}
	virtual ~EditorPanel() = default;

	void Draw();

	// パネルを表示中か
	bool IsVisible()const { return m_isVisible; }

	// 表示状態を変える
	void SetVisible(bool flg) { m_isVisible = flg; }

	// ウィンドウの名前を返す
	const std::string& GetWindowName()const { return m_windowName; }

protected:

	// 派生クラスで実装
	virtual void OnDraw() = 0;

	// ウィンドウのフラグを設定
	virtual ImGuiWindowFlags GetImGuiWindowFlags()const { return 0; }

private:

	// パネルを表示するのか
	bool m_isVisible = true;

	std::string m_windowName = {};
};