#include "TimeManager.h"

#include"../../main.h"

void TimeManager::Init()
{

}

void TimeManager::Update()
{
	float rawDeltaTime = Application::Instance().GetDeltaTime();

	// rawDeltaTime と kMaxDeltaTime の
	// 小さい方をm_deltaTimeへ入れる
	m_deltaTime =std::min(rawDeltaTime, kMaxDeltaTime);
}
