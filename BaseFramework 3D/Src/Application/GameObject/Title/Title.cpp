#include "Title.h"

#include "../../Scene/SceneManager.h"

#include"../../System/Mouse/Mouse.h"
#include"../../System/Mouse/CursorVisibility.h"

#include"../../Scene/Setting/Setting.h"

#include"../../System/TimeManager/TimeManager.h"

namespace
{
	// キー(マウスのボタン)が押されているか
	bool IsKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}
}

void Title::Init()
{
	// タイトルではマウスカーソルを表示する
	// (ゲーム・ゲームオーバー・リザルトのどこから戻ってきても、必ず見える状態にする)
	SetCursorVisible(true);

	// 各テクスチャを初期化
	InitTexture(m_spTitleBGTex,       "TitleBG.png");
	InitTexture(m_spTitleLogoTex,     "TitleLogo2.png");
	InitTexture(m_spCharactersTex,    "Characters.png");
	InitTexture(m_spStartButtonTex,   "StartButton2.png");
	InitTexture(m_spSettingButtonTex, "SettingButton2.png");

	// 前のシーンで押していたキーで、いきなり決定されないようにする
	m_isPrevMouseDown = IsKeyDown(VK_LBUTTON);
	m_prevMousePos    = Mouse::Instance().Get2DMousePos();
}

void Title::Update()
{

	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Editor);
	}

	// 画像をサインカーブで上下に揺らす
	UpdateSinCurve();

	UpdateButtonInput();

	UpdateTitleLogoShine();

	UpdateButtonShine();
}

void Title::DrawSprite()
{
	
	// 背景
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTitleBGTex, 0, 0);
	// キャラクター
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spCharactersTex, 0, m_charactersTexPosY);

	// 加算合成で描画
	KdShaderManager::Instance().ChangeBlendState(KdBlendState::Add);

	//----------------------------------------------------------------
	// シャインで帯状に光らせる
	//----------------------------------------------------------------
	
	KdShaderManager::Instance().m_spriteShader.SetShineEnable(true);
	// シャインの値をセット
	KdShaderManager::Instance().m_spriteShader.SetShine(m_shinePos, ShineWidth,ShineIntensity);

	// タイトルロゴ
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_spTitleLogoTex, 0, 250);

	// シャインの値をセット
	//KdShaderManager::Instance().m_spriteShader.SetShine(m_buttonShinePos, ButtonShineWidth, ButtonShineIntensity,ButtonShineSlope);

	KdShaderManager::Instance().m_spriteShader.SetShineEnable(false);

	// ボタンを描画
	DrawButton(m_spStartButtonTex,   m_startButtonPos,   m_selectedButton == ButtonType::Start);
	DrawButton(m_spSettingButtonTex, m_settingButtonPos, m_selectedButton == ButtonType::Setting);



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

void Title::DrawButton(const std::shared_ptr<KdTexture> tex, const Math::Vector2& pos, bool isSelected)
{

	Math::Vector2 buttonTexSize = ButtonTexSize;

	// 選ばれているボタンは、少し大きく描く
	if (isSelected)
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
	// 入力の状態 (「押した瞬間」を取るため、毎フレーム更新する)
	const bool  isMouseDown = IsKeyDown(VK_LBUTTON);
	const POINT mousePos    = Mouse::Instance().Get2DMousePos();

	const bool isMouseTrigger = isMouseDown && !m_isPrevMouseDown;
	const bool isMouseMoved   = mousePos.x != m_prevMousePos.x || mousePos.y != m_prevMousePos.y;

	m_isPrevMouseDown = isMouseDown;
	m_prevMousePos    = mousePos;

	// 設定画面が開いている間は、タイトルのボタンを操作できないようにする
	std::shared_ptr<Setting> spSetting = m_wpSetting.lock();
	if (spSetting && spSetting->IsOpen())
	{
		return;
	}


	const bool isHoverStart = IsHoverButton(mousePos, m_startButtonPos);
	const bool isHoverSetting = IsHoverButton(mousePos, m_settingButtonPos);

	if (!isHoverStart && !isHoverSetting)
	{
		m_selectedButton = ButtonType::None;
		return;
	}

	// マウスは、動いた時だけ、乗っているボタンを選ぶ
	// (止まっている間も選ぶと、キーで選んだボタンをすぐ上書きしてしまう)
	if (isMouseMoved)
	{
		if (isHoverStart)
		{
			Select(ButtonType::Start);
		}
		else if (isHoverSetting)
		{
			Select(ButtonType::Setting);
		}
	}

	// ボタンの上でクリック
	if (isMouseTrigger)
	{
		if (isHoverStart)
		{
			Decide(ButtonType::Start);
		}
		else if (isHoverSetting)
		{
			Decide(ButtonType::Setting);
		}
	}
}

void Title::Select(ButtonType button)
{
	if (m_selectedButton == button)
	{
		return;
	}

	// SEを再生
	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/System/Select.wav", false);

	m_selectedButton = button;
}

void Title::Decide(ButtonType button)
{
	switch (button)
	{
	case ButtonType::Start:

		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
		// ゲーム中はカーソルを消す
		SetCursorVisible(false);

		// SEを再生
		KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/System/Confirm.wav", false);
		break;

	case ButtonType::Setting:
	{
		// 設定画面を開く
		std::shared_ptr<Setting> spSetting = m_wpSetting.lock();

		if (spSetting)
		{
			spSetting->Open();
		}

		break;
	}
	}
}

bool Title::IsHoverButton(const POINT& mousePos, const Math::Vector2& buttonPos) const
{
	// ボタンの範囲を算出
	const Math::Vector2 buttonMax = buttonPos + ButtonTexSize / 2;
	const Math::Vector2 buttonMin = buttonPos - ButtonTexSize / 2;

	return  buttonMin.x < mousePos.x && mousePos.x < buttonMax.x &&
			buttonMin.y < mousePos.y && mousePos.y < buttonMax.y;
}

void Title::UpdateTitleLogoShine()
{

	float deltaTime = TimeManager::Instance().GetDeltaTime();

	// 待ち時間中は動かさない
	if (m_shineWaitCount > 0)
	{
		m_shineWaitCount-=deltaTime;
		return;
	}

	// 左から右へ進める
	m_shinePos += ShineSpeed*deltaTime;

	// 右端を完全に抜けたら、左に戻して待ち時間へ
	if (m_shinePos > 1.0f + ShineWidth)
	{
		m_shinePos = -ShineWidth;
		m_shineWaitCount = ShineWaitTime;
	}
}

void Title::UpdateButtonShine()
{
	float deltaTime = TimeManager::Instance().GetDeltaTime();

	// 待ち時間中は動かさない
	if (m_buttonShineWaitTime > 0)
	{
		m_buttonShineWaitTime -= deltaTime;
		return;
	}
	// 左から右へ進める
	m_buttonShinePos += ButtonShineSpeed * deltaTime;

	// 右端を完全に抜けたら、左に戻して待ち時間へ
	if (m_buttonShinePos > 1.5f + ButtonShineWidth)
	{
		m_buttonShinePos = -ButtonShineWidth;
		m_buttonShineWaitTime = ButtonShineWaitTime;
	}
}
