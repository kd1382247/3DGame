#include "Title.h"

#include "../../Scene/SceneManager.h"

#include"../../System/Mouse/Mouse.h"

#include"../../Scene/Setting/Setting.h"


void Title::Init()
{
	// 各テクスチャを初期化
	InitTexture(m_spTitleBGTex,       "TitleBG.png");
	InitTexture(m_spTitleLogoTex,     "TitleLogo2.png");
	InitTexture(m_spCharactersTex,    "Characters.png");
	InitTexture(m_spStartButtonTex,   "StartButton2.png");
	InitTexture(m_spSettingButtonTex, "SettingButton2.png");


}

void Title::Update()
{
	// 画像をサインカーブで上下に揺らす
	UpdateSinCurve();

	UpdateButtonInput();

}

void Title::DrawSprite()
{

	
	// 背景
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTitleBGTex, 0, 0);
	// キャラクター
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCharactersTex, 0, m_charactersTexPosY);

	// 加算合成で描画
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);

	// タイトルロゴ
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTitleLogoTex, 0, 250);

	// ボタンを描画
	DrawButton(m_spStartButtonTex,   m_startButtonPos,   m_isHoverStartButton);
	DrawButton(m_spSettingButtonTex, m_settingButtonPos, m_isHoverSettingButton);

	KdShaderManager::Instance().UndoBlendState();

}

void Title::InitTexture(std::shared_ptr<KdTexture>& tex, std::string fileName)
{
	if (tex)
	{
		return;
	}

	tex = std::make_shared<KdTexture>();
	tex->Load(FilePath + fileName);

}

void Title::DrawButton(const std::shared_ptr<KdTexture> tex, const Math::Vector2& pos, bool isHover)
{

	Math::Vector2 buttonTexSize = ButtonTexSize;

	if (isHover)
	{
		buttonTexSize.x += 20;
		buttonTexSize.y += 10;
	}

	KdShaderManager::Instance().m_spriteShader.DrawTex(tex, pos.x, pos.y,buttonTexSize.x,buttonTexSize.y);
}

void Title::UpdateSinCurve()
{
	m_angle += 3.1;
	if (m_angle > 360)
	{
		m_angle -= 360;
	}

	m_charactersTexPosY += std::sinf(DirectX::XMConvertToRadians(m_angle)) * 0.2;

}

void Title::UpdateButtonInput()
{
	// マウスの左ボタンの状態 (「押した瞬間」を取るため、毎フレーム更新する)
	const bool isMouseDown    = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
	const bool isMouseTrigger = isMouseDown && !m_isPrevMouseDown;
	m_isPrevMouseDown = isMouseDown;

	// 設定画面が開いている間は、タイトルのボタンを操作できないようにする
	std::shared_ptr<Setting> spSetting = m_wpSetting.lock();
	if (spSetting && spSetting->IsOpen())
	{
		m_isHoverStartButton   = false;
		m_isHoverSettingButton = false;
		return;
	}

	POINT mousePos = Mouse::Instance().Get2DMousePos();

	// スタートボタンの幅を算出
	Math::Vector2 startButtonMax = m_startButtonPos + ButtonTexSize/2;
	Math::Vector2 startButtonMin = m_startButtonPos - ButtonTexSize/2;

	// マウスがスタートボタンの上にいるか
	if (startButtonMin.x < mousePos.x && mousePos.x<startButtonMax.x &&
		startButtonMin.y<mousePos.y && mousePos.y < startButtonMax.y)
	{
		m_isHoverStartButton = true;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			SceneManager::Instance().SetNextScene(SceneManager::SceneType::Editor);
		}
	}
	else
	{
		m_isHoverStartButton = false;
	}

	// スタートボタンの幅を算出
	Math::Vector2 settingButtonMax = m_settingButtonPos + ButtonTexSize / 2;
	Math::Vector2 settingButtonMin = m_settingButtonPos - ButtonTexSize / 2;

	// マウスがスタートボタンの上にいるか
	if (settingButtonMin.x < mousePos.x && mousePos.x < settingButtonMax.x &&
		settingButtonMin.y < mousePos.y && mousePos.y < settingButtonMax.y)
	{
		m_isHoverSettingButton = true;

		// クリックした瞬間に設定画面を開く
		if (isMouseTrigger && spSetting)
		{
			spSetting->Open();
		}
	}
	else
	{
		m_isHoverSettingButton = false;
	}

}
