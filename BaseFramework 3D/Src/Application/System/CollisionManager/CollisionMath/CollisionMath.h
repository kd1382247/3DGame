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
		float radius, const Math::Vector3& move,
		const Math::Vector3& boxMin, const Math::Vector3& boxMax,
		float& outTOI,
		Math::Vector3& outNormal);

	bool SegmentVsAABB(const Math::Vector3& start,
		const Math::Vector3& move, 
		const Math::Vector3& boxMin,
		const Math::Vector3& boxMax,
		float& outTOI,
		Math::Vector3& outNormal);



}