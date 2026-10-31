#include "RoomGate.h"

#include<filesystem>

#include"../../../Editor/EditorManager.h"
#include"../../../System/CollisionManager/CollisionManager.h"
#include"../../../System/CollisionManager/CollisionMath/CollisionMath.h"
#include"../../../System/TimeManager/TimeManager.h"
#include"../../../Scene/SceneManager.h"

#include"../Room/Room.h"

void RoomGate::Init()
{
	SetObjectName("RoomGate");

	SetObjectCategory(KdGameObject::ObjectCategory::Gimmick);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// Scaleは当たり判定の箱の大きさ(横, 高さ, 厚み)
	SetScale({ 2.2f,3.0f,0.5f });

	CollisionManager::Instance().RegisterObject(CollisionLayer::Gate, shared_from_this());

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("RoomGate",GetCollisionBox(),KdCollider::Type::TypeBump);

	LoadModel();
}

void RoomGate::Update()
{
	auto spRoom = GetLinkedRoom();

	// 部屋が見つからない柵は、開いたままにしておく
	const bool shouldClose = spRoom && spRoom->ShouldCloseGate();

	// 最初の1フレームは、動かさずに今の状態へ合わせる
	if (m_needSnap)
	{
		m_openRate = shouldClose ? 0.0f : 1.0f;
		m_shouldClose = shouldClose;
		m_needSnap = false;
		return;
	}

	const float deltaTime = TimeManager::Instance().GetDeltaTime();

	if (shouldClose != m_shouldClose)
	{
		m_shouldClose = shouldClose;

		auto spRoom = m_wpRoom.lock();
		if (spRoom)
		{
			if(spRoom->GetRoomType()!=Room::RoomType::Boss)
			{
				KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Stage/Close/Close.wav");
			}
		}
	}

	if (shouldClose)
	{
		m_openRate -= deltaTime / std::max(m_closeDuration, 0.01f);
	}
	else
	{
		m_openRate += deltaTime / std::max(m_openDuration, 0.01f);
	}

	m_openRate = std::clamp(m_openRate, 0.0f, 1.0f);

}

void RoomGate::DrawLit()
{
	if (!m_spModel)
	{
		return;
	}

	// モデルは大きさ(Scale)を使わず、回転と位置だけで描く
	const Math::Matrix mat =
		Math::Matrix::CreateFromQuaternion(GetRotationQuaternion())
		* Math::Matrix::CreateTranslation(GetPos() + Math::Vector3(0.0f, GetLift(), 0.0f));

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, mat);
}

void RoomGate::DrawDebug()
{
	// Active Stageのギミックだけ、デバッグ表示がONのとき(または選択中)に箱を出す
	if (!ShouldDrawDebug())
	{
		KdGameObject::DrawDebug();
		return;
	}

	Math::Color color;

	if (IsSelected())
	{
		color = kRedColor + kGreenColor;	// 黄:選択中
	}
	else if (IsBlocking())
	{
		color = kRedColor;					// 赤:通れない
	}
	else
	{
		color = kGreenColor;				// 緑:開いている
	}

	const Math::Matrix mat =
		Math::Matrix::CreateScale(GetScale())
		* Math::Matrix::CreateFromQuaternion(GetRotationQuaternion())
		* Math::Matrix::CreateTranslation(GetBoxCenter());

	m_pDebugWire->AddDebugBox(mat, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, true, color);

	KdGameObject::DrawDebug();
}

void RoomGate::DrawInspectorHeader()
{
	KdGameObject::DrawBasicInspecter();
}

