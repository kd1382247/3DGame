#include "AABBCollision.h"

#include"AABBCollisionManager.h"

#include"../../../../Editor/EditorManager.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

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

void AABBCollision::DrawInspector()
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
}

void AABBCollision::SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	// AABBは回転を表現できないため、位置と大きさだけをStageに追従させる
	SetPos(stagePos + m_localPos * stageScale);
	SetScale(m_localScale * stageScale);
}

void AABBCollision::Destroy()
{
	AABBCollisionManager::Instance().RemoveAABBCollision(GetID());
}
