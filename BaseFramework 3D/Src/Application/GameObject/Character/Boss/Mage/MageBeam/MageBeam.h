#pragma once

class Player;

#include"MageBeamDesc.h"

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

	// ビームの設定
	void Setup(const MageBeamDesc& desc);

	bool IsFinished()const { return m_isFinished; }

	// 攻撃を取り消す(エフェクトを止めて、オブジェクトも消す)
	void Cancel();

private:


	std::weak_ptr<Player> m_wpPlayer;

	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

	// 再生中のエフェクトのハンドル(未再生の間は-1)
	Effekseer::Handle m_effectHandle = -1;

	MageBeamDesc m_desc = {};

	bool       m_isFinished = false;

	// ビームが有効な残り時間(秒)。0以下になったら消える
	float m_remainingTime = 0.0f;

	// 再ヒットまでの残り時間(秒)。0以下の間にプレイヤーが範囲内にいればヒットする
	float m_hitCooldown = 0.0f;

};
