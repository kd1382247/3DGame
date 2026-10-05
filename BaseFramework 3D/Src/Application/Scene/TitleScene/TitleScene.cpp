#include "TitleScene.h"

#include"../../GameObject/Title/Title.h"

void TitleScene::Event()
{

}

void TitleScene::Init()
{
	std::shared_ptr<Title>title = std::make_shared<Title>();
	title->Init();

	AddObject(title);
}
