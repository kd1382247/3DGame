#pragma once

class Mage;

class MageBeam;

#include"../../../../StateMachine/StateBase.h"


class MageBeamState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	// 当たり判定と同じ座標・向き・サイズを使うことで、見た目と実際の範囲を一致させる
	void CreateBeamRange();
	void UpdateBeamRange();
	void HideBeamRange();


	// 攻撃が発生するタイミングを表す
	void CreateTelegraphIndicator();
	void UpdateTelegraphIndicator();
	void HideTelegraphIndicator();

	// 詠唱開始時に確定させる、Beamの発射座標(ボスの前方へオフセット済み)・方向
	// (テレグラフ表示・実際のBeam生成の両方で同じ値を使うことで見た目と実際の範囲を一致させる)
	Math::Vector3 m_beamPos = {};
	Math::Vector3 m_beamDir = {};

	// カラースフィア(矩形)のスロット番号保存用
	int m_colorSphereHandle = -1;

	int m_telegraphIndicatorHandle = -1;

	// 詠唱中に再生する予告(windup)エフェクトのハンドル
	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

	// 生成したBeam本体への参照(これが無くなる/終了したらNormalStateへ戻る)
	std::weak_ptr<MageBeam> m_wpMageBeam;

	Math::Vector3 m_effectPos = Math::Vector3::Zero;

	float m_castTimer = 0.0f;
	bool  m_hasCast = false;

};
