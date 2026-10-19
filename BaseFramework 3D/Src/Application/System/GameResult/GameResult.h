#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// ゲームの結果 (ゲームシーンで記録して、リザルトシーンで表示する)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// シーンが切り替わっても消えない (SceneManager::ResetSystems() の対象にしていない)
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class GameResult
{
public:

	// クリアタイム (秒) : ゲームを始めてから、ボスを倒すまでの時間
	void  SetClearTime(const float time) { m_clearTime = time; }
	float GetClearTime()const { return m_clearTime; }

private:

	float m_clearTime = 0.0f;

private:

	GameResult() {}
	~GameResult() {}

public:

	static GameResult& Instance()
	{
		static GameResult instance;
		return instance;
	}
};
