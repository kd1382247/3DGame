#include "CollisionProfiler.h"

void CollisionProfiler::AddTime(const Section& section, const float ms)
{
	size_t index = static_cast<size_t>(section);

	m_sectionList[index] += ms;

}

void CollisionProfiler::AddCount(const Counter & counter, const int n)
{
	size_t index = static_cast<size_t>(counter);

	m_counterList[index] += n;
}

void CollisionProfiler::EndFrame()
{
	m_frame++;

	if (m_frame >= MaxFrame)
	{
		KdDebugGUI::Instance().ClearLog();
		KdDebugGUI::Instance().AddLog(
			"total:%.2f move:%.2f\nchara:%.2f (pair:%.2f push:%.2f)\n  pushCount:%.1f\n hitCount:%.1f\n",
			CalcAverageTime (Section::Total),
			CalcAverageTime (Section::Movement),
			CalcAverageTime (Section::Character),
			CalcAverageTime (Section::Pair),
			CalcAverageTime (Section::Push),
			CalcAverageCount(Counter::PushCount),
			CalcAverageCount(Counter::HitCount)
		);

		ClearProfiler();
	}

}

void CollisionProfiler::ClearProfiler()
{
	m_frame = 0;

	m_sectionList.fill(0);
	m_counterList.fill(0);

}

float CollisionProfiler::CalcAverageTime(const CollisionProfiler::Section& section)
{
	return m_sectionList[static_cast<size_t>(section)] / m_frame;
}

float CollisionProfiler::CalcAverageCount(const CollisionProfiler::Counter& counter)
{
	return m_counterList[static_cast<size_t>(counter)] / static_cast<float>(m_frame);
}
