#pragma once

#include"../CharacterBase.h"

class Player;

class WayPoint;

class EnemyBase :public CharacterBase
{
public:


	enum class MoveState
	{
		DirectChase,
		FollowPath
	};

	EnemyBase(){}
	~EnemyBase()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;


	void SetUpReference()override;

protected:


	MoveState m_currentMoveState = MoveState::DirectChase;
	MoveState m_nextMoveState = MoveState::DirectChase;


	std::weak_ptr<Player>m_wpPlayer;

	bool CanDirectChase();
	void CreatePath();

	void ChangeMoveState(const MoveState nextState);

	void UpdatePath();
	void SetPath(const std::vector<int>& path,const int goalID);

	// 敵が通るWayPointのID一覧
	std::vector<int>m_path;

	// 現在目指しているWayPointが、m_pathの何番目か
	size_t m_pathIndex = 0;

	int m_goalWayPointID = 0;


private:


};