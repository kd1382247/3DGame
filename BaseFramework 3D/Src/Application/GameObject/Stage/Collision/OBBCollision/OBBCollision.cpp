#include "OBBCollision.h"

#include"OBBCollisionManager.h"

#include"../../../../Editor/EditorManager.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/CollisionManager/CollisionMath/CollisionMath.h"

void OBBCollision::Init()
{

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// KdColliderには単位箱を登録する(位置・大きさ・回転はm_mWorldが掛ける)
	RegisterUnitBoxCollider("OBB", KdCollider::TypeBump, true);

	CollisionManager::Instance().RegisterObject(CollisionLayer::OBB, shared_from_this());

	SetPos(Math::Vector3::Zero);
}

void OBBCollision::DrawDebug()
{
	if (!OBBCollisionManager::Instance().IsDebug())
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

	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, true, color);

	KdGameObject::DrawDebug();

}

void OBBCollision::GetWorldBounds(Math::Vector3& outMin, Math::Vector3& outMax) const
{
	CollisionMath::OBBtoAABB(GetBox(), outMin, outMax);
}

bool OBBCollision::OverlapSphere(const DirectX::BoundingSphere & sphere, Math::Vector3 & outPush, Math::Vector3 & outNormal) const
{
	return CollisionMath::SphereVsOBB(sphere,GetBox(),outPush,outNormal);
}

bool OBBCollision::SweepSphere(const Math::Vector3& start, const Math::Vector3& move, float radius, float& outToi, Math::Vector3& outNormal) const
{
	return CollisionMath::SphereSweepVsOBB(start,move,radius,GetBox(),outToi,outNormal);
}

void OBBCollision::SetCollisionType(const std::string& typeName)
{
	for (int i = 0; i < std::size(m_collisionTypeName); i++)
	{
		if (typeName == m_collisionTypeName[i])
		{
			m_collisionType = static_cast<OBBCollisionType>(i);
		}
	}
}

DirectX::BoundingOrientedBox OBBCollision::GetBox() const
{
	return GetWorldOBB();
}

void OBBCollision::DrawInspectorHeader()
{
	// 基底クラスのInspectorを呼ぶ
	StageOwnedObject::DrawInspectorHeader();

	Math::Vector3 rotation = GetRotation();

	if(!UsesLocalRotation())
	{
		if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f))
		{
			SetRotation(rotation);
			EditorManager::Instance().MarkDirty();
		}
	}

	std::string collTypeName = m_collisionTypeName[static_cast<int>(GetCollisionType())];

	if (ImGui::BeginCombo("CollisionType", collTypeName.c_str()))
	{
		for (int i = 0; i < std::size(m_collisionTypeName); i++)
		{
			if (ImGui::Selectable(m_collisionTypeName[i].c_str(),
				i == static_cast<int>(GetCollisionType())))
			{
				if (i != static_cast<int>(GetCollisionType()))
				{

					SetCollisionType(static_cast<OBBCollisionType>(i));

					EditorManager::Instance().MarkDirty();
				}
			}

		}

		ImGui::EndCombo();
	}
}

void OBBCollision::CopyFrom(const std::shared_ptr<StageOwnedObject>& ownedObject)
{

	auto obb = std::dynamic_pointer_cast<OBBCollision>(ownedObject);

	if (!obb)
	{
		return;
	}

	StageOwnedObject::CopyFrom(ownedObject);

	SetCollisionType(obb->GetCollisionType());
}
