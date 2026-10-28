#include "OBBCollision.h"

#include"OBBCollisionManager.h"

#include"../../../../Editor/EditorManager.h"

#include"../../../../System/CollisionManager/CollisionManager.h"
#include"../../../../System/CollisionManager/CollisionMath/CollisionMath.h"

void OBBCollision::Init()
{

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("OBB", GetBox(), KdCollider::TypeBump);

	CollisionManager::Instance().RegisterObject(CollisionLayer::OBB, shared_from_this());

	SetPos(Math::Vector3::Zero);
}

void OBBCollision::DrawDebug()
{

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
	DirectX::BoundingOrientedBox box = GetBox();

	static constexpr int CornerNum = 8;

	DirectX::XMFLOAT3 corners[CornerNum];

	// 8つの角をとる
	box.GetCorners( &corners[0]);

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
	DirectX::BoundingOrientedBox box;

	box.Center = GetPos();

	box.Extents =
	{
		GetScale().x * 0.5f,
		GetScale().y * 0.5f,
		GetScale().z * 0.5f
	};


	Math::Vector3 rot = GetRotation();

	Math::Quaternion quaternion =
		Math::Quaternion::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(rot.y),
			DirectX::XMConvertToRadians(rot.x),
			DirectX::XMConvertToRadians(rot.z)
		);

	box.Orientation =
	{
		quaternion.x,
		quaternion.y,
		quaternion.z,
		quaternion.w
	};

	return box;
}

void OBBCollision::DrawInspectorHeader()
{
	// 座標変更(Stageからのローカル位置)
	Math::Vector3 pos = GetLocalPos();

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetLocalPos(pos);
		EditorManager::Instance().MarkDirty();
	}

	Math::Vector3 scale = GetLocalScale();

	// 大きさ変更(Stageからのローカル大きさ)
	if (ImGui::DragFloat3("Size", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
		EditorManager::Instance().MarkDirty();
	}


	Math::Vector3 rotation = GetRotation();
	// 回転(Stageには追従させず、OBB自身の回転をそのまま使う)
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f))
	{
		SetRotation(rotation);
		EditorManager::Instance().MarkDirty();
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

void OBBCollision::SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	// 回転はStageに追従させず、位置と大きさだけを追従させる
	SetPos(stagePos + m_localPos * stageScale);
	SetScale(m_localScale * stageScale);
}

void OBBCollision::Destroy()
{
	OBBCollisionManager::Instance().RemoveOBBCollision(GetID());
}
