#include "PlayerDieState.h"

#include"../../Player.h"

#include"../../../../../Scene/SceneManager.h"
#include"../../../../../Scene/GameScene/GameScene.h"

void PlayerDieState::OnStart(Player* owner)
{
	m_isNotified = false;

	owner->PlayAnimation(PlayerAnimationType::Die);

	KdAudioManager::Instance().Play("Asset/Data/Sound/SE/Player/Death/Death.wav");
}

void PlayerDieState::OnUpdate(Player * owner)
{
	if (owner->IsAnimationFinished())
	{
		owner->PlayAnimation(PlayerAnimationType::DieStay);

		// 死亡モーションが終わったら、ゲームオーバーを知らせる (1回だけ)
		// エディタのPlayではGameSceneではないので、何も起きない
		if (!m_isNotified)
		{
			m_isNotified = true;

			const auto gameScene = SceneManager::Instance().GetCurrentScene<GameScene>();

			if (gameScene)
			{
				gameScene->NotifyGameOver();
			}
		}
	}
}

void PlayerDieState::OnExit(Player * owner)
{

}
