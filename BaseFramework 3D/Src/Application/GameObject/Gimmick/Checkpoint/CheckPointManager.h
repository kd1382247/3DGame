#pragma once

#include<string>
#include<unordered_set>
#include<vector>

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// チェックポイントの記録 (シングルトン)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// ・到達した中で一番大きい順番と、その場所(ゲームオーバー後に、ここから再開する)を覚える
// ・到達した時点のプレイ時間と、クリア条件を満たした部屋も覚える (再開したとき、ゲームの状態を合わせるため)
// ・シーンを作り直しても消えない(リトライでチェックポイントから始められるように、シーンの外で持つ)
// ・最初から始めるとき(タイトルから)や、クリアしたときは、Reset()で消す
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class CheckPointManager
{
public:

	// 記録を全部消す
	void Reset();

	// チェックポイントに到達したことを記録する
	// 今の記録より大きい順番のときだけ記録して、trueを返す
	// (同じ順番や、小さい順番には記録しない。戻っても、順番が下がらない)
	bool TryReach(const int order, const Math::Vector3& pos);

	// 到達した時点の進行を記録する (TryReachがtrueのときに呼ぶ)
	// playTime        : そのときのプレイ時間(秒)
	// clearedRoomNames: クリア条件を満たしていた部屋の名前
	void SaveProgress(const float playTime, const std::vector<std::string>& clearedRoomNames);

	// 記録があるか (1つも到達していなければfalse)
	bool HasCheckPoint()const { return m_reachedOrder != kNoOrder; }

	// 到達した中で一番大きい順番 (記録がなければ-1)
	int GetReachedOrder()const { return m_reachedOrder; }

	// 再開する位置 (記録がなければ原点)
	const Math::Vector3& GetRespawnPos()const { return m_respawnPos; }

	// 再開するときのプレイ時間 (秒)
	float GetPlayTime()const { return m_playTime; }

	// 到達した時点で、クリア条件を満たしていた部屋か
	bool IsRoomCleared(const std::string& roomName)const { return m_clearedRoomNames.contains(roomName); }

private:

	// 記録がないときの順番
	static constexpr int kNoOrder = -1;

	// 到達した中で一番大きい順番
	int m_reachedOrder = kNoOrder;

	// 再開する位置
	Math::Vector3 m_respawnPos = Math::Vector3::Zero;

	// 到達した時点のプレイ時間 (秒)
	float m_playTime = 0.0f;

	// 到達した時点で、クリア条件を満たしていた部屋の名前
	std::unordered_set<std::string> m_clearedRoomNames;

private:

	CheckPointManager(){}
	~CheckPointManager(){}

public:

	static CheckPointManager& Instance()
	{
		static CheckPointManager instance;
		return instance;
	}

};
