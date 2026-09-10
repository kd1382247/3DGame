#pragma once

class TimeManager
{
public:

	void Init();
	void Update();

	float GetDeltaTime()               const { return m_deltaTime; }
	float GetUnscaleeDeltaTime()       const { return m_unscaledDeltaTime; }

	float GetTimeScale()               const { return m_timeScale; }
	void  SetTimeScale(const float scale)    { m_timeScale = scale; }

	bool  IsHitStop() const { return m_isHitStop; }
	void  StartHitStop(const float duration);
	void  UpdateHitStop();

private:

	float m_unscaledDeltaTime = 0.0f;
	float m_deltaTime = 0.0f;

	float m_timeScale = 1.0f;

	static constexpr float kMaxDeltaTime = 1.0f / 30.0f;

	// ヒットストップ関連
	float m_hitStopTimer = 0.0f;
	bool  m_isHitStop = false;

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