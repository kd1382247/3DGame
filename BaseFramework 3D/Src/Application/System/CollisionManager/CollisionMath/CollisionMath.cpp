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
		localMove,
		radius,
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

bool CollisionMath::SphereSweepVsAABB(const Math::Vector3& start, const Math::Vector3& move, float radius, const Math::Vector3& boxMin, const Math::Vector3& boxMax, float& outTOI, Math::Vector3& outNormal)
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

bool CollisionMath::SphereVsCylinder(const DirectX::BoundingSphere& sphere, const Math::Vector3& cylinderCenter, float cylinderRadius, float cylinderHalfHeight, Math::Vector3& outPush, Math::Vector3& outNormal)
{
	outPush = Math::Vector3::Zero;
	outNormal = Math::Vector3::Zero;

	const Math::Vector3 sphereCenter = sphere.Center;

	// 高さが重なっていなければ当たらない
	if (std::abs(sphereCenter.y - cylinderCenter.y) >= cylinderHalfHeight + sphere.Radius)
	{
		return false;
	}

	// 水平方向(X,Z)の距離だけで判定する
	const float dx = sphereCenter.x - cylinderCenter.x;
	const float dz = sphereCenter.z - cylinderCenter.z;

	const float distanceSq = dx * dx + dz * dz;

	const float radiusSum = cylinderRadius + sphere.Radius;

	if (distanceSq >= radiusSum * radiusSum)
	{
		return false;
	}

	const float distance = std::sqrt(distanceSq);

	// 中心がほぼ同じ場所だと押し出す方向が決まらないので、決め打ちの方向にする
	if (distance < 0.0001f)
	{
		outNormal = Math::Vector3::Right;
		outPush = outNormal * radiusSum;

		return true;
	}

	outNormal = Math::Vector3(dx / distance, 0.0f, dz / distance);
	outPush = outNormal * (radiusSum - distance);

	return true;
}

bool CollisionMath::SphereSweepVsCylinder(const Math::Vector3& start, const Math::Vector3& move, float radius, const Math::Vector3& cylinderCenter, float cylinderRadius, float cylinderHalfHeight, float& outTOI, Math::Vector3& outNormal)
{
	// 水平方向(X,Z)だけの動き
	const float vx = move.x;
	const float vz = move.z;

	const float a = vx * vx + vz * vz;

	// 水平に動かないなら、側面には当たらない
	if (a < 0.000001f)
	{
		return false;
	}

	// 円柱の中心から見た、開始位置
	const float px = start.x - cylinderCenter.x;
	const float pz = start.z - cylinderCenter.z;

	const float radiusSum = cylinderRadius + radius;

	const float c = px * px + pz * pz - radiusSum * radiusSum;

	// 最初からめり込んでいる場合は、めり込み解消(SphereVsCylinder)に任せる
	if (c <= 0.0f)
	{
		return false;
	}

	// 2次方程式 a*t^2 + 2*b*t + c = 0 を解く
	const float b = px * vx + pz * vz;

	const float discriminant = b * b - a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	const float t = (-b - std::sqrt(discriminant)) / a;

	// 今回の移動の間に当たらない(離れていく向きの場合もここで外れる)
	if (t < 0.0f || t > 1.0f)
	{
		return false;
	}

	// 当たる瞬間に、高さが重なっているか
	const float hitY = start.y + move.y * t;

	if (std::abs(hitY - cylinderCenter.y) >= cylinderHalfHeight + radius)
	{
		return false;
	}

	// 当たった位置から、円柱の中心を引いた水平方向が法線
	const float hx = px + vx * t;
	const float hz = pz + vz * t;

	const float length = std::sqrt(hx * hx + hz * hz);

	if (length < 0.0001f)
	{
		return false;
	}

	outTOI = t;
	outNormal = Math::Vector3(hx / length, 0.0f, hz / length);

	return true;
}


// 2つのAABBが重なっているか
bool CollisionMath::AABBVsAABB(const Math::Vector3& minA, const Math::Vector3& maxA,const Math::Vector3& minB, const Math::Vector3& maxB)
{
	return minA.x <= maxB.x && maxA.x >= minB.x
		&& minA.y <= maxB.y && maxA.y >= minB.y
		&& minA.z <= maxB.z && maxA.z >= minB.z;
}

void CollisionMath::OBBtoAABB(const DirectX::BoundingOrientedBox& obb, Math::Vector3& outMin, Math::Vector3& outMax)
{
	DirectX::BoundingOrientedBox box = obb;

	static constexpr int CornerNum = 8;

	DirectX::XMFLOAT3 corners[CornerNum];

	// 8つの角をとる
	box.GetCorners(&corners[0]);

	// 要素0で初期化
	Math::Vector3 cornerMin = corners[0];
	Math::Vector3 cornerMax = corners[0];


	for (int i = 0; i < CornerNum; i++)
	{
		Math::Vector3 corner = corners[i];

		cornerMin = Math::Vector3::Min(cornerMin, corner);
		cornerMax = Math::Vector3::Max(cornerMax, corner);
	}

	outMin = cornerMin;
	outMax = cornerMax;
}

void CollisionMath::PreserveHorizontalSpeedOnSlope(const Math::Vector3& preProjectMove, const Math::Vector3& normal, Math::Vector3& move)
{

	// 法線のY成分が無いと平面の方程式が解けないため補正しない
	// (歩行可能面はIsWalkableSurfaceで角度チェック済みのため通常は発生しない)
	if (std::abs(normal.y) <= 0.000001f)
	{
		return;
	}

	const float dx = preProjectMove.x;
	const float dz = preProjectMove.z;

	// 元々水平方向の移動が無かった(重力のみ等)場合は補正しない
	if (dx * dx + dz * dz <= 0.000001f)
	{
		return;
	}

	// 坂の平面上の移動ベクトルは法線と直交する
	// (normal.x*dx + normal.y*dy + normal.z*dz = 0)ので、
	// 元の水平移動量(dx, dz)をそのまま保ったまま
	// 坂の平面に乗るために必要な高さdyを直接求める
	const float slopeY = -(normal.x * dx + normal.z * dz) / normal.y;

	move.x = dx;
	move.y = slopeY;
	move.z = dz;

}

