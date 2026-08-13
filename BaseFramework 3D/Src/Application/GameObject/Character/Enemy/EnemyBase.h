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

	void DrawInspector()override;


	void SetUpReference()override;

	/*void ChangeMoveState(MoveState nextState);*/

protected:

	MoveState m_moveState = MoveState::DirectChase;

	std::weak_ptr<Player>m_wpPlayer;

private:


};