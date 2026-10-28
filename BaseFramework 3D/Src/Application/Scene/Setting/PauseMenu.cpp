#include "PauseMenu.h"

#include "../../../Framework/Audio/KdAudio.h"
#include "../../System/Mouse/Mouse.h"
#include "Setting.h"

namespace
{
	// 座標は「画面中央が(0,0)・上がプラス」 (Mouse::Get2DMousePos と同じ)

	// 画面サイズの半分 (1280x720)
	constexpr int ScreenHalfW = 640;
	constexpr int ScreenHalfH = 360;

	// 画面を暗くする色
	const Math::Color DimColor = { 0.0f, 0.0f, 0.0f, 0.6f };

	// ボタン (画像の大きさ 410x70。上から: CONTROLS / SETTING / TITLE)
	// 当たり判定(マウスが乗る範囲)も、この大きさを使う
	constexpr int ButtonWidth  = 410;
	constexpr int ButtonHeight = 70;
	constexpr int ItemCount    = 3;

	// ボタンの中心のY座標 (上から順。ボタン同士の間隔は、ボタンの高さ70の分を引いて50)
	constexpr int ButtonY[ItemCount] = { 120, 0, -120 };

	// 選ばれているボタンの大きさの倍率
	constexpr float ButtonSelectScale = 1.1f;

	// 効果音のファイル名
	const std::string MoveSE   = "Asset/Data/Sound/SE/System/Select.wav";	// 選んでいる項目が変わった時
	const std::string DecideSE = "Asset/Data/Sound/SE/System/Confirm.wav";	// 決定した時

	// キー(マウスのボタン)が押されているか
	bool IsKeyDown(int key)
	{
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}

	// 項目の、ボタンの中心のY座標
	int GetButtonY(int index)
	{
		return ButtonY[index];
	}
}

void PauseMenu::Init()
{
	m_spControlsButtonTex = std::make_shared<KdTexture>();
	m_spControlsButtonTex->Load(TexFilePath + "ControlsButton.png");

	m_spSettingButtonTex = std::make_shared<KdTexture>();
	m_spSettingButtonTex->Load(TexFilePath + "SettingButton.png");

	m_spTitleButtonTex = std::make_shared<KdTexture>();
	m_spTitleButtonTex->Load(TexFilePath + "TitleButton.png");

	m_spControlsPanelTex = std::make_shared<KdTexture>();
	m_spControlsPanelTex->Load(TexFilePath + "ControlsPanel.png");

	m_page          = Page::Closed;
	m_selected      = Item::Controls;
	m_isIgnoreInput = false;
	m_isTitleRequested = false;
}

void PauseMenu::Update()
{
	// 入力の状態
	// 閉じている間も毎フレーム更新しておく (開いた瞬間に「押した瞬間」を正しく判定するため)
	const bool  isMouseDown = IsKeyDown(VK_LBUTTON);
	const POINT mousePos    = Mouse::Instance().Get2DMousePos();

	const bool isMouseTrigger = isMouseDown && !m_isPrevMouseDown;
	const bool isMouseMoved   = mousePos.x != m_prevMousePos.x || mousePos.y != m_prevMousePos.y;

	m_isPrevMouseDown = isMouseDown;
	m_prevMousePos    = mousePos;

	// 設定画面を、×ボタンなどで閉じたら、メニューへ戻る
	if (m_page == Page::Setting)
	{
		const std::shared_ptr<Setting> spSetting = m_wpSetting.lock();

		if (!spSetting || !spSetting->IsOpen())
		{
			ChangePage(Page::Menu);
		}
	}

	// TITLE を選んだ後は、入力を受け付けない (タイトルへ移動するのを待つ)
	if (m_isTitleRequested)
	{
		return;
	}

	// 画面を切り替えたフレームは、入力を受け付けない
	// (設定画面を×のEnterで閉じた時、そのEnterでメニューの項目まで決定してしまうのを防ぐ)
	if (m_isIgnoreInput)
	{
		m_isIgnoreInput = false;
		return;
	}

	if (m_page == Page::Menu)
	{
		UpdateMenu(isMouseTrigger, isMouseMoved, mousePos);
	}
}

