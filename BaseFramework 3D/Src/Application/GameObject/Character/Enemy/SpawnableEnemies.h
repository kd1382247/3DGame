#pragma once

#include <array>
#include <string>

//==================================================================
// ステージ上で「生成させる対象」として選べる敵の名前一覧
// (KdGameObjectFactoryに登録しているクラス名と同じ文字列にする)
// EnemySpawner / ChestMonster など、敵を生成するクラスで共通して使う
//==================================================================
namespace SpawnableEnemies
{
	inline const std::array<std::string, 6> Names =
	{
		"Bomb", "Cactas", "Mushroom", "Slime", "StarFish", "TurtleShell"
	};

	// 先読みする敵のモデルのパス (GameScene が、シーンの開始時に読み込んでおく)
	// 各敵の Init() で InitCharacterModel() に渡しているパスと、同じ文字列にする
	// ・Mushroom は見た目が2種類あるので、2つ入れる
	// ・Slime は大きさが違っても同じモデルなので、1つで足りる
	// ・ChestMonster はステージに置く敵で、ステージの読み込みの時に読み込まれるので入れない
	// 敵を増やした時は、Names と、ここの両方に足す
	inline const std::array<std::string, 7> ModelPaths =
	{
		"Asset/Models/Enemy/Bomb/Bomb.gltf",
		"Asset/Models/Enemy/Cactas/Cactas.gltf",
		"Asset/Models/Enemy/Mushroom/MushroomAngry/MushroomAngry.gltf",
		"Asset/Models/Enemy/Mushroom/MushroomSmile/MushroomSmile.gltf",
		"Asset/Models/Enemy/Slime/Slime.gltf",
		"Asset/Models/Enemy/StarFish/StarFish.gltf",
		"Asset/Models/Enemy/TurtleShell/TurtleShell.gltf"
	};
}
