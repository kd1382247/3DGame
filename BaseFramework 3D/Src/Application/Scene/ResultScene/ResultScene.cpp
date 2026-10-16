#include "ResultScene.h"

#include"../SceneManager.h"
#include"../../BGMManager/BGMManager.h"

void ResultScene::Event()
{
	// 仮:Enterキーでタイトルに戻る(リザルトの操作ができたら置き換える)
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene(SceneManager::SceneType::Title);
	}
}

void ResultScene::Init()
{
	// リザルトのBGMを流す(シーンが切り替わるときにBGMManager::Reset()で解除される)
	BGMManager::Instance().SetOverride(BGMManager::BGMType::Result);

	// TODO: リザルトの表示物(スコア・ランクなど)をAddObjectで追加する
}
