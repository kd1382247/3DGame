#include "WallCollision.h"

#include"WallCollisionManager.h"

#include"../../../../../Editor/EditorManager.h"
#include"../../../../../System/CollisionManager/CollisionManager.h"

void WallCollision::Init()
{

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	CollisionManager::Instance().RegisterObject(CollisionLayer::Wall, shared_from_this());

	SetPos(Math::Vector3::Zero);
}

void WallCollision::DrawDebug()
{

	Math::Vector3 halfSize =
	{
		m_scale.x * 0.5f,
		m_scale.y * 0.5f,
		m_scale.z * 0.5f
	};


	m_pDebugWire->AddDebugBox(m_mWorld, halfSize, Math::Vector3::Zero, false, kGreenColor);

	KdGameObject::DrawDebug();

}

DirectX::BoundingBox WallCollision::GetBox() const
{
	DirectX::BoundingBox box;

	box.Center = GetPos();

	box.Extents =
	{
		m_scale.x * 0.5f,
		m_scale.y * 0.5f,
		m_scale.z * 0.5f
	};

	return box;
}

void WallCollision::DrawInspector()
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
}

void WallCollision::Destroy()
{
	WallCollisionManager::Instance().RemoveWallCollision(GetID());
}
