#pragma once

#include"../CharacterBase.h"

class Player;

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

	void DrawInspecter()override;

	void SetPlayer(std::shared_ptr<Player>_player)
	{
		m_wpPlayer = _player;
	}


protected:

	MoveState m_moveState = MoveState::FollowPath;

	std::weak_ptr<Player>m_wpPlayer;

private:


};