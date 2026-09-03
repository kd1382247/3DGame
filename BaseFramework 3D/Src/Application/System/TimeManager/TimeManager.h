#pragma once

class TimeManager
{
public:

	void Init();
	void Update();

	float GetDeltaTime()const { return m_deltaTime; }


private:

	// 前フレームから経過した秒数
	float m_deltaTime = 0.0f;

	static constexpr float kMaxDeltaTime = 1.0f / 30.0f;

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