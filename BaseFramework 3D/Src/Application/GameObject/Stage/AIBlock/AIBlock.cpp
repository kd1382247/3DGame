#include "AIBlock.h"

void AIBlock::Init()
{

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();


	m_pCollider = std::make_unique<KdCollider>();

	DirectX::BoundingBox box;

	// AIBlockLocal空間での中心
	box.Center = { 0.0f,0.0f,0.0f };

	// 基本サイズ
	box.Extents = { 0.5f,0.5f,0.5f };

	m_pCollider->RegisterCollisionShape(
		"AIBlock",
		box,
		KdCollider::TypeSight
	);

}

void AIBlock::DrawDebug()
{
	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f },Math::Vector3::Zero,false,kGreenColor);
	KdGameObject::DrawDebug();
}
