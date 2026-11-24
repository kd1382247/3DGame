#pragma once

#include"../../ComponentBase.h"

class CameraDistanceFade :public ComponentBase
{
public:

	void PostUpdate(KdGameObject& obj)override;

	// フェード率を返す
	float GetFadeRate()const { return m_fadeRate; }

	// カメラの位置を毎フレーム受け取る
	void UpdateCameraPos(const Math::Vector3& pos) { m_cameraPos = pos; }

	// 地面からの高さをセット
	void SetFadePivotHeight(float height) { m_fadePivotHeight = height; }

private:

	// カメラの距離に応じてフェード率を求める
	void UpdateCameraFade(const Math::Vector3& pos);

	// カメラの位置
	Math::Vector3 m_cameraPos = {};

	// フェード率
	float m_fadeRate = 0.0f;

	// オブジェクトの地面からの高さ
	float m_fadePivotHeight = 0.5f;

};