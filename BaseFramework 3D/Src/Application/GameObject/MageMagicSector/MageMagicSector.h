#pragma once

class Player;

// Mageの魔法円攻撃の当たり判定専用オブジェクト
// (発生してから一定時間(予備動作)経過後に一度だけ判定し、消える)
class MageMagicSector :public KdGameObject
{
public:

	MageMagicSector() {}
	~MageMagicSector()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 魔法円の設定(座標・半径・発動までの予備動作時間(秒)・ダメージ・)
	void Setup(const Math::Vector3& pos,const Math::Vector3& dir,float angleDeg, float radius, float telegraphTime, float damage);

	void SetEffectInfo(const std::string& fileName, float size, float speed, float startFrame, float endFrame);

private:


	struct EffectInfo
	{
		std::string m_fileName = {};
		Math::Vector3 m_pos = {};
		float       m_size = 0.0f;
		float       m_speed = 0.0f;
		float       m_startFrame = 0.0f;
		float       m_endFrame = 0.0f;
	};


	struct Sector
	{
		Math::Vector3 dir = {};
		float         m_angleDeg = 0.0f;
		float         m_radius=0.0f;
	};

	// 当たり判定と同じ座標・半径を使うことで、見た目と実際の範囲を一致させる
	void CreateMagicSectorRange();
	void UpdateMagicSectorRange();
	void HideMagicSectorRange();

	// 攻撃が発生するタイミングを表す
	void CreateTelegraphIndicator();
	void UpdateTelegraphIndicator();
	void HideTelegraphIndicator();

	std::weak_ptr<Player> m_wpPlayer;

	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

	// 再生中のエフェクトのハンドル(未再生の間は-1)
	Effekseer::Handle m_effectHandle = -1;

	EffectInfo m_effectInfo = {};

	Sector     m_sector = {};

	// 発動までの残り予備動作時間(秒)
	float m_telegraphTime = 0.0f;

	// 発動までの時間を保持
	float m_telegraphTimeTotal = 0.0f;

	float m_damage = 0.0f;

	bool m_hitTarget = false;

	// 当たり判定の有効時間(秒)
	float m_hitboxActiveTime = 1.0f;

	// カラースフィアのスロット番号保存用
	int m_colorSphereHandle = -1;

	int m_telegraphIndicatorHandle = -1;
};
