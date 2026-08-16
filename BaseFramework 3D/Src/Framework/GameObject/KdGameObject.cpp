#include "KdGameObject.h"

#include"../../Application/Editor/EditorManager.h"

void KdGameObject::DrawDebug()
{
	// 早期リターン
	if (!m_pDebugWire)return;

	m_pDebugWire->Draw();
}

void KdGameObject::SetScale(float scalar)
{
	Math::Vector3 scale(scalar);

	SetScale(scale);
}

void KdGameObject::SetScale(const Math::Vector3& scale)
{
	Math::Vector3 vecX = m_mWorld.Right();
	Math::Vector3 vecY = m_mWorld.Up();
	Math::Vector3 vecZ = m_mWorld.Backward(); 
	
	vecX.Normalize();
	vecY.Normalize();
	vecZ.Normalize();

	m_mWorld.Right(vecX * scale.x);
	m_mWorld.Up(vecY * scale.y);
	m_mWorld.Backward(vecZ * scale.z);
}

Math::Vector3 KdGameObject::GetScale() const
{
	return Math::Vector3(m_mWorld.Right().Length(), m_mWorld.Up().Length(), m_mWorld.Backward().Length());
}

void KdGameObject::SetRotation(const Math::Vector3& rotation)
{
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(rotation.y),
		DirectX::XMConvertToRadians(rotation.x),
		DirectX::XMConvertToRadians(rotation.z));

	
	Math::Matrix transMat = Math::Matrix::CreateTranslation(GetPos());
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());


	m_mWorld = scaleMat * rotMat * transMat;

	m_rotation = rotation;
}


void KdGameObject::CalcDistSqrFromCamera(const Math::Vector3& camPos)
{
	m_distSqrFromCamera = (m_mWorld.Translation() - camPos).LengthSquared();
}

bool KdGameObject::Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetBox, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

void KdGameObject::DrawInspector()
{

	// オブジェクトの名前変更
	char nameBaffer[128];

	strcpy_s(
		nameBaffer,
		sizeof(nameBaffer),
		GetObjectName().c_str()
	);

	if (ImGui::InputText("Name", nameBaffer, sizeof(nameBaffer)))
	{
		SetObjectName(nameBaffer);
		EditorManager::Instance().MarkDirty();
	}

	// 座標変更
	Math::Vector3 pos = GetPos();

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetPos(pos);
		EditorManager::Instance().MarkDirty();
	}

	Math::Vector3 scale = GetScale();

	// 大きさ変更
	if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
	{
		SetScale(scale);
		EditorManager::Instance().MarkDirty();
	}

	// 回転
	Math::Vector3 rotation = GetRotation();

	if (ImGui::DragFloat3("Rotation", &rotation.x, 1))
	{
		SetRotation(rotation);
		EditorManager::Instance().MarkDirty();
	}

}

nlohmann::json KdGameObject::SaveData() const
{
	nlohmann::json json;

	json["Class"] = GetObjectName();

	json["Name"] = GetObjectName();

	Math::Vector3 pos = GetPos();

	json["Position"] =
	{
		{"x",pos.x},
		{"y",pos.y},
		{"z",pos.z}
	};

	Math::Vector3 scale = GetScale();

	json["Scale"] =
	{
		{"x",scale.x},
		{"y",scale.y},
		{"z",scale.z}
	};

	Math::Vector3 rotation= GetRotation();

	json["Rotation"] =
	{
		{"x",rotation.x},
		{"y",rotation.y},
		{"z",rotation.z}
	};

	return json;
}
