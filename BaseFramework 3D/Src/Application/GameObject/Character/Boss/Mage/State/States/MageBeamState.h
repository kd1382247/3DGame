#pragma once

class Mage;

class MageBeam;

#include"../../../../StateMachine/StateBase.h"

#include"../../ColorSphereIndicator/ColorSphereIndicator.h"
#include"../../Parameter/MageParameter.h"

class MageBeamState :public StateBase<Mage>
{
public:

	void OnStart(Mage* mage)override;
	void OnUpdate(Mage* mage)override;
	void OnExit(Mage* mage)override;

private:

	// ビーム攻撃の段階
	enum class Phase
	{
		Cast,    // 詠唱中(予告が広がっていく)
		Active,  // ビーム発射後(ビームが終わるのを待つ)
		End      // 撃ち終わった後のアニメーション
	};

	// 詠唱中の更新(詠唱時間が経ったらStartActiveへ進む)
	void UpdateCast(Mage* mage);

	// ビームを撃って、発射後の段階へ進む
	void StartActive(Mage* mage);

	// 発射後の更新(ビームが終わったらNormalStateへ戻る)
	void UpdateActive(Mage* mage);

	void UpdateEnd(Mage* mage);

	Phase m_phase = Phase::Cast;

	// 詠唱開始時にParameterから受け取る設定(詠唱中・発射時は、Inspectorで値を変えても同じ値を使い続ける)
	MageParameter::Beam m_param = {};

	// 詠唱開始時に確定させる、Beamの発射座標(ボスの前方へオフセット済み)・方向
	// (テレグラフ表示・実際のBeam生成の両方で同じ値を使うことで見た目と実際の範囲を一致させる)
	Math::Vector3 m_beamPos = {};
	Math::Vector3 m_beamDir = {};

	// 攻撃範囲用のカラースフィア(詠唱中・発射後を通して表示する)
	ColorSphereIndicator m_rangeIndicator;

	// 攻撃予測用のカラースフィア(詠唱中だけ、範囲いっぱいまで広がっていく)
	ColorSphereIndicator m_telegraphIndicator;

	// 詠唱中に再生する予告(windup)エフェクトのハンドル
	std::weak_ptr<KdEffekseerObject> m_wpEffekseerObj;

	// 生成したBeam本体への参照(これが無くなる/終了したらNormalStateへ戻る)
	std::weak_ptr<MageBeam> m_wpMageBeam;

	float m_castTimer = 0.0f;

};
