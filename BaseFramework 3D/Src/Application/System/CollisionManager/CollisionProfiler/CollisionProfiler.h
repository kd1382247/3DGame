#pragma once

class CollisionProfiler
{
public:

	CollisionProfiler(){}
	~CollisionProfiler(){}
	
	enum class Section
	{
		Total,
		Movement,
		Character,
		Pair,
		Push,
		Max
	};

	enum class Counter
	{
		PushCount,
		HitCount,
		Max
	};

	// Sectionの区間に時間を足す
	void AddTime(const Section& section, const float ms);

	// 回数を足す
	void AddCount(const Counter& counter, const int n = 1);

	// フレーム数を増やし60に達したらログを出す
	void EndFrame();


	class ScopedTimer
	{
	public:

		ScopedTimer(CollisionProfiler& profiler,const CollisionProfiler::Section& section)
			:m_profiler(profiler),m_section(section),m_startTime(Clock::now()){}

		using Clock = std::chrono::steady_clock;
		using Ms = std::chrono::duration<float, std::milli>;

		~ScopedTimer()
		{
			// 現在の区間にms加算
			m_profiler.AddTime(m_section, Ms(Clock::now() - m_startTime).count());
		}

	private:

		// 計測始めの時間
		Clock::time_point          m_startTime = {};

		CollisionProfiler::Section m_section = {};

		CollisionProfiler& m_profiler;

	};

private:

	void ClearProfiler();

	float CalcAverageTime(const CollisionProfiler::Section& section);
	float CalcAverageCount(const CollisionProfiler::Counter& counter);

	static constexpr size_t SectionSize = static_cast<size_t>(Section::Max);
	static constexpr size_t CounterSize = static_cast<size_t>(Counter::Max);

	std::array<float, SectionSize> m_sectionList = {};
	std::array<int,   CounterSize> m_counterList = {};

	static constexpr int MaxFrame = 60;
	int m_frame = 0;
	
};
