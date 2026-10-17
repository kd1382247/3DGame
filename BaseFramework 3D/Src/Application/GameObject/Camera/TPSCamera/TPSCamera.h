#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:

	enum class BossIntroState
	{
		None,
		Approach,
		Hold,
		Roar,
		Return
	};

	struct BossIntroParam
	{
		float distance       = 5.0f;  // ボスからカメラまでの距離
		float height         = 2.0f;  // カメラの高さ(ボスの座標から)
		float lookHeight     = 2.0f;  // 見る点の高さ(ボスの座標から)
		float approachSpeed  = 0.04f; // 近づく速さ
		float returnSpeed    = 0.06f; // 戻る速さ
		float shakeAmplitude = 0.3f;  // カメラの揺れの大きさ
		float shakeDuration  = 4.0f;  // 揺れの時間(秒)
		float shakeFrequency = 8.0f;  // 揺れる速さ
	};

	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

	void StartBossIntro(const Math::Vector3& pos, const BossIntroParam& param);

	// 咆哮の合図
	void StartRoar()
	{
		// Holdの時だけ受け付ける
		if (m_introState == BossIntroState::Hold)
		{
			m_introState = BossIntroState::Roar;
		}
	}
	// introStateがNone以外ならtrueを返す
	bool IsBossIntroPlaying()const { return m_introState != BossIntroState::None; }

	bool IsBossIntroHold()const { return m_introState == BossIntroState::Hold; }


private:

	// ボスの登場演出
	void UpdateBossIntro();
	void ApplyBossIntro();

	// 揺れの量を返す
	float CalcShakeOffsest()const;

	Math::Quaternion CreateLookRotation(const Math::Vector3& camPos, const float lookHeight);


	// マウスでの回転とターゲットへの追従で、カメラのワールド行列を更新する
	void UpdateCameraMatrix(const std::shared_ptr<const KdGameObject>& spTarget);

	// プレイヤーからカメラへレイを飛ばし、壁の向こう側にいたらプレイヤー側へ引き寄せる
	void PullInByRay(const KdGameObject& target);

	// カメラ位置にスフィアを置き、壁に近すぎたら押し戻す
	void PushOutBySphere();

	// カメラとの距離
	float m_cameraDist = -1.0f;

	// 遅れて追う位置を覚える
	Math::Vector3 m_smoothTargetPos = {};

	bool          m_isSmoothTargetInit = false;


	// ボスの登場演出用変数

	BossIntroState m_introState = BossIntroState::None;
	BossIntroParam m_introParam = {};

	Math::Vector3    m_bossPos = {};
	Math::Vector3    m_introCamPos = {};
	Math::Quaternion m_introCamRot = {};

	// 通常のカメラから演出カメラへの混ぜ具合
	float            m_introBlend = 0.0f;
	// Roarに入ってからの経過時間(秒)
	float            m_shakeTime = 0.0f;

};
