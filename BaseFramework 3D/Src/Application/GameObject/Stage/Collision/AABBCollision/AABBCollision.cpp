#include "AABBCollision.h"

#include"AABBCollisionManager.h"

#include"../../../../Editor/EditorManager.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/CollisionManager/CollisionMath/CollisionMath.h"

void AABBCollision::Init()
{

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("AABB",GetBox(),KdCollider::TypeBump);

	CollisionManager::Instance().RegisterObject(CollisionLayer::AABB, shared_from_this());

	SetPos(Math::Vector3::Zero);
}

void AABBCollision::DrawDebug()
{
	if (!AABBCollisionManager::Instance().IsDebug())
	{
		return;
	}

	// Active StageのAABBだけ表示する
	if (GetParent() != EditorManager::Instance().GetActiveStage())
	{
		return;
	}

	Math::Color color;

	if (IsSelected())
	{
		color = kRedColor + kGreenColor;
	}
	else
	{
		color = kGreenColor;
	}

	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, false,color);

	KdGameObject::DrawDebug();

}

void AABBCollision::GetWorldBounds(Math::Vector3& outMin, Math::Vector3& outMax) const
{
	DirectX::BoundingBox box = GetBox();

	Math::Vector3 boxCenter = box.Center;
	Math::Vector3 boxExtents = box.Extents;

	outMin = boxCenter - boxExtents;
	outMax = boxCenter + boxExtents;
}

bool AABBCollision::OverlapSphere(const DirectX::BoundingSphere & sphere, Math::Vector3 & outPush, Math::Vector3 & outNormal) const
{
	return CollisionMath::SphereVsAABB(sphere,GetBox(),outPush,outNormal);
}

bool AABBCollision::SweepSphere(const Math::Vector3& start, const Math::Vector3& move, float radius, float& outToi, Math::Vector3& outNormal) const
{
	Math::Vector3 outMin = {};
	Math::Vector3 outMax = {};

	GetWorldBounds(outMin, outMax);

	return CollisionMath::SphereSweepVsAABB(start, move, radius, outMin, outMax, outToi, outNormal);
}

DirectX::BoundingBox AABBCollision::GetBox() const
{
	DirectX::BoundingBox box;

	box.Center = GetPos();

	box.Extents =
	{
		GetScale().x * 0.5f,
		GetScale().y * 0.5f,
		GetScale().z * 0.5f
	};

	return box;
}