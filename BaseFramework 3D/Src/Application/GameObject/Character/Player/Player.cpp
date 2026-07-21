#include "Player.h"

void Player::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/TestChara/TestChara.gltf");

		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetAnimation("Bow"));


		m_objectName = "Player";
	}

	SetPos({ -12.0f, 12.5f, 1.5f });
}

void Player::Update()
{

	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());

	if (m_spModel->NeedCalcNodeMatrices())
	{
		m_spModel->CalcNodeMatrices();
	}

	m_Gravity += 0.04;

	m_mWorld._42 -= m_Gravity;


	// 座標更新
	Math::Vector3 nowPos = GetPos();
	Math::Vector3 moveVec = Math::Vector3::Zero;
	float         speed = 0.1;

	if (GetAsyncKeyState('D')) { moveVec.x = 1; }
	if (GetAsyncKeyState('A')) { moveVec.x = -1; }
	if (GetAsyncKeyState('W')) { moveVec.z = 1; }
	if (GetAsyncKeyState('S')) { moveVec.z = -1; }

	moveVec.Normalize();

	nowPos += moveVec * speed;

	// 大きさ
	Math::Vector3 nowScale = GetScale();

	Math::Matrix transMat = Math::Matrix::CreateTranslation(nowPos);
	Math::Matrix scaleMat = Math::Matrix::CreateScale(nowScale);
	m_mWorld =  scaleMat*transMat;


}
