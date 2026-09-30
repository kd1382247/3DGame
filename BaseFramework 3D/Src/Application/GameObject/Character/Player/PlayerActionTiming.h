#pragma once

// 攻撃系アクション(通常攻撃・必殺技)のフレーム指定
// すべて「アニメーションの経過フレーム(60fps換算)」で指定する
struct PlayerActionTiming
{
	// 攻撃判定が出るフレーム区間
	float hitStart = 0.0f;
	float hitEnd = 0.0f;

	// トレイルポリゴンを表示するフレーム区間(剣を振っている間だけ表示したい時に調整する)
	float trailStart = 0.0f;
	float trailEnd = 0.0f;
};
