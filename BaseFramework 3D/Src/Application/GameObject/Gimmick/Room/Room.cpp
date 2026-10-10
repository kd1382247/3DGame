#include "Room.h"

void Room::Init()
{
	SetObjectName("Room");

	SetObjectCategory(KdGameObject::ObjectCategory::Gimmick);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	SetScale({ 10,6,10 });
	SetPos({ 5.0f,0.0f,0.0f });

}

void Room::DrawDebug()
{
	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, false, kBlueColor);
}

void Room::DrawInspectorHeader()
{
	KdGameObject::DrawBasicInspecter();
}

void Room::DrawInspector()
{
	
	

	if (ImGui::BeginCombo("RoomType", m_roomTypeName[static_cast<int>(m_roomType)].c_str()))
	{
		for (int i = 0; i < m_roomTypeName->size(); i++)
		{
			if(ImGui::Selectable(m_roomTypeName[i]),)
		}

	}



}

void Room::SaveData()
{}

void Room::LoadData()
{}

DirectX::BoundingBox Room::GetBox()const
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

bool Room::IsInside(const Math::Vector3& pos) const
{

	if (GetBox().Contains(DirectX::XMLoadFloat3(&pos)))
	{
		return true;
	}
	
	return false;
}

void Room::SetCollisionType(const std::string typeName)
{
	for (int i = 0; i < std::size(m_roomTypeName); i++)
	{
		if (typeName == m_roomTypeName[i])
		{
			m_roomType = static_cast<RoomType>(i);
		}
	}
}
