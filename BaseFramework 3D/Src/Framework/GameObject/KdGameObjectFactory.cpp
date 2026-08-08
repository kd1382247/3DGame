#include "KdGameObjectFactory.h"

// ファクトリーに登録するクラス
#include"../../Application/GameObject/Camera/TPSCamera/TPSCamera.h"

#include"../../Application/GameObject/Character/Player/Player.h"
#include"../../Application/GameObject/Terrains/Ground/Ground.h"

#include"../../Application/GameObject/Terrains/Box.h"

// ウェイポイント
#include"../../Application/System/PathFinding/WayPoint/WayPoint.h"

// 敵
#include"../../Application/GameObject/Character/Enemy/Beholder/Beholder.h"
#include"../../Application/GameObject//Character/Enemy/Bomb/Bomb.h"
#include"../../Application/GameObject//Character/Enemy/Cactas/Cactas.h"
#include"../../Application/GameObject//Character/Enemy/ChestMonster/ChestMonster.h"
#include"../../Application/GameObject//Character/Enemy/Golem/Golem.h"
#include"../../Application/GameObject//Character/Enemy/Mage/Mage.h"
#include"../../Application/GameObject//Character/Enemy/Mushroom/Mushroom.h"
#include"../../Application/GameObject//Character/Enemy/Slime/Slime.h"
#include"../../Application/GameObject//Character/Enemy/StarFish/StarFish.h"
#include"../../Application/GameObject//Character/Enemy/Swarm/Swarm.h"
#include"../../Application/GameObject//Character/Enemy/TurtleShell/TurtleShell.h"




void KdGameObjectFactory::Init()
{

	//===================================================================
	// プレイヤー
	//===================================================================

	// プレイヤー
	Register<Player>("Player");
	
	//===================================================================
	// 敵
	//===================================================================

	// Beholder
	Register<Beholder>("Beholder");
	// Bomb
	Register<Bomb>("Bomb");
	// Cactas
	Register<Cactas>("Cactas");
	// ChestMonster
	Register<ChestMonster>("ChestMonster");
	// Golem
	Register<Golem>("Golem");
	// Mage
	Register<Mage>("Mage");
	// Mushroom
	Register<Mushroom>("Mushroom");
	// Slime
	Register<Slime>("Slime");
	// StarFish
	Register<Mage>("StarFish");
	// Swarm
	Register<Swarm>("Swarm");
	// TurtleShell
	Register<TurtleShell>("TurtleShell");

	//===================================================================
	// ステージ
	//===================================================================

	// 地面
	Register<Ground>("Ground");
	// ボックス
	Register<Box>("Box");
	// 地面
	Register<TPSCamera>("TPSCamera");

	//===================================================================
	// ウェイポイント
	//===================================================================
	Register<WayPoint>("WayPoint");

}

void KdGameObjectFactory::RegisterCreateFunction(const std::string_view str, const std::function<std::shared_ptr<KdGameObject>(void)> func)
{
	m_createFunctions[str.data()] = func;
}

std::shared_ptr<KdGameObject> KdGameObjectFactory::CreateGameObject(const std::string objName) const
{
	auto creater = m_createFunctions.find(objName);

	if (creater == m_createFunctions.end())
	{
		assert(0 && "GameObjectFactoryに未登録のゲームオブジェクトクラスです");

		return nullptr;
	}

	return  creater->second();
}

// リスト内の最適化（寿命の尽きたオブジェクトの解放）
void KdGameObjectFactory::RemoveExpiredObjects()
{
	for (auto objIter = m_objects.begin(); objIter != m_objects.end();)
	{
		if ((*objIter)->IsExpired())
		{
			objIter = m_objects.erase(objIter);

			continue;
		}

		++objIter;
	}
}