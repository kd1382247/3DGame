#include "ResultScene.h"

#include"../SceneManager.h"
#include"../../BGMManager/BGMManager.h"
#include"../../GameObject/GameResultUI/ResultUI.h"

void ResultScene::Event()
{
	if (!m_spResultUI || m_isNextSceneRequested)
	{
		return;
	}

	// 選ばれるまで待つ
	const ResultUI::Choice choice = m_spResultUI->GetChoice();

	if (choice == ResultUI::Choice::None)
	{
		return;
	}

	m_isNextSceneRequested = true;

	if (choice == ResultUI::Choice::Retry)
	{
		// 同じシーンでも、新しく作り直される (前の状態は ResetSystems() で消える)
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Game);
	}
	else
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
	}
}

void ResultScene::Init()
{
	// リザルトのBGMを流す(シーンが切り替わるときにBGMManager::Reset()で解除される)
	BGMManager::Instance().SetOverride(BGMManager::BGMType::Result);

	// リザルトの画面 (クリアタイムは、ゲームシーンが GameResult に記録したものを読む)
	m_spResultUI = std::make_shared<ResultUI>();
	m_spResultUI->Init();

	AddObject(m_spResultUI);
}
