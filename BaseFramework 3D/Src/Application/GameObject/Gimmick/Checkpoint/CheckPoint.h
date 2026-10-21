#pragma once

#include"../../../Editor/EditorManager.h"

class Player;

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// チェックポイント (エディタで置いて、プレイヤーが範囲に入ると、その場所を記録する)
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// ・部屋(Room)やステージのクリアには関係せず、プレイヤーとの距離だけで決まる
// ・Gimmickとは別のカテゴリ(ObjectCategory::CheckPoint)で、Hierarchyも別の一覧に出る
// ・ステージには属さない(StageGimmickを継承しない)。ステージの持ち主も、追従もない
//   StageData.json には、他のオブジェクトと同じように、位置などと一緒にそのまま保存される
// ・順番(Order)が今の記録より大きいときだけ、記録を更新して、プレイヤーのHPを全回復する (CheckPointManager)
//   (同じチェックポイントで、2回回復することはない)
// ・記録するとき、プレイ時間と、クリア条件を満たした部屋も一緒に覚える (リトライで、この時点の状態に戻すため)
// ・デバッグ線は2つの球 : 内側の球(kRadius)と、範囲の球(m_radius)。範囲の球に触れると発動する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class CheckPoint :public KdGameObject
{
public:

	CheckPoint(){}
	~CheckPoint()override{}

	void Init()override;
	void Update()override;

	void DrawDebug()override;

	void DrawInspectorHeader()override;
	void DrawInspector()override;

	void SetUpReference()override;

	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json)override;

	// チェックポイントの順番
	int GetOrder()const { return m_order; }

	// 範囲の半径(この球にプレイヤーが触れると発動する)
	float GetRadius()const { return m_radius; }

	// 内側の球の半径(エディタでの目印・クリック選択用)
	float GetCoreRadius()const { return kRadius; }

	// 到達済みか (今の記録の順番以下なら、到達済みとみなす)
	bool IsReached()const { return m_isReached; }

private:

	// デバッグの線を出すか(DrawDebugの最初に呼ぶ)
	// PlayMode : Debugチェックに従う
	// EditMode : DebugチェックがONか、選択中のときに出す
	bool ShouldDrawDebug();

	// 到達した時点の、プレイ時間とクリア条件を満たした部屋を、CheckPointManagerに覚えさせる
	void SaveProgress()const;

	// 新しく置いたときの、範囲の半径
	static constexpr float kDefaultRadius = 3.0f;

	// チェックポイントの順番
	int m_order = 0;

	// 範囲の半径
	float m_radius = kDefaultRadius;

	// 中心の半径(エディタ用)
	const float kRadius = 0.5f;

	// 到達済みか(デバッグ線の色を変えるために覚えておく)
	bool m_isReached = false;

	// プレイヤー(範囲に入ったかの判定と、回復に使う)
	std::weak_ptr<Player> m_wpPlayer;
};
