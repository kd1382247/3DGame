#include "EditorCamera.h"

void EditorCamera::Init()
{
	CameraBase::Init();
	
	m_mWorld = Math::Matrix::CreateTranslation(Math::Vector3(0.0f, 5.0f, -10.0f));
}

void EditorCamera::Update()
{

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		m_isCameraControl = true;
	}
	else
	{
		m_isCameraControl = false;
	}


	
	if (m_isCameraControl)
	{
		// カメラの回転
		UpdateRotateByMouse();

		m_mRotation = GetRotationMatrix();

		// カメラの移動
		Math::Vector3 move = Math::Vector3::Zero;
		float         speed = 0.2f;

		Math::Vector3 forward = m_mWorld.Backward();
		Math::Vector3 right = m_mWorld.Right();
		Math::Vector3 up = m_mWorld.Up();

		if (GetAsyncKeyState('W') & 0x8000)move += forward;
		if (GetAsyncKeyState('S') & 0x8000)move -= forward;
		if (GetAsyncKeyState('A') & 0x8000)move -= right;
		if (GetAsyncKeyState('D') & 0x8000)move += right;

		if (GetAsyncKeyState('E') & 0x8000)move += up;
		if (GetAsyncKeyState('Q') & 0x8000)move -= up;

		move.Normalize();

		Math::Vector3 nowPos = GetPos();
		nowPos += move * speed;

		Math::Matrix transMat = Math::Matrix::CreateTranslation(nowPos);


		m_mWorld = m_mRotation * transMat;
	}

}
