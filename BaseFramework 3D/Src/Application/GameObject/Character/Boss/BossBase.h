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

	void OnHit(const AttackInfo attackInfo)override;

private:


protected:

	// パラメータクラスのインスペクター描画。各Enemyが実装する
	virtual void DrawParameterInspector() = 0;

	// HPが最大値の半分以下かどうか(全ボス共通の第2フェーズ判定)
	bool IsSecondPhase()const;

	// 重み付き抽選。weights[i]が0以下の要素は選ばれない。戻り値は選ばれたweightsのindex
	int LotteryPattern(const std::vector<float>& weights)const;

	std::weak_ptr<Player>m_wpPlayer;



};