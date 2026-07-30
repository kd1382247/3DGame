#include "GameObjectFactory.h"

#include"../../Scene/SceneManager.h"

// ファクトリーに登録するクラス
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../GameObject/Character/Player/Player.h"
#include"../../GameObject/Terrains/Ground/Ground.h"



void GameObjectFactory::Init()
{
	AddClassFactory();
}

void GameObjectFactory::AddClassFactory()
{
	// プレイヤー
	m_gameObjectFactory.RegisterCreateFunction
	(
		"Player", []() {return std::make_shared<Player>(); }
	);

	// 地面
	m_gameObjectFactory.RegisterCreateFunction
	(
		"Ground", []() {return std::make_shared<Ground>(); }
	);

	// 地面
	m_gameObjectFactory.RegisterCreateFunction
	(
		"TPSCamera", []() {return std::make_shared<TPSCamera>(); }
	);

}

std::shared_ptr<KdGameObject> GameObjectFactory::CreateObject(const std::string_view className)
{

	auto newObject =
		m_gameObjectFactory.CreateGameObject(className);

	if (!newObject)
	{
		OutputDebugStringA(
			"オブジェクトの生成に失敗しました\n");

		return nullptr;
	}

	newObject->Init();

	SceneManager::Instance().AddObject(newObject);

	return newObject;

}
