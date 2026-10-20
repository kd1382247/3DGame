#pragma once

#include"EnemySpawnerRequest.h"

// 「敵を出して」という合図を、ボスなどからスポナーへ届ける窓口
// (合図を出した場所にある部屋を探して、その部屋に紐づいたスポナーへ渡す)
class EnemySpawnerManager
{
public:

	// 合図を出す。requesterPosは、合図を出したもの(ボスなど)の座標
	// 渡せたスポナーがなければfalse
	bool RequestSpawn(const Math::Vector3& requesterPos, const EnemySpawnerRequest& request);

private:

	EnemySpawnerManager() {}
	~EnemySpawnerManager() {}

public:

	static EnemySpawnerManager& Instance()
	{
		static EnemySpawnerManager instance;
		return instance;
	}

};
