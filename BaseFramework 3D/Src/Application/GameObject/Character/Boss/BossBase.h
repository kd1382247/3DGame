#pragma once

#include"../../Character/CharacterBase.h"

class Player;

class BossBase :public CharacterBase
{

public:

	void Init()override;

	void PostUpdate()override;
	void SetUpReference()override;

	void DrawInspector()override;

private:


protected:

	// パラメータクラスのインスペクター描画。各Enemyが実装する
	virtual void DrawParameterInspector() = 0;

	std::weak_ptr<Player>m_wpPlayer;



};