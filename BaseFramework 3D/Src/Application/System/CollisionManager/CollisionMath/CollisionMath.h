#pragma once

namespace CollisionMath
{

	bool SphereVsAABB(
		const DirectX::BoundingSphere& sphere,
		const DirectX::BoundingBox& box,
		Math::Vector3& outPush,
		Math::Vector3& outNormal);

	bool SphereVsOBB(
		const DirectX::BoundingSphere& sphere,
		const DirectX::BoundingOrientedBox& obb,
		Math::Vector3& outPush,
		Math::Vector3& outNormal);


	bool SphereSweepVsOBB(
		const Math::Vector3& start,
		const Math::Vector3& move,
		float radius,
		const DirectX::BoundingOrientedBox& obb,
		float& outTOI,
		Math::Vector3& outNormal);

	bool SphereSweepVsAABB(
		const Math::Vector3& sphereCenter, 
		const Math::Vector3& move,
		float radius, 
		const Math::Vector3& boxMin, const Math::Vector3& boxMax,
		float& outTOI,
		Math::Vector3& outNormal);

	// 縦向き(Y軸に平行)の円柱とキャラの球のめり込み判定
	// 円柱は、中心からY方向にcylinderHalfHeightの範囲。押し戻しは水平方向だけ
	bool SphereVsCylinder(
		const DirectX::BoundingSphere& sphere,
		const Math::Vector3& cylinderCenter,
		float cylinderRadius,
		float cylinderHalfHeight,
		Math::Vector3& outPush,
		Math::Vector3& outNormal);

	// 動く球と縦向き円柱のスウィープ判定(最初に当たる時刻と、水平方向の法線を返す)
	bool SphereSweepVsCylinder(
		const Math::Vector3& start,
		const Math::Vector3& move,
		float radius,
		const Math::Vector3& cylinderCenter,
		float cylinderRadius,
		float cylinderHalfHeight,
		float& outTOI,
		Math::Vector3& outNormal);

	bool SegmentVsAABB(const Math::Vector3& start,
		const Math::Vector3& move, 
		const Math::Vector3& boxMin,
		const Math::Vector3& boxMax,
		float& outTOI,
		Math::Vector3& outNormal);


	bool AABBVsAABB(
		const Math::Vector3& minA, const Math::Vector3& maxA,
		const Math::Vector3& minB, const Math::Vector3& maxB);

	void OBBtoAABB(const DirectX::BoundingOrientedBox& obb, Math::Vector3& outMin, Math::Vector3& outMax);


	// 歩行可能面(坂)では、水平方向(X,Z)の移動量は入力のまま保ち、
	// 坂の平面の方程式から必要な高さ(Y)だけを再計算することで、
	// 斜め移動時も方向を歪ませずに水平速度を保つ
	void PreserveHorizontalSpeedOnSlope(
		const Math::Vector3& preProjectMove,
		const Math::Vector3& normal,
		Math::Vector3& move);


}