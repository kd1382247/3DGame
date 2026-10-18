#include "AIBlock.h"

#include"AIBlockManager.h"

#include"../../../../Editor/EditorManager.h"

void AIBlock::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// エディタでクリックして選択するための判定
	// (CollisionManagerには登録しないので、キャラクターとは当たらない)
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("AIBlock", GetBox(), KdCollider::TypeBump);

	SetPos(Math::Vector3::Zero);
}

void AIBlock::DrawDebug()
{
	// AABB(緑)と区別するため青で表示。選択中は水色
	Math::Color color;

	if (IsSelected())
	{
		color = kBlueColor + kGreenColor;
	}
	else
	{
		color = kBlueColor;
	}

	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, false, color);

	KdGameObject::DrawDebug();
}

DirectX::BoundingBox AIBlock::GetBox() const
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

void AIBlock::DrawInspectorHeader()
{
	// 座標変更(Stageからのローカル位置)
	Math::Vector3 pos = GetLocalPos();

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetLocalPos(pos);
		EditorManager::Instance().MarkDirty();
	}

	// 大きさ変更(Stageからのローカル大きさ)
	Math::Vector3 scale = GetLocalScale();

	if (ImGui::DragFloat3("Size", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
		EditorManager::Instance().MarkDirty();
	}
}

void AIBlock::SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	// AABBは回転を表現できないため、位置と大きさだけをStageに追従させる
	SetPos(stagePos + m_localPos * stageScale);
	SetScale(m_localScale * stageScale);
}

void AIBlock::Destroy()
{
	AIBlockManager::Instance().RemoveAIBlock(GetID());
}