void RoomGate::DrawInspector()
{
	// 追従するステージ
	m_stageFollower.DrawInspector();

	// 紐づける部屋
	const char* preview = m_linkedRoomName.empty() ? "(None)" : m_linkedRoomName.c_str();

	if (ImGui::BeginCombo("LinkedRoom", preview))
	{
		if (ImGui::Selectable("(None)", m_linkedRoomName.empty()))
		{
			if (!m_linkedRoomName.empty())
			{
				m_linkedRoomName.clear();
				m_wpRoom.reset();
				m_warnedMissingRoom = false;

				EditorManager::Instance().MarkDirty();
			}
		}

		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			auto room = std::dynamic_pointer_cast<Room>(obj);

			if (!room)
			{
				continue;
			}

			const std::string name = room->GetObjectName();

			if (ImGui::Selectable(name.c_str(), name == m_linkedRoomName))
			{
				if (name != m_linkedRoomName)
				{
					m_linkedRoomName = name;
					m_wpRoom = room;
					m_warnedMissingRoom = false;

					EditorManager::Instance().MarkDirty();
				}
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::DragFloat("OpenHeight", &m_openHeight, 0.05f, 0.0f, 30.0f))
	{
		EditorManager::Instance().MarkDirty();
	}

	if (ImGui::DragFloat("OpenDuration", &m_openDuration, 0.05f, 0.01f, 30.0f))
	{
		EditorManager::Instance().MarkDirty();
	}

	if (ImGui::DragFloat("CloseDuration", &m_closeDuration, 0.05f, 0.01f, 30.0f))
	{
		EditorManager::Instance().MarkDirty();
	}

	// モデルのパス(Enterで読み込み直す)
	char buffer[256] = {};
	strncpy_s(buffer, sizeof(buffer), m_modelPath.c_str(), _TRUNCATE);

	if (ImGui::InputText("ModelPath", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		m_modelPath = buffer;

		LoadModel();

		EditorManager::Instance().MarkDirty();
	}

	ImGui::Text("OpenRate : %.2f", m_openRate);
}

nlohmann::json RoomGate::SaveData() const
{
	nlohmann::json json = KdGameObject::SaveData();

	m_stageFollower.SaveData(json);

	json["LinkedRoom"] = m_linkedRoomName;
	json["OpenHeight"] = m_openHeight;
	json["OpenDuration"] = m_openDuration;
	json["CloseDuration"] = m_closeDuration;
	json["ModelPath"] = m_modelPath;

	return json;
}

void RoomGate::LoadData(const nlohmann::json& json)
{
	m_stageFollower.LoadData(json);

	if (json.contains("LinkedRoom"))
	{
		m_linkedRoomName = json["LinkedRoom"].get<std::string>();
	}

	if (json.contains("OpenHeight"))
	{
		m_openHeight = json["OpenHeight"].get<float>();
	}

	if (json.contains("OpenDuration"))
	{
		m_openDuration = json["OpenDuration"].get<float>();
	}

	if (json.contains("CloseDuration"))
	{
		m_closeDuration = json["CloseDuration"].get<float>();
	}

	if (json.contains("ModelPath"))
	{
		const std::string path = json["ModelPath"].get<std::string>();

		if (path != m_modelPath)
		{
			m_modelPath = path;

			LoadModel();
		}
	}
}

DirectX::BoundingOrientedBox RoomGate::GetCollisionBox() const
{
	DirectX::BoundingOrientedBox box;

	box.Center = GetBoxCenter();

	box.Extents =
	{
		GetScale().x * 0.5f,
		GetScale().y * 0.5f,
		GetScale().z * 0.5f
	};

	const Math::Quaternion quaternion = GetRotationQuaternion();

	box.Orientation =
	{
		quaternion.x,
		quaternion.y,
		quaternion.z,
		quaternion.w
	};

	return box;
}

void RoomGate::GetWorldBounds(Math::Vector3& outMin, Math::Vector3& outMax) const
{
	CollisionMath::OBBtoAABB(GetCollisionBox(), outMin, outMax);
}

bool RoomGate::OverlapSphere(const DirectX::BoundingSphere & sphere, Math::Vector3 & outPush, Math::Vector3 & outNormal) const
{
	return CollisionMath::SphereVsOBB(sphere, GetCollisionBox(), outPush, outNormal);
}

bool RoomGate::SweepSphere(const Math::Vector3& start, const Math::Vector3& move, float radius, float& outToi, Math::Vector3& outNormal) const
{
	return CollisionMath::SphereSweepVsOBB(start, move, radius, GetCollisionBox(), outToi, outNormal);
}


std::shared_ptr<Room> RoomGate::FindRoomByName(const std::string& name) const
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto room = std::dynamic_pointer_cast<Room>(obj);

		if (room && room->GetObjectName() == name)
		{
			return room;
		}
	}

	return nullptr;
}

std::shared_ptr<Room> RoomGate::GetLinkedRoom()
{
	if (m_linkedRoomName.empty())
	{
		return nullptr;
	}

	auto spRoom = m_wpRoom.lock();

	if (spRoom)
	{
		return spRoom;
	}

	// 見失っていたら、名前で探し直す
	spRoom = FindRoomByName(m_linkedRoomName);

	if (spRoom)
	{
		m_wpRoom = spRoom;
		return spRoom;
	}

	// 見つからないときは、ログを1回だけ出す
	if (!m_warnedMissingRoom)
	{
		KdDebugGUI::Instance().AddLog("RoomGate : linked room not found : %s\n", m_linkedRoomName.c_str());

		m_warnedMissingRoom = true;
	}

	return nullptr;
}

void RoomGate::LoadModel()
{
	m_spModel = nullptr;

	// ファイルが無いときは、モデル無し(デバッグの箱だけ)で動かす
	if (m_modelPath.empty() || !std::filesystem::exists(m_modelPath))
	{
		return;
	}

	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData(m_modelPath);
}

float RoomGate::GetLift() const
{
	// 開き始めと開き終わりをゆっくりにする(smoothstep)
	const float eased = m_openRate * m_openRate * (3.0f - 2.0f * m_openRate);

	return m_openHeight * eased;
}

Math::Vector3 RoomGate::GetBoxCenter() const
{
	// Posは足元の中心。箱の高さの半分だけ上が箱の中心で、開くほど持ち上がる
	return GetPos() + Math::Vector3(0.0f, GetScale().y * 0.5f + GetLift(), 0.0f);
}

Math::Quaternion RoomGate::GetRotationQuaternion() const
{
	const Math::Vector3 rot = GetRotation();

	return Math::Quaternion::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(rot.y),
		DirectX::XMConvertToRadians(rot.x),
		DirectX::XMConvertToRadians(rot.z));
}
