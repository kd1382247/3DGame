#pragma once

class Player;

// Mageのビーム攻撃の当たり判定専用オブジェクト
// (詠唱終了と同時に生成され、持続時間の間、一定間隔でダメージを与え続ける)
class MageBeam :public KdGameObject
{
public:

	MageBeam() {}
	~MageBeam()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// ビームの設定(発射座標・方向・長さ・幅・ダメージ・持続時間(秒))
	void Setup(const Math::Vector3& pos, const Math::Vector3& dir, float length, float width, float damage, float duration);

	void SetEffectInfo(const std::string& fileName, float size, float speed, float startFrame, float endFrame, const Math::Vector3& rotation,const Math::Vector3&pos);

	bool IsFinished()const { return m_isFinished; }

private:

	struct EffectInfo
	{
		std::string m_fileName   = {};
		Math::Vector3      m_pos = {};
		float       m_size       = 0.0f;
		float       m_speed      = 0.0f;
		float       m_startFrame = 0.0f;
		float       m_endFrame   = 0.0f;
		Math::Vector3 m_rotation = {};
	};

	struct Beam
	{
		Math::Vector3 m_dir    = {};
		float         m_length = 0.0f;
		float         m_width  = 0.0f;
	};


	std::weak_ptr<Player> m_wpPlayer;

	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

	// 再生中のエフェクトのハンドル(未再生の間は-1)
	Effekseer::Handle m_effectHandle = -1;

	EffectInfo m_effectInfo = {};

	Beam       m_beam = {};

	bool       m_isFinished = false;

	float m_damage = 0.0f;

	// ビームが有効な残り時間(秒)。0以下になったら消える
	float m_duration = 0.0f;

	// 再ヒットまでの残り時間(秒)。0以下の間にプレイヤーが範囲内にいればヒットする
	float m_hitCooldown = 0.0f;

};
