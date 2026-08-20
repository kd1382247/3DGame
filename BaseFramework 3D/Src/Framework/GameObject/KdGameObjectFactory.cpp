#include "KdGameObjectFactory.h"

// ファクトリーに登録するクラス

#include"../../Application/GameObject/Character/Player/Player.h"

#include"../../Application/GameObject/Camera/TPSCamera/TPSCamera.h"

// ウェイポイント
#include"../../Application/GameObject/WayPoint/WayPoint.h"

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

// ステージ
#include"../../Application/GameObject/Stage/Stage01/Render/Stage01.h"

// 当たり判定(壁)
#include"../../Application/GameObject/Stage/Stage01/Collision/WallCollision/WallCollision.h"



void KdGameObjectFactory::Init()
{
	//===================================================================
	// カメラ
	//===================================================================

	Register<TPSCamera>("TPSCamera", KdGameObject::ObjectCategory::Camera);

	//===================================================================
	// プレイヤー
	//===================================================================

	// プレイヤー
	Register<Player>("Player",KdGameObject::ObjectCategory::Character);
	
	//===================================================================
	// 敵
	//===================================================================

	// Beholder
	Register<Beholder>("Beholder", KdGameObject::ObjectCategory::Character);
	// Bomb
	Register<Bomb>("Bomb", KdGameObject::ObjectCategory::Character);
	// Cactas
	Register<Cactas>("Cactas", KdGameObject::ObjectCategory::Character);
	// ChestMonster
	Register<ChestMonster>("ChestMonster", KdGameObject::ObjectCategory::Character);
	// Golem
	Register<Golem>("Golem", KdGameObject::ObjectCategory::Character);
	// Mage
	Register<Mage>("Mage", KdGameObject::ObjectCategory::Character);
	// Mushroom
	Register<Mushroom>("Mushroom", KdGameObject::ObjectCategory::Character);
	// Slime
	Register<Slime>("Slime", KdGameObject::ObjectCategory::Character);
	// StarFish
	Register<StarFish>("StarFish", KdGameObject::ObjectCategory::Character);
	// Swarm
	Register<Swarm>("Swarm", KdGameObject::ObjectCategory::Character);
	// TurtleShell
	Register<TurtleShell>("TurtleShell", KdGameObject::ObjectCategory::Character);

	//===================================================================
	// ステージ
	//===================================================================

	// Stage01
	Register<Stage01>("Stage01", KdGameObject::ObjectCategory::Stage);


	//===================================================================
	// 当たり判定
	//===================================================================

	// 壁
	Register<WallCollision>("WallCollision", KdGameObject::ObjectCategory::None);

	//===================================================================
	// ウェイポイント
	//===================================================================
	Register<WayPoint>("WayPoint", KdGameObject::ObjectCategory::None);

}

void KdGameObjectFactory::RegisterCreateFunction(const std::string_view str, const CreateFunction func, const KdGameObject::ObjectCategory category)
{
	m_createFunctions[str.data()] = { func,category };
}


std::shared_ptr<KdGameObject> KdGameObjectFactory::CreateGameObject(const std::string& objName) const
{
	// 登録されているクラス名から生成情報を検索
	auto entryIt = m_createFunctions.find(objName);

	// 未登録のクラス名だった場合
	if (entryIt == m_createFunctions.end())
	{
		assert(0 && "GameObjectFactoryに未登録のゲームオブジェクトクラスです");

		return nullptr;
	}

	// 登録されている生成関数を実行
	return  entryIt->second.createFunc();
}