void PauseMenu::DrawSprite()
{
	// 閉じている時と、設定画面の時(Setting が自分で描く)は何も描かない
	if (m_page != Page::Menu && m_page != Page::Controls)
	{
		return;
	}

	KdSpriteShader& sprite = KdShaderManager::Instance().m_spriteShader;

	// 画面全体を暗くする
	sprite.DrawBox(0, 0, ScreenHalfW, ScreenHalfH, &DimColor);

	if (m_page == Page::Controls)
	{
		// 操作説明のパネル
		sprite.DrawTex(m_spControlsPanelTex, 0, 0);

		return;
	}

	// メニューのボタン
	DrawButton(Item::Controls, m_spControlsButtonTex);
	DrawButton(Item::Setting,  m_spSettingButtonTex);
	DrawButton(Item::Title,    m_spTitleButtonTex);
}

void PauseMenu::Open()
{
	// 開く時は、いつも CONTROLS を選んだ状態から始める
	m_selected         = Item::Controls;
	m_isTitleRequested = false;

	ChangePage(Page::Menu);
}

void PauseMenu::Back()
{
	switch (m_page)
	{
	case Page::Controls:
		ChangePage(Page::Menu);
		break;

	case Page::Setting:
	{
		// 設定画面を閉じる (音量を保存して閉じる)。メニューへは、Update の中で戻る
		const std::shared_ptr<Setting> spSetting = m_wpSetting.lock();

		if (spSetting)
		{
			spSetting->Close();
		}

		ChangePage(Page::Menu);
		break;
	}

	case Page::Menu:
		ChangePage(Page::Closed);
		break;

	default:
		break;
	}
}

void PauseMenu::UpdateMenu(bool isMouseTrigger, bool isMouseMoved, const POINT& mousePos)
{

	// マウスは、動いた時だけ、乗っているボタンを選ぶ
	// (止まっている間も選ぶと、キーで選んだ項目をすぐ上書きしてしまう)
	Item hoverItem = Item::Controls;
	const bool isHover = GetHoverItem(mousePos, hoverItem);

	if (!isHover)
	{
		m_selected = Item::None;
		return;
	}

	if (isMouseMoved && isHover)
	{
		Select(hoverItem);
	}

	// ボタンの上でクリック

	if (isMouseTrigger && isHover)
	{
		Decide(hoverItem);
	}
}

void PauseMenu::Select(Item item)
{
	if (item == m_selected)
	{
		return;
	}

	m_selected = item;

	KdAudioManager::Instance().PlaySE(MoveSE, false);
}

void PauseMenu::Decide(Item item)
{
	m_selected = item;

	if (item == Item::Controls)
	{
		KdAudioManager::Instance().PlaySE(DecideSE, false);

		ChangePage(Page::Controls);
		return;
	}

	if (item == Item::Title)
	{
		KdAudioManager::Instance().PlaySE(DecideSE, false);

		// タイトルへの移動は GameScene が行う (IsTitleRequested() で聞かれる)
		m_isTitleRequested = true;
		return;
	}

	// 設定画面を開く (開く時の効果音は Setting が鳴らす)
	const std::shared_ptr<Setting> spSetting = m_wpSetting.lock();

	if (spSetting)
	{
		spSetting->Open();

		ChangePage(Page::Setting);
	}
}

void PauseMenu::ChangePage(Page page)
{
	m_page          = page;
	m_isIgnoreInput = true;
}

bool PauseMenu::GetHoverItem(const POINT& mousePos, Item& outItem)const
{
	if (std::abs(mousePos.x) > ButtonWidth / 2)
	{
		return false;
	}

	for (int i = 0; i < ItemCount; ++i)
	{
		if (std::abs(mousePos.y - GetButtonY(i)) <= ButtonHeight / 2)
		{
			outItem = static_cast<Item>(i);
			return true;
		}
	}

	return false;
}

void PauseMenu::DrawButton(Item item, const std::shared_ptr<KdTexture>& tex)
{
	if (!tex)
	{
		return;
	}

	// 選ばれているボタンは、明るく少し大きく描く
	const bool isSelected = (item == m_selected);

	const float scale      = isSelected ? ButtonSelectScale : 1.0f;

	const int texW = static_cast<int>(tex->GetWidth());
	const int texH = static_cast<int>(tex->GetHeight());

	const Math::Rectangle srcRect = { 0, 0, texW, texH };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		tex, 0, GetButtonY(static_cast<int>(item)),
		static_cast<int>(texW * scale), static_cast<int>(texH * scale),
		&srcRect);
}
