#include "../../../Application/main.h"

#include "KdDebugGUI.h"

#include"../../../Application/Scene/SceneManager.h"
#include"../../../Application/Editor/EditorManager.h"
#include"../../../Application/Scene/EditorScene/EditorScene.h"


KdDebugGUI::KdDebugGUI()
{}
KdDebugGUI::~KdDebugGUI()
{ 
	GuiRelease(); 
}

void KdDebugGUI::GuiInit(int w, int h)
{
	// 初期化済みなら動作させない
	if (m_uqLog) return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(Application::Instance().GetWindowHandle(), ImVec2(w,h));
	ImGui_ImplDX11_Init(KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());

#include "imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	// 日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);
	m_uqLog = std::make_unique<ImGuiAppLog>();

	m_uqErrorLog = std::make_unique<ImGuiAppLog>();
}

void KdDebugGUI::GuiProcess()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	//===========================================================
	// ImGui開始
	//===========================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//===========================================================
	// 以下にImGui描画処理を記述
	//===========================================================

	// デバッグウィンドウ(日本語を表示したい場合はこう書く)
//	if (ImGui::Begin(U8("えふぴぃえす")))
//	{
		// FPS
//		ImGui::Text("FPS : %d", Application::Instance().GetNowFPS());
//	}
//	ImGui::End();


	DrawEditor();


	//=====================================================
	// ログ出力 ・・・ AddLog("～") で追加
	//=====================================================

//	m_uqLog->AddLog("hello world\n");

	//=====================================================
	// 別ソースファイルからログを出力する場合
	//=====================================================

//	KdDebugGUI::Instance().AddLog("TestLog\n");

	//===========================================================
	// ここより上にImGuiの描画はする事
	//===========================================================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void KdDebugGUI::AddLog(const char* fmt,...)
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	// 長いメッセージでも落ちないよう、バッファを大きくして
	// 入りきらない分は切り捨てる(vsnprintfは必ず終端文字を付ける)
	char tmpStr[1024] = {};
	va_list args;
	va_start(args, fmt);
	vsnprintf(tmpStr, sizeof(tmpStr), fmt, args);
	va_end(args);

	// 整形済みの文字列を書式として解釈させない("%"が含まれていても壊れない)
	m_uqLog->AddLog("%s", tmpStr);
}

void KdDebugGUI::ClearLog()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog->Clear();
}

void KdDebugGUI::AddErrorLog(const char* fmt, ...)
{
	// 初期化されてないなら動作させない
	if (!m_uqErrorLog) return;

	// 長いメッセージでも落ちないよう、バッファを大きくして
	// 入りきらない分は切り捨てる(vsnprintfは必ず終端文字を付ける)
	char tmpStr[1024] = {};
	va_list args;
	va_start(args, fmt);
	vsnprintf(tmpStr, sizeof(tmpStr), fmt, args);
	va_end(args);

	// 整形済みの文字列を書式として解釈させない("%"が含まれていても壊れない)
	m_uqErrorLog->AddLog("%s", tmpStr);
}

void KdDebugGUI::ClearErrorLog()
{
	// 初期化されてないなら動作させない
	if (!m_uqErrorLog) return;

	m_uqErrorLog->Clear();
}

void KdDebugGUI::GuiRelease()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog = nullptr;
	m_uqErrorLog = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void KdDebugGUI::DrawEditor()
{

	// エディタシーンのみ表示
	if (!SceneManager::Instance().GetCurrentScene<EditorScene>())
	{
		return;
	}

	if (EditorManager::Instance().IsPlayMode())
	{
		// ログウィンドウ
		m_uqLog->Draw("Log Window");
	}

	if (EditorManager::Instance().IsEditMode())
	{
		// エラーウィンドウ
		m_uqErrorLog->Draw("Error Window");
	}

	EditorManager::Instance().Draw();

}
