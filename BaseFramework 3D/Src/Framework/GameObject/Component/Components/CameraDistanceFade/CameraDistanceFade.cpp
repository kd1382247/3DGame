#include "CameraDistanceFade.h"

namespace
{
	// カメラが近い時にプレイヤーを透けさせる設定
	// この距離より遠いと不透明。近づくほど透け始める
	constexpr float kFadeStartDist = 2.5f;
	// この距離以下で完全に透明になる
	constexpr float kFadeEndDist = 1.0f;
	// 透け具合の変化の速さ(0～1、小さいほどゆっくり)
	constexpr float kFadeSpeed = 0.15f;
	// 透明度の上限
	constexpr float kFadeMaxRate = 0.8f;
}

void CameraDistanceFade::PostUpdate(KdGameObject& obj)
{
	UpdateCameraFade(obj.GetPos());
}

void CameraDistanceFade::UpdateCameraFade(const Math::Vector3& pos)
{

	float targetRate = 0.0f;

	// プレイヤーの体の中心とカメラとの距離
	const Math::Vector3 center = pos + Math::Vector3(0.0f, m_fadePivotHeight, 0.0f);
	const float dist = (m_cameraPos - center).Length();

	// 遠い(開始距離以上)なら0、近い(終了距離以下)なら1になる割合
	const float rate = (dist - kFadeEndDist) / (kFadeStartDist - kFadeEndDist);
	targetRate = 1.0f - std::clamp(rate, 0.0f, 1.0f);


	// 急に変わらないよう、少しずつ目標へ近づける
	m_fadeRate += (targetRate - m_fadeRate) * kFadeSpeed;

	if (m_fadeRate > kFadeMaxRate)
	{
		m_fadeRate = kFadeMaxRate;
	}

	// ほぼ0なら0にして、シェーダー側の処理を省く
	if (m_fadeRate < 0.001f)
	{
		m_fadeRate = 0.0f;
	}
}
