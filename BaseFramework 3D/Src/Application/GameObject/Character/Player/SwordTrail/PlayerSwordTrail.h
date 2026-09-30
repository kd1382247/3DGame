#pragma once

class Player;

class PlayerSwordTrail
{
public:

	void Init();

	void StartTrail();					// 攻撃開始時に呼ぶ
	void UpdateTrail(Player& player);	// 毎フレーム呼ぶ
	void EndTrail();					// 攻撃終了時に呼ぶ

	// トレイルを表示するフレーム区間をセットする(剣を振っている間だけ表示したい時に調整する)
	void SetTrailTiming(float trailStart, float trailEnd) { m_trailStartFrame = trailStart; m_trailEndFrame = trailEnd; }

	std::shared_ptr<KdTrailPolygon>GetTrailPolygon()const { return m_spTrailPolygon; }

private:


	void AddPoint(const Math::Vector3& tipPos, const Math::Vector3& basePos);

	std::shared_ptr<KdTrailPolygon> m_spTrailPolygon = nullptr;

	bool m_isTracking = false;

	// トレイルを表示するフレーム区間(この区間の外にいる間はトレイルを伸ばさない)
	float m_trailStartFrame = 0.0f;
	float m_trailEndFrame = 0.0f;

	// Swordメッシュのバインドポーズ座標から出した、剣先とその少し手前の点（ローカル座標）
	const Math::Vector3 m_tipLocalPos = { 0.579f, 0.619f, 1.16f };
	const Math::Vector3 m_baseLocalPos = { 0.584f, 0.619f, 0.10f };

	Math::Vector3 m_tipHistory[4] = {};
	Math::Vector3 m_baseHistory[4] = {};
	int m_historyCount = 0;


};