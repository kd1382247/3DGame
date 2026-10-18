#include "Room.h"

#include"../../../Editor/EditorManager.h"
#include"RoomManager.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../System/TimeManager/TimeManager.h"
#include"../../../Scene/SceneManager.h"
#include"../../Gimmick/EnemySpawner/EnemySpawner.h"

#include"../../../GameObject/Character/Player/Player.h"
#include"../../Character/Boss/BossBase.h"
#include"../../Character/Enemy/EnemyBase.h"

void Room::Init()
{
	SetObjectName("Room");

	SetObjectCategory(KdGameObject::ObjectCategory::Gimmick);


	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Room", GetBox(), KdCollider::TypeEvent);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	SetScale({ 10,6,10 });
	SetPos({ 5.0f,0.0f,0.0f });

}

void Room::Update()
{
	
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	m_bodyRadius = spPlayer->GetBumpSphereRadius();

	bool isInside = IsInside(spPlayer->GetPos());

	if (m_isPlayerInside != isInside)
	{
		if (isInside)
		{
			RoomManager::Instance().EnterRoom(
			std::dynamic_pointer_cast<Room>(shared_from_this()));
		}
		else
		{
			RoomManager::Instance().ExitRoom(
				std::dynamic_pointer_cast<Room>(shared_from_this()));
		}

		m_isPlayerInside = isInside;
	}

	bool isPlayerInCore = IsInsideCore(spPlayer->GetPos(), m_bodyRadius);

	if (m_isPlayerInCore != isPlayerInCore)
	{
		if (isPlayerInCore)
		{
			// 初めて入ったときだけ、戦闘前の演出に進む
			if (m_roomState == RoomState::Unvisited)
			{
				if (m_roomType == RoomType::Safe)
				{
					ChangeState(RoomState::Cleared);
				}
				else
				{
					ChangeState(RoomState::Intro);

					m_waitBossIntro = StartBossIntro();
				}
			}
		}

		m_isPlayerInCore = isPlayerInCore;
	}

	// 状態ごとの処理
	switch (m_roomState)
	{
	case RoomState::Intro:
		UpdateIntro();
		break;

	case RoomState::InCombat:
		UpdateCombat();
		break;

	case RoomState::BossDefeat:
		UpdateBossDefeat();
		break;

	case RoomState::Outro:
		UpdateOutro();
		break;
	}

}

void Room::DrawDebug()
{
	// Active Stageのギミックだけ、デバッグ表示がONのとき(または選択中)に箱を出す
	if (!ShouldDrawDebug())
	{
		KdGameObject::DrawDebug();
		return;
	}

	// 外の箱の色は、部屋の状態で変える
	Math::Color color;

	switch (m_roomState)
	{
	case RoomState::Unvisited: color = kBlueColor;             break;	// 青:未訪問
	case RoomState::Intro:     color = kRedColor + kBlueColor; break;	// 紫:戦闘前の演出中
	case RoomState::InCombat:  color = kRedColor;              break;	// 赤:戦闘中
	case RoomState::BossDefeat:color = Math::Color(1.0f, 1.0f, 1.0f, 1.0f); break;	// 白:ボス撃破の演出中
	case RoomState::Outro:     color = Math::Color(0.5f, 1.0f, 0.5f, 1.0f); break;	// 薄緑:扉が開く演出中
	case RoomState::Cleared:   color = kGreenColor;            break;	// 緑:クリア済み
	}

	// 選択中は黄色
	if (IsSelected())
	{
		color = kRedColor + kGreenColor;
	}

	DirectX::BoundingBox core = GetCoreBox(m_bodyRadius);

	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, true, color);

	// 内側の箱(入室完了の判定範囲)を水色で表示する
	// 外の箱と大きさが違うので、m_mWorld(Scale入り)は使わず、
	// 「位置だけの行列」と「半分の大きさ(Extents)」をそのまま渡す
	m_pDebugWire->AddDebugBox(
		Math::Matrix::CreateTranslation(core.Center),
		core.Extents,
		Math::Vector3::Zero,
		true,
		kGreenColor + kBlueColor);

	KdGameObject::DrawDebug();
}

void Room::DrawInspectorHeader()
{
	KdGameObject::DrawBasicInspecter();
}

void Room::DrawInspector()
{
	// 追従するステージ
	m_stageFollower.DrawInspector();

	// 内側の箱(バトル開始の判定範囲)の位置と大きさ
	if (ImGui::DragFloat3("CoreOffset", &m_coreOffset.x, 0.05f))
	{
		EditorManager::Instance().MarkDirty();
	}

	if (ImGui::DragFloat3("CoreSize", &m_coreSize.x, 0.05f, 0.1f, 1000.0f))
	{
		EditorManager::Instance().MarkDirty();
	}

	if (ImGui::DragFloat("IntroDuration", &m_introDuration, 0.05f, 0.0f, 30.0f))
	{
		EditorManager::Instance().MarkDirty();
	}

	if (ImGui::DragFloat("OutroDuration", &m_outroDuration, 0.05f, 0.0f, 30.0f))
	{
		EditorManager::Instance().MarkDirty();
	}

	// ボスを倒した演出は、ボス部屋だけで使う
	if (m_roomType == RoomType::Boss)
	{
		if (ImGui::DragFloat("BossDefeatDuration", &m_bossDefeatDuration, 0.05f, 0.0f, 30.0f))
		{
			EditorManager::Instance().MarkDirty();
		}
	}

	std::string roomTypeName = m_roomTypeName[static_cast<int>(m_roomType)];

	if (ImGui::BeginCombo("RoomType", roomTypeName.c_str()))
	{
		for (int i = 0; i < std::size(m_roomTypeName); i++)
		{

			if (ImGui::Selectable(m_roomTypeName[i].c_str(),
				i == static_cast<int>(m_roomType)))
			{

				if (i != static_cast<int>(m_roomType))
				{
					SetRoomType(static_cast<RoomType>(i));

					EditorManager::Instance().MarkDirty();
						
				}
			}
		}
		ImGui::EndCombo();
	}
}

void Room::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

nlohmann::json Room::SaveData() const
{
	nlohmann::json roomJson = KdGameObject::SaveData();
	
	roomJson["CoreOffset"] =
	{
		{"x",m_coreOffset.x},
		{"y",m_coreOffset.y},
		{"z",m_coreOffset.z}
	};

	roomJson["CoreSize"] =
	{
		{"x",m_coreSize.x},
		{"y",m_coreSize.y},
		{"z",m_coreSize.z}
	};

	m_stageFollower.SaveData(roomJson);

	roomJson["IntroDuration"] = m_introDuration;
	roomJson["BossDefeatDuration"] = m_bossDefeatDuration;
	roomJson["OutroDuration"] = m_outroDuration;
	roomJson["RoomType"] = GetRoomTypeName();

	return roomJson;

}

void Room::LoadData(const nlohmann::json& json)
{
	m_stageFollower.LoadData(json);

	if (json.contains("CoreOffset"))
	{
		m_coreOffset.x = json["CoreOffset"]["x"].get<float>();
		m_coreOffset.y = json["CoreOffset"]["y"].get<float>();
		m_coreOffset.z = json["CoreOffset"]["z"].get<float>();
	}

	if (json.contains("CoreSize"))
	{
		m_coreSize.x = json["CoreSize"]["x"].get<float>();
		m_coreSize.y = json["CoreSize"]["y"].get<float>();
		m_coreSize.z = json["CoreSize"]["z"].get<float>();
	}
	else if (json.contains("EnterDistance"))
	{
		// 古い保存データ:以前と同じ大きさになるように、外側の箱をEnterDistanceだけ縮めた大きさにする
		// (このときScaleは、LoadDataの前に読み込み済み)
		const float enterDistance = json["EnterDistance"].get<float>();

		m_coreSize.x = std::max(0.1f, GetScale().x - enterDistance * 2.0f);
		m_coreSize.y = GetScale().y;
		m_coreSize.z = std::max(0.1f, GetScale().z - enterDistance * 2.0f);
	}

	if (json.contains("IntroDuration"))
	{
		m_introDuration = json["IntroDuration"].get<float>();
	}

	if (json.contains("BossDefeatDuration"))
	{
		m_bossDefeatDuration = json["BossDefeatDuration"].get<float>();
	}

	if (json.contains("OutroDuration"))
	{
		m_outroDuration = json["OutroDuration"].get<float>();
	}

	if (json.contains("RoomType"))
	{
		const std::string roomTypeName = json["RoomType"].get<std::string>();

		for (int i = 0; i < std::size(m_roomTypeName); i++)
		{
			if (roomTypeName == m_roomTypeName[i])
			{
				SetRoomType(static_cast<RoomType>(i));
			}
		}
	}

}

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
	// Boxの中にいたらtrue
	return GetBox().Contains(DirectX::XMLoadFloat3(&pos)) == DirectX::CONTAINS;
}

bool Room::IsInsideCore(const Math::Vector3& pos, float bodyRadius) const
{
	// 体の半径の分だけ縮めて、体が全部入ってから「入った」とする
	return GetCoreBox(bodyRadius).Contains(DirectX::XMLoadFloat3(&pos)) == DirectX::CONTAINS;
}

DirectX::BoundingBox Room::GetCoreBox(float margin) const
{
	DirectX::BoundingBox box;

	// 内側の箱は、部屋の中心からのずれと、決めた大きさで作る
	box.Center = GetPos() + m_coreOffset;

	box.Extents =
	{
		std::max(0.0f, m_coreSize.x * 0.5f - margin),
		m_coreSize.y * 0.5f,
		std::max(0.0f, m_coreSize.z * 0.5f - margin)
	};

	return box;
}

void Room::SetRoomType(const std::string& typeName)
{
	for (int i = 0; i < std::size(m_roomTypeName); i++)
	{
		if (typeName == m_roomTypeName[i])
		{
			SetRoomType(static_cast<RoomType>(i));
		}
	}
}

// ログに出す状態名
static const char* GetRoomStateName(const Room::RoomState state)
{
	switch (state)
	{
	case Room::RoomState::Unvisited: return "Unvisited";
	case Room::RoomState::Intro:     return "Intro";
	case Room::RoomState::InCombat:  return "InCombat";
	case Room::RoomState::BossDefeat:return "BossDefeat";
	case Room::RoomState::Outro:     return "Outro";
	case Room::RoomState::Cleared:   return "Cleared";
	}

	return "";
}

bool Room::StartBossIntro()
{

	if (m_roomType != RoomType::Boss)
	{
		return false;
	}

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto boss = std::dynamic_pointer_cast<BossBase>(obj);
		
		if (!boss)
		{
			continue;
		}

		if (IsInside(boss->GetPos()))
		{
			boss->StartIntro();
			return true;
		}
	}


	return false;
}

void Room::ChangeState(RoomState next)
{
	m_roomState = next;

	// 状態が変わるたびに、経過時間を数え直す
	m_stateTimer = 0.0f;
}

void Room::FinishIntro()
{
	// 演出中でなければ何もしない
	if (m_roomState != RoomState::Intro)
	{
		return;
	}

	m_waitBossIntro = false;

	ChangeState(RoomState::InCombat);
}

void Room::UpdateIntro()
{
	if (m_waitBossIntro)
	{
		return;
	}

	// 演出の仕組みができるまでは、決めた時間が経ったら演出が終わったことにする
	m_stateTimer += TimeManager::Instance().GetDeltaTime();

	if (m_stateTimer >= m_introDuration)
	{
		FinishIntro();
	}
}

void Room::UpdateCombat()
{
	
	bool isFinished = SpawnerFinished();
	if (!isFinished)
	{
		return;
	}

	// 部屋の中の生きている敵が0体になったら、クリア後の演出に進む
	if (CountAliveEnemies() == 0)
	{
		// ボス部屋は、先にボスを倒した演出を見せてから扉の演出に進む
		if (m_roomType == RoomType::Boss)
		{
			ChangeState(RoomState::BossDefeat);
		}
		else
		{
			ChangeState(RoomState::Outro);
		}
	}
}

void Room::StartBossDefeat()
{
	// 戦闘中のボス部屋だけ受け付ける
	if (m_roomState != RoomState::InCombat || m_roomType != RoomType::Boss)
	{
		return;
	}

	ChangeState(RoomState::BossDefeat);

	// ボスが演出を終えて FinishBossDefeat() を呼ぶまで、時間では終わらせない
	m_waitBossDefeat = true;

	// 部屋の中の敵を、ボスと同時に全員倒す
	KillAllEnemies();
}

void Room::FinishBossDefeat()
{
	// ボスを倒した演出中でなければ何もしない
	if (m_roomState != RoomState::BossDefeat)
	{
		return;
	}

	m_waitBossDefeat = false;

	ChangeState(RoomState::Outro);
}

void Room::UpdateBossDefeat()
{
	// ボスの撃破演出が終わるのを待つ(終わらせるのは、ボス側から呼ぶFinishBossDefeat)
	if (m_waitBossDefeat)
	{
		return;
	}

	// 演出の仕組みができるまでは、決めた時間が経ったら演出が終わったことにする
	m_stateTimer += TimeManager::Instance().GetDeltaTime();

	if (m_stateTimer >= m_bossDefeatDuration)
	{
		FinishBossDefeat();
	}
}

void Room::FinishOutro()
{
	// 扉の演出中でなければ何もしない
	if (m_roomState != RoomState::Outro)
	{
		return;
	}

	ChangeState(RoomState::Cleared);
}

void Room::UpdateOutro()
{
	// 演出の仕組みができるまでは、決めた時間が経ったら演出が終わったことにする
	m_stateTimer += TimeManager::Instance().GetDeltaTime();

	if (m_stateTimer >= m_outroDuration)
	{
		FinishOutro();
	}
}

bool Room::SpawnerFinished()
{

	// スポナーが生成を終了したかどうかを調べる
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj) { continue; }

		auto spawner = std::dynamic_pointer_cast<EnemySpawner>(obj);

		if (!spawner) { continue; }

		if (spawner->GetLinkRoomName() != GetObjectName())
		{
			continue;
		}

		// 1つでもウェーブが終了していなかったら
		if (!spawner->IsFinished())
		{
			return false;
		}
	}

	return true;
}

void Room::KillAllEnemies()
{
	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

		if (!enemy)
		{
			continue;
		}

		// 部屋の外にいる敵は倒さない
		if (!IsInside(enemy->GetPos()))
		{
			continue;
		}

		enemy->ForceKill();
	}
}

int Room::CountAliveEnemies() const
{
	int count = 0;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto character = std::dynamic_pointer_cast<CharacterBase>(obj);

		if (!character)
		{
			continue;
		}

		// プレイヤーは敵ではない
		if (std::dynamic_pointer_cast<Player>(character))
		{
			continue;
		}

		// 部屋の外にいるものは数えない
		if (!IsInside(character->GetPos()))
		{
			continue;
		}

		++count;
	}

	return count;
}
