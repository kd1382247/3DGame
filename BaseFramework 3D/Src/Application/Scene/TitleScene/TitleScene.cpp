#include "TitleScene.h"

#include"../../GameObject/Title/Title.h"
#include"../Setting/Setting.h"
#include"../../BGMManager/BGMManager.h"

void TitleScene::Event()
{

}

void TitleScene::Init()
{
	// タイトルのBGMを流す(シーンが切り替わるときにBGMManager::Reset()で解除される)
	BGMManager::Instance().SetOverride(BGMManager::BGMType::Title);

	// 設定画面 (音量調節)
	std::shared_ptr<Setting>setting = std::make_shared<Setting>();
	setting->Init();

	std::shared_ptr<Title>title = std::make_shared<Title>();
	title->Init();
	title->SetSetting(setting);

	AddObject(title);

	// Titleより後に追加する (オブジェクトリストの順に描画されるので、Titleの上に重なる)
	AddObject(setting);
}
