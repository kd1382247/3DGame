#pragma once

struct EnemySpawnerRequest
{
	int   count    = 1;    //生成する数
	float interval = 0.2f; // 1体ごとの生成の間隔
};