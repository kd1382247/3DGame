#include "CollisionMath.h"

bool CollisionMath::SphereVsAABB(const DirectX::BoundingSphere& sphere, const DirectX::BoundingBox& box, Math::Vector3& outPush, Math::Vector3& outNormal)
{
	outPush = Math::Vector3::Zero;

	Math::Vector3 sphereCenter = sphere.Center;

	Math::Vector3 boxCenter = box.Center;
	Math::Vector3 boxExtents = box.Extents;

	// ==============================
	// BoxのMin / Max座標を求める
	// ==============================

	Math::Vector3 boxMin = boxCenter - boxExtents;

	Math::Vector3 boxMax = boxCenter + boxExtents;

	// ==============================
	// Sphere中心から一番近い
	// Box上の点を求める
	// ==============================

	Math::Vector3 closestPoint;

	closestPoint.x = std::clamp(sphereCenter.x, boxMin.x, boxMax.x);
	closestPoint.y = std::clamp(sphereCenter.y, boxMin.y, boxMax.y);
	closestPoint.z = std::clamp(sphereCenter.z, boxMin.z, boxMax.z);

	// ==============================
	// 最近接点 → Sphere中心
	// ==============================

	Math::Vector3 diff = sphereCenter - closestPoint;

	float distanceSq = diff.LengthSquared();

	float radiusSq = sphere.Radius * sphere.Radius;

	// SphereとBoxが離れている
	if (distanceSq > radiusSq)
	{
		return false;
	}

	// ==============================
	// Sphere中心がBox内部にない場合
	// ==============================

	if (distanceSq > 0.000001f)
	{
		float distance = std::sqrt(distanceSq);

		Math::Vector3 normal = diff / distance;

		float overlap = sphere.Radius - distance;

		outPush = normal * overlap;

		outNormal = normal;

		return true;
	}

	// ==============================
	// Sphere中心がBox内部にある場合
	//
	// closestPoint == sphereCenter
	// になるので、通常の方法では
	// 押し戻し方向を求められない。
	// ==============================

	float distanceLeft = sphereCenter.x - boxMin.x;

	float distanceRight = boxMax.x - sphereCenter.x;

	float distanceBottom = sphereCenter.y - boxMin.y;

	float distanceTop = boxMax.y - sphereCenter.y;

	float distanceBack = sphereCenter.z - boxMin.z;

	float distanceFront = boxMax.z - sphereCenter.z;

	// 一番近い面までの距離
	float minDistance = distanceLeft;

	outPush = Math::Vector3(-(distanceLeft + sphere.Radius), 0.0f, 0.0f);

	outNormal = Math::Vector3::Left;

	if (distanceRight < minDistance)
	{
		minDistance = distanceRight;

		outPush = Math::Vector3(distanceRight + sphere.Radius, 0.0f, 0.0f);

		outNormal = Math::Vector3::Right;
	}

	if (distanceBottom < minDistance)
	{
		minDistance = distanceBottom;

		outPush = Math::Vector3(0.0f, -(distanceBottom + sphere.Radius), 0.0f);


		outNormal = Math::Vector3::Down;
	}

	if (distanceTop < minDistance)
	{
		minDistance = distanceTop;

		outPush = Math::Vector3(0.0f, distanceTop + sphere.Radius, 0.0f);

		outNormal = Math::Vector3::Up;
	}

	if (distanceBack < minDistance)
	{
		minDistance = distanceBack;

		outPush = Math::Vector3(0.0f, 0.0f, -(distanceBack + sphere.Radius));

		outNormal = Math::Vector3::Forward;
	}

	if (distanceFront < minDistance)
	{
		minDistance = distanceFront;

		outPush = Math::Vector3(0.0f, 0.0f, distanceFront + sphere.Radius);

		outNormal = Math::Vector3::Backward;
	}

	return true;
}

bool CollisionMath::SphereVsOBB(const DirectX::BoundingSphere& sphere, const DirectX::BoundingOrientedBox& obb, Math::Vector3& outPush, Math::Vector3& outNormal)
{
	Math::Vector4 orientation = obb.Orientation;

	// Sphere中心をOBBローカル空間へ
	Math::Vector3 localCenter =
		XMVector3InverseRotate(Math::Vector3(sphere.Center) - Math::Vector3(obb.Center), orientation);

	DirectX::BoundingSphere localSphere;
	localSphere.Center = localCenter;
	localSphere.Radius = sphere.Radius;

	// OBBはローカル空間ではAABB
	DirectX::BoundingBox localBox;
	localBox.Center = { 0.0f, 0.0f, 0.0f };
	localBox.Extents = obb.Extents;

	Math::Vector3 localPush = Math::Vector3::Zero;
	Math::Vector3 localNormal = Math::Vector3::Zero;

	if (!SphereVsAABB(localSphere, localBox, localPush, localNormal))
	{
		return false;
	}

	// ローカル → ワールド
	outPush = XMVector3Rotate(localPush, orientation);

	outNormal = XMVector3Rotate(localNormal, orientation);

	if (outNormal.LengthSquared() > 0.000001f)
	{
		outNormal.Normalize();
	}

	return true;

}

