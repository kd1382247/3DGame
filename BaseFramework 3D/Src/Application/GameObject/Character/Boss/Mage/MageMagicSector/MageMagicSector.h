#pragma once

class Player;

#include"../ColorSphereIndicator/ColorSphereIndicator.h"
#include"MageMagicSectorDesc.h"

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

	// 扇形攻撃の設定
	void Setup(const MageMagicSectorDesc& desc);

	bool IsFinished()const { return m_isFinished; }

	// 攻撃を取り消す(予告の表示とエフェクトを消して、オブジェクトも消す)
	void Cancel();

private:


	// 攻撃範囲・攻撃予測の表示を更新する
	// (当たり判定と同じ座標・半径を使うことで、見た目と実際の範囲を一致させる)
	void UpdateIndicators();

	std::weak_ptr<Player> m_wpPlayer;

	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

	// 再生中のエフェクトのハンドル(未再生の間は-1)
	Effekseer::Handle m_effectHandle = -1;

	// 扇形攻撃の設定(Setupで受け取る。dirは正規化済み)
	MageMagicSectorDesc m_desc = {};

	// エフェクトを再生する基準座標(Setupで求める)
	Math::Vector3 m_effectPos = {};

	bool       m_isFinished = false;

	// 発動までの残り予備動作時間(秒)
	float m_telegraphTime = 0.0f;

	// 当たり判定が出るまでの残り時間(秒。予告が終わってエフェクトが出てから減り始める)
	float m_hitDelayTime = 0.0f;

	bool m_hitTarget = false;

	// 当たり判定の有効時間(秒)
	float m_hitboxActiveTime = 1.0f;

	// 攻撃範囲用のカラースフィア(予告が終わったあとも、エフェクトが終わるまで表示する)
	ColorSphereIndicator m_rangeIndicator;

	// 攻撃予測用のカラースフィア(発動に近づくにつれて、範囲いっぱいまで広がっていく)
	ColorSphereIndicator m_telegraphIndicator;

	bool m_isPlaySE = false;
};
