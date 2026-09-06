#include "OBBCollision.h"

#include"OBBCollisionManager.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../System/CollisionManager/CollisionManager.h"

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

void OBBCollision::DrawInspector()
{
	// 座標変更
	Math::Vector3 pos = GetPos();

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetPos(pos);
		EditorManager::Instance().MarkDirty();
	}

	Math::Vector3 scale = GetScale();

	// 大きさ変更
	if (ImGui::DragFloat3("Size", &scale.x, 0.01f))
	{
		SetScale(scale);
		EditorManager::Instance().MarkDirty();
	}
	

	Math::Vector3 rotation = GetRotation();
	// 回転
	if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01f))
	{
		SetRotation(rotation);
		EditorManager::Instance().MarkDirty();
	}

}

void OBBCollision::Destroy()
{
	OBBCollisionManager::Instance().RemoveOBBCollision(GetID());
}
