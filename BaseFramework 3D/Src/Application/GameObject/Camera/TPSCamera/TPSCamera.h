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
		Stagger,
		Return,
		Stay	// 撃破演出が終わった後も、ボスを映し続ける
	};

	struct BossIntroParam
	{
		float distance       = 5.0f;  // ボスからカメラまでの距離
		float height         = 2.0f;  // カメラの高さ(ボスの座標から)
		float lookHeight     = 2.0f;  // 見る点の高さ(ボスの座標から)
		float approachSpeed  = 0.04f; // 近づく速さ(撃破演出では、ボスの方を向く速さ)
		float returnSpeed    = 0.06f; // 戻る速さ
		float shakeAmplitude = 0.3f;  // カメラの揺れの大きさ
		float shakeDuration  = 4.0f;  // 揺れの時間(秒)
		float shakeFrequency = 8.0f;  // 揺れる速さ

		// 撃破演出用
		float swayAngle             = 6.0f; // ボスを見たまま、左右に振れる角度(度)
		float swayFrequency         = 0.25f; // 左右に振れる速さ(1秒に何往復か)
		float staggerShakeAmplitude = 0.2f; // 小刻みな揺れの大きさ
		float staggerShakeFrequency = 8.0f; // 小刻みな揺れの速さ(1秒に何往復か)
		float staggerShakeInterval  = 0.8f;  // 小刻みな揺れが始まる間隔(秒)
		float staggerShakeBurst     = 0.4f;  // 1回の小刻みな揺れの長さ(秒)
		float staggerDuration       = 2.5f;  // 揺れ(回り込み・小刻みな揺れ)が続く時間(秒)。過ぎると揺れは止まる
		float staggerFadeTime       = 1.0f;  // 揺れが止まる前に、少しずつ弱める時間(秒)
	};

	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;

	void StartBossIntro(const Math::Vector3& pos, const BossIntroParam& param);

	// ボスの撃破演出(ボスの座標を渡す)
	// カメラはプレイヤーの後ろに付いたまま、ボスの方を向き続ける(プレイヤーが動いてもボスを映す)
	// EndBossDefeat()が呼ばれても戻らず、ボスを映し続ける
	void StartBossDefeat(const Math::Vector3& pos, const BossIntroParam& param);

	// 撃破演出の終了の合図(ボスが完全に消えたら呼ぶ)
	// カメラはプレイヤーには戻らず、ボスを映したまま留まる(演出再生中ではなくなる)
	void EndBossDefeat()
	{
		// 撃破演出の、近づき中かよろけ中の時だけ受け付ける
		if (m_isDefeat &&
			(m_introState == BossIntroState::Approach || m_introState == BossIntroState::Stagger))
		{
			m_introState = BossIntroState::Stay;
		}
	}

	// 咆哮の合図
	void StartRoar()
	{
		// Holdの時だけ受け付ける
		if (m_introState == BossIntroState::Hold)
		{
			m_introState = BossIntroState::Roar;
		}
	}
	// 演出の再生中ならtrueを返す(NoneとStay(撃破演出の後)の時はfalse)
	bool IsBossIntroPlaying()const { return m_introState != BossIntroState::None && m_introState != BossIntroState::Stay; }

	bool IsBossIntroHold()const { return m_introState == BossIntroState::Hold; }

	void SetIsTargetGrounded(const bool flg) { m_isTargetGrounded=flg; }

	void SetFovWide(const bool flg) { m_isFovWide = flg; }

private:

	// ボスの登場演出
	void UpdateBossIntro();
	void ApplyBossIntro();

	// 揺れの量を返す
	float CalcShakeOffsest()const;

	// 撃破演出：ボスの方向から、左右にずらす角度(ラジアン)を返す
	float CalcSwayAngle()const;
	// 撃破演出：小刻みな揺れの量(カメラの左右方向)を返す
	float CalcStaggerShake()const;
	// 撃破演出：揺れの強さの倍率(1.0 〜 0.0)。staggerDurationが近づくと0へ向かう
	float CalcStaggerFade()const;

	// 撃破演出の更新(ボスの方を向く、揺れの時間を進める)
	void UpdateBossDefeat(const float deltaTime);
	// 撃破演出：カメラの向き(Y回転)を、ボスの方向へ近づける。向ききったらtrueを返す
	bool TurnToBoss();
	// 撃破演出：小刻みな揺れを、カメラの位置に足す
	void ApplyDefeatShake();

	Math::Quaternion CreateLookRotation(const Math::Vector3& camPos, const float lookHeight);

	// マウスでの回転とターゲットへの追従で、カメラのワールド行列を更新する
	void UpdateCameraMatrix(const std::shared_ptr<const KdGameObject>& spTarget);

	// プレイヤーからカメラへレイを飛ばし、壁の向こう側にいたらプレイヤー側へ引き寄せる
	void PullInByRay(const KdGameObject& target);

	// カメラ位置にスフィアを置き、壁に近すぎたら押し戻す
	void PushOutBySphere();

	// FOVを更新
	void UpdateFov();


	// カメラとの距離
	float m_cameraDist = -1.0f;

	// 遅れて追う位置を覚える
	Math::Vector3 m_smoothTargetPos = {};

	bool          m_isSmoothTargetInit = false;

	// 追従する対象が地面に着いているか
	bool          m_isTargetGrounded = true;

	// 今のFOV(初期値は元のFOVを入れる)
	float         m_currentFov = 0.0f;

	bool          m_isFovWide = false;


	// ボスの登場演出用変数
	BossIntroState m_introState = BossIntroState::None;
	BossIntroParam m_introParam = {};

	Math::Vector3    m_bossPos = {};
	Math::Vector3    m_introCamPos = {};
	Math::Quaternion m_introCamRot = {};

	// 通常のカメラから演出カメラへの混ぜ具合
	float            m_introBlend = 0.0f;
	// Roar / Stagger に入ってからの経過時間(秒)
	float            m_shakeTime = 0.0f;

	// 撃破演出ならtrue(咆哮の登場演出ならfalse)
	bool             m_isDefeat = false;

};
