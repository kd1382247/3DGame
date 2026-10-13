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
}
