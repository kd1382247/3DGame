#pragma once

class TimeManager
{
public:

	void Init();
	void Update();

	float GetDeltaTime()               const { return m_deltaTime; }
	float GetUnscaleeDeltaTime()       const { return m_unscaledDeltaTime; }

	float GetTimeScale()               const { return m_timeScale; }


	bool  IsHitStop() const { return m_isHitStop; }
	void  StartHitStop(const float duration);
	void  UpdateHitStop();

	void  StartSlowMotion(const float scale, const float duration);
	void  UpdateSlowMotion();

private:
	
	void  UpdateTimeScale();


	float m_unscaledDeltaTime = 0.0f;
	float m_deltaTime = 0.0f;
	float m_timeScale = 1.0f;
	
	static constexpr float kMaxDeltaTime = 1.0f / 30.0f;

	// ヒットストップ関連
	float m_hitStopTimer = 0.0f;
	bool  m_isHitStop = false;

	// スローモーション関連
	float m_slowTimer    = 0.0f;  // 残り時間(秒、実時間)
	float m_maxSlowTimer = 0.0f;  // スローの全体の長さ
	float m_slowScale    = 1.0f;  // スローの倍率
	bool  m_isSlow       = false; // スロー中かのフラグ
	
	

private:

	TimeManager(){}
	~TimeManager(){}

public:

	static TimeManager& Instance()
	{
		static TimeManager instance;
		return instance;
	}

};