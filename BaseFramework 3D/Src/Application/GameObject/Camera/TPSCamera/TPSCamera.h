#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()		override;


private:

	// マウスでの回転とターゲットへの追従で、カメラのワールド行列を更新する
	void UpdateCameraMatrix(const std::shared_ptr<const KdGameObject>& spTarget);

	// プレイヤーからカメラへレイを飛ばし、壁の向こう側にいたらプレイヤー側へ引き寄せる
	void PullInByRay(const KdGameObject& target);

	// カメラ位置にスフィアを置き、壁に近すぎたら押し戻す
	void PushOutBySphere();

	// カメラとの距離
	float m_cameraDist = -1.0f;

	// 遅れて追う位置を覚える
	Math::Vector3 m_smoothTargetPos = {};

	bool          m_isSmoothTargetInit = false;

};
