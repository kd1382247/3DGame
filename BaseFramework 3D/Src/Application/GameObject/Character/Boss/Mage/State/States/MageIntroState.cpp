#include "MageIntroState.h"

#include"../../Mage.h"
#include"MageNormalState.h"

#include"../../../../../Gimmick/Room/Room.h"
#include"../../../../../Gimmick/Room/RoomManager.h"


void MageIntroState::OnStart(Mage* mage)
{
	mage->PlayAnimation(MageAnimationType::Idle);
}

void MageIntroState::OnUpdate(Mage * mage)
{
	if(m_introPhase!=Phase::WaitStart)
	{
		mage->UpdateFacingDirection();
	}

	switch (m_introPhase)
	{
	case MageIntroState::Phase::WaitStart:

		if (mage->IsIntroRequested())
		{
			mage->StartCameraIntro();
			m_introPhase = MageIntroState::Phase::WaitCamera;
		}
		break;
	case MageIntroState::Phase::WaitCamera:

		// カメラが位置に着いたら
		if (mage->IsCameraIntroHold())
		{
			mage->PlayAnimation(MageAnimationType::Roar);
			m_introPhase = MageIntroState::Phase::Roar;
		}
		break;

	case MageIntroState::Phase::Roar:

		if (!m_isRoar)
		{
			mage->StartCameraRoar();

			// 咆哮SEを再生
			KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Boss/Mage/Roar/Roar.wav");

			KdEffekseerManager::GetInstance().Play("Boss/Mage/Roar/Roar.efkefc",mage->GetPos()+Math::Vector3(0.0f,1.4f,1.5f), 1.0f, 0.3f);

			m_isRoar = true;
		}
		
		if (mage->IsAnimationFinished())
		{
			m_introPhase = MageIntroState::Phase::WaitEnd;
			mage->PlayAnimation(MageAnimationType::Idle);
		}

		break;
	case MageIntroState::Phase::WaitEnd:

		// イントロ終了が終了したら
		if(!mage->IsBossIntroPlaying())
		{

			m_pMachine->ChangeState<MageNormalState>();

			// 現在いる部屋を取得
			auto room = RoomManager::Instance().GetCurrentRoom();

			if (!room){	return;}
	
			room->FinishIntro();

			return;
		}

		break;
	}
}

void MageIntroState::OnExit(Mage * mage)
{
	// 攻撃のカウントが進んでいるのでカウントをリセット
	mage->EndAttack();

	mage->CreateHPBar();
}
