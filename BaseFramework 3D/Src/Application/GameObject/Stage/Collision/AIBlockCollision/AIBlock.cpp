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
	if (!AIBlockManager::Instance().IsDebug())
	{
		return;
	}

	// Active StageのAIBlockだけ表示する
	if (GetOwner() != EditorManager::Instance().GetActiveStage())
	{
		return;
	}

	// AABB(緑)と区別するため青で表示。選択中は黄色
	Math::Color color;

	if (IsSelected())
	{
		color = kRedColor + kGreenColor;
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
