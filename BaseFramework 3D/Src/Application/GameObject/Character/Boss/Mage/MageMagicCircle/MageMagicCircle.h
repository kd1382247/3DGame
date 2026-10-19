#pragma once

class Player;

#include"../ColorSphereIndicator/ColorSphereIndicator.h"
#include"MageMagicCircleDesc.h"

// Mageの魔法円攻撃の当たり判定専用オブジェクト
// (発生してから一定時間(予備動作)経過後に一度だけ判定し、消える)
class MageMagicCircle :public KdGameObject
{
public:

	MageMagicCircle() {}
	~MageMagicCircle()override {}

	void Init()override;
	void Update()override;

	void SetUpReference()override;

	// 魔法円の設定
	void Setup(const MageMagicCircleDesc& desc);

	// 攻撃が終わった(エフェクトが終わった、または取り消された)かどうか
	bool IsFinished()const { return IsExpired(); }

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

	// 魔法円の設定(Setupで受け取る)
	MageMagicCircleDesc m_desc = {};

	// 発動までの残り予備動作時間(秒)
	float m_telegraphTime = 0.0f;

	bool m_hitTarget = false;

	// 当たり判定の有効時間(秒)
	float m_hitboxActiveTime = 1.0f;

	// 攻撃範囲用のカラースフィア(予告が終わったあとも、エフェクトが終わるまで表示する)
	ColorSphereIndicator m_rangeIndicator;

	// 攻撃予測用のカラースフィア(発動に近づくにつれて、範囲いっぱいまで広がっていく)
	ColorSphereIndicator m_telegraphIndicator;
};