bool CollisionMath::SphereSweepVsOBB(const Math::Vector3& start, const Math::Vector3& move, float radius, const DirectX::BoundingOrientedBox& obb, float& outTOI, Math::Vector3& outNormal)
{

	Math::Vector4 orientation = obb.Orientation;

	// -------------------------------
	// World → OBBローカル
	// -------------------------------

	Math::Vector3 localStart =
		XMVector3InverseRotate(start - Math::Vector3(obb.Center), orientation);

	Math::Vector3 localMove =
		XMVector3InverseRotate(move, orientation);

	// ============================
	// OBBローカルではAABBになる
	// ============================

	Math::Vector3 extents = obb.Extents;

	Math::Vector3 boxMin = -extents;
	Math::Vector3 boxMax = extents;

	Math::Vector3 localNormal = Math::Vector3::Zero;

	// ============================
	// 既存のAABB Sweepを再利用
	// ============================

	if (!SphereSweepVsAABB(
		localStart,
		radius,
		localMove,
		boxMin,
		boxMax,
		outTOI,
		localNormal))
	{
		return false;
	}

	// ============================
	// Local Normal → World Normal
	// ============================

	outNormal = XMVector3Rotate(localNormal, orientation);

	outNormal.Normalize();

	return true;

	
}

bool CollisionMath::SphereSweepVsAABB(const Math::Vector3& start, float radius, const Math::Vector3& move, const Math::Vector3& boxMin, const Math::Vector3& boxMax, float& outTOI, Math::Vector3& outNormal)
{

	Math::Vector3 radiusVec(radius, radius, radius);

	// スフィアの半径分、ボックスの範囲を広げる
	Math::Vector3 expandedMin = boxMin - radiusVec;
	Math::Vector3 expandedMax = boxMax + radiusVec;

	return SegmentVsAABB(start, move, expandedMin, expandedMax, outTOI, outNormal);
}

bool CollisionMath::SegmentVsAABB(const Math::Vector3& start, const Math::Vector3& move, const Math::Vector3& boxMin, const Math::Vector3& boxMax, float& outTOI, Math::Vector3& outNormal)
{

	constexpr float epsilon = 0.000001f;
	constexpr float contactEpsilon = 0.001f;

	// 表面接触している面の中から、
	// 最も強く内部へ進もうとしている面を選ぶ
	bool hasStartContact = false;

	float mostNegativeInto = 0.0f;

	Math::Vector3 startContactNormal =Math::Vector3::Zero;
	// =====================================
	// 開始地点がBox表面付近にいて
	// 内部へ進もうとしている場合は
	// TOI = 0 の衝突として扱う
	// =====================================

	// X Min
	if (std::abs(start.x - boxMin.x) <= contactEpsilon &&
		start.y >= boxMin.y - contactEpsilon &&
		start.y <= boxMax.y + contactEpsilon &&
		start.z >= boxMin.z - contactEpsilon &&
		start.z <= boxMax.z + contactEpsilon)
	{
		Math::Vector3 normal =
			Math::Vector3::Left;

		float into =
			move.Dot(normal);

		if (into < mostNegativeInto)
		{
			mostNegativeInto = into;
			startContactNormal = normal;
			hasStartContact = true;
		}
	}

	// X Max
	if (std::abs(start.x - boxMax.x) <= contactEpsilon &&
		start.y >= boxMin.y - contactEpsilon &&
		start.y <= boxMax.y + contactEpsilon &&
		start.z >= boxMin.z - contactEpsilon &&
		start.z <= boxMax.z + contactEpsilon)
	{
		Math::Vector3 normal =
			Math::Vector3::Right;

		float into =
			move.Dot(normal);

		if (into < mostNegativeInto)
		{
			mostNegativeInto = into;
			startContactNormal = normal;
			hasStartContact = true;
		}
	}

	// Y Min
	if (std::abs(start.y - boxMin.y) <= contactEpsilon &&
		start.x >= boxMin.x - contactEpsilon &&
		start.x <= boxMax.x + contactEpsilon &&
		start.z >= boxMin.z - contactEpsilon &&
		start.z <= boxMax.z + contactEpsilon)
	{
		Math::Vector3 normal =
			Math::Vector3::Down;

		float into =
			move.Dot(normal);

		if (into < mostNegativeInto)
		{
			mostNegativeInto = into;
			startContactNormal = normal;
			hasStartContact = true;
		}
	}

	if (std::abs(start.y - boxMax.y) <= contactEpsilon &&
		start.x >= boxMin.x - contactEpsilon &&
		start.x <= boxMax.x + contactEpsilon &&
		start.z >= boxMin.z - contactEpsilon &&
		start.z <= boxMax.z + contactEpsilon)
	{
		Math::Vector3 normal =
			Math::Vector3::Up;

		float into =
			move.Dot(normal);

		if (into < mostNegativeInto)
		{
			mostNegativeInto = into;
			startContactNormal = normal;
			hasStartContact = true;
		}
	}

	// Z Min
	if (std::abs(start.z - boxMin.z) <= contactEpsilon &&
		start.x >= boxMin.x - contactEpsilon &&
		start.x <= boxMax.x + contactEpsilon &&
		start.y >= boxMin.y - contactEpsilon &&
		start.y <= boxMax.y + contactEpsilon)
	{
		Math::Vector3 normal =
			Math::Vector3::Forward;

		float into =
			move.Dot(normal);

		if (into < mostNegativeInto)
		{
			mostNegativeInto = into;
			startContactNormal = normal;
			hasStartContact = true;
		}
	}

	if (std::abs(start.z - boxMax.z) <= contactEpsilon &&
		start.x >= boxMin.x - contactEpsilon &&
		start.x <= boxMax.x + contactEpsilon &&
		start.y >= boxMin.y - contactEpsilon &&
		start.y <= boxMax.y + contactEpsilon)
	{
		Math::Vector3 normal =
			Math::Vector3::Backward;

		float into =
			move.Dot(normal);

		if (into < mostNegativeInto)
		{
			mostNegativeInto = into;
			startContactNormal = normal;
			hasStartContact = true;
		}
	}

	if (hasStartContact)
	{
		outTOI = 0.0f;
		outNormal = startContactNormal;
		return true;
	}


	// 衝突位置
	float tEnter = 0.0f;
	float tExit = 1.0f;

	// 当たった面の法線ベクトル
	Math::Vector3 hitNormal = Math::Vector3::Zero;

	// X軸
	if (std::abs(move.x) < epsilon)
	{
		// 動いていない場合、ボックスの範囲内にいるか
		if (start.x<boxMin.x || start.x>boxMax.x)
		{
			return false;
		}
	}
	else
	{
		float t1 = (boxMin.x - start.x) / move.x;
		float t2 = (boxMax.x - start.x) / move.x;

		float axisEnter = std::min(t1, t2);
		float axisExit = std::max(t1, t2);

		if (axisEnter > tEnter)
		{
			tEnter = axisEnter;

			// 法線ベクトルを更新
			hitNormal = move.x > 0 ?
				Math::Vector3::Left :
				Math::Vector3::Right;
		}

		tExit = std::min(tExit, axisExit);

		if (tEnter > tExit)
		{
			return false;
		}
	}

	// Y軸
	if (std::abs(move.y) < epsilon)
	{
		// 動いていない場合、ボックスの範囲内にいるか
		if (start.y<boxMin.y || start.y>boxMax.y)
		{
			return false;
		}
	}
	else
	{
		float t1 = (boxMin.y - start.y) / move.y;
		float t2 = (boxMax.y - start.y) / move.y;

		float axisEnter = std::min(t1, t2);
		float axisExit = std::max(t1, t2);

		if (axisEnter > tEnter)
		{
			tEnter = axisEnter;

			// 法線ベクトルを更新
			hitNormal = move.y > 0 ?
				Math::Vector3::Down :
				Math::Vector3::Up;
		}

		tExit = std::min(tExit, axisExit);

		if (tEnter > tExit)
		{
			return false;
		}
	}

	// Z軸
	if (std::abs(move.z) < epsilon)
	{
		// 動いていない場合、ボックスの範囲内にいるか
		if (start.z<boxMin.z || start.z>boxMax.z)
		{
			return false;
		}
	}
	else
	{
		float t1 = (boxMin.z - start.z) / move.z;
		float t2 = (boxMax.z - start.z) / move.z;

		float axisEnter = std::min(t1, t2);
		float axisExit = std::max(t1, t2);

		if (axisEnter > tEnter)
		{
			tEnter = axisEnter;

			// 法線ベクトルを更新
			hitNormal = move.z > 0 ?
				Math::Vector3::Forward :
				Math::Vector3::Backward;
		}

		tExit = std::min(tExit, axisExit);

		if (tEnter > tExit)
		{
			return false;
		}
	}

	// 移動によって新しく面に衝突していない
	if (hitNormal.LengthSquared() <= 0.000001f)
	{

		return false;
	}

	outTOI = tEnter;
	outNormal = hitNormal;

	return true;

}
