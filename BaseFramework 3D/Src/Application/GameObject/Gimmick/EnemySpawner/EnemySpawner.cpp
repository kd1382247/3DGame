#include "EnemySpawner.h"

#include"../../../System/TimeManager/TimeManager.h"
#include"../../../Scene/SceneManager.h"

#include"../../Character/Enemy/EnemyBase.h"

#include"../../../System/ReferenceManager/ReferenceManager.h"
#include"../../../../Framework/GameObject/KdGameObjectFactory.h"

#include"../../../System/CollisionManager/CollisionManager.h"
#include"../../../System/CollisionManager/CollisionMath/CollisionMath.h"

#include"../../../Editor/InspectorWidgets.h"

#include"../Room/Room.h"

void EnemySpawner::Init()
{

	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Gimmick/EnemySpawner.gltf");

		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		// 当たり判定は球(クリック選択用)。モデルの形ではなく、半径kColliderRadiusの球にする
		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape(
			"EnemySpawner",
			Math::Vector3(0.0f,0.0f, 0.0f),
			kColliderRadius,
			KdCollider::TypeEvent|KdCollider::TypeBump);

		CollisionManager::Instance().RegisterObject(CollisionLayer::Gimmick,shared_from_this());
	}

	SetObjectName("EnemySpawner");
	SetObjectCategory(KdGameObject::ObjectCategory::Gimmick);

	SetPos({ 0.0f,0.0f,0.0f });

}

void EnemySpawner::Update()
{
	// 倒された敵をリストから削除
	EraseEnemy();

	// スポナーから出終わった敵を、押し戻し対象に戻す
	UpdateEmergingEnemies();

	auto spRoom = GetLinkedRoom();

	if (!spRoom)
	{
		return;
	}

	if (!spRoom->IsCombat()||IsFinished())
	{
		return;
	}

	if (m_currentWave >= static_cast<int>(m_waves.size()))
	{
		return;
	}


	if (m_spawnedInWave == 0)
	{
		m_waitTime = m_waves[m_currentWave].m_startDelay;

		// 敵が全滅していたら次のウェーブに進める
		if (!IsAllEnemiesDead())
		{
			return;
		}
	}
	else
	{
		m_waitTime = m_waves[m_currentWave].m_spawnInterval;
	}

	m_waveTimer += TimeManager::Instance().GetDeltaTime();

	// 少しずつ敵を生成する
	if (m_waveTimer >= m_waitTime)
	{
		SpawnEnemy();
		m_waveTimer = 0.0f;
		m_spawnedInWave++;
	}

	// 現在のウェーブで敵を生成しきったら、次のウェーブに進める
	if (m_spawnedInWave >= m_waves[m_currentWave].m_spawnCount)
	{
		m_currentWave++;
		// 配列の要素を超えないようにクランプ
		if (m_currentWave >= static_cast<int>(m_waves.size()))
		{
			m_currentWave = static_cast<int>(m_waves.size());
		}
		m_spawnedInWave = 0;

		m_spawnIntroEffect = false;
	}

}

void EnemySpawner::DrawLit()
{
	if (m_spModel)
	{
		// 選択中はリムライトで光らせる(プレイヤーなどと同じ)
		if (IsSelected())
		{
			KdShaderManager::Instance().m_StandardShader.SetSelected(true);
		}

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);

		KdShaderManager::Instance().m_StandardShader.SetSelected(false);
	}
}

void EnemySpawner::DrawDebug()
{
	// Active Stageのスポナーだけ、デバッグ表示がONのとき(または選択中)に球を出す
	if (!ShouldDrawDebug())
	{
		KdGameObject::DrawDebug();
		return;
	}

	// 選択中は黄色、通常はオレンジ
	Math::Color color = Math::Color(1.0f, 0.5f, 0.0f, 1.0f);

	if (IsSelected())
	{
		color = kRedColor + kGreenColor;
	}

	m_pDebugWire->AddDebugSphere(GetColliderCenter(), kColliderRadius, color);

	KdGameObject::DrawDebug();
}

void EnemySpawner::DrawInspectorHeader()
{
	KdGameObject::DrawBasicInspecter();

	ImGui::Separator();

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

		// オブジェクトリストから部屋を探す
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

	// ウェーブを追加
	if (ImGui::Button("Add Wave"))
	{
		// 初期値を入れる
		Wave wave;
		wave.m_enemyNames.emplace(SpawnableEnemies::Names[0]);
		wave.m_spawnCount = 1;
		wave.m_spawnInterval = 0.2f;
		wave.m_startDelay = 2.0f;

		m_waves.push_back(wave);

		EditorManager::Instance().MarkDirty();
	}
}

void EnemySpawner::DrawInspector()
{
	ImGui::Text("Waves : %d", static_cast<int>(m_waves.size()));

	// ウェーブを消す用の番号
	int removeIndex = -1;

	for (int i = 0; i < m_waves.size(); i++)
	{
		ImGui::PushID(i);

		std::string waveName = "Wave" + std::to_string(i + 1);
		if (ImGui::CollapsingHeader(waveName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			InspectorUI::Count  ("SpawnCount",    m_waves[i].m_spawnCount, 1);
			InspectorUI::Seconds("SpawnInterval", m_waves[i].m_spawnInterval);
			InspectorUI::Seconds("StartDelay",    m_waves[i].m_startDelay);

			// スポーンの対象の敵の名前をループで表示
			for (const auto& enemyName : SpawnableEnemies::Names)
			{
				bool checked = m_waves[i].m_enemyNames.contains(enemyName);

				// 敵を出現させるかのチェックボックス
				if (ImGui::Checkbox(enemyName.c_str(), &checked))
				{
					if (checked)
					{
						m_waves[i].m_enemyNames.insert(enemyName);
					}
					else
					{
						m_waves[i].m_enemyNames.erase(enemyName);
					}
						
					EditorManager::Instance().MarkDirty();
				}
			}

			// 現在のループの回数を入れる
			if (ImGui::Button("RemoveWave"))
			{
				removeIndex = i;
			}
		}
		ImGui::PopID();
	}

	if (removeIndex >= 0)
	{
		m_waves.erase(m_waves.begin() + removeIndex);
		EditorManager::Instance().MarkDirty();
	}

}

bool EnemySpawner::IsFinished() const
{
	return m_currentWave>= static_cast<int>(m_waves.size())
		&& IsAllEnemiesDead();
}

nlohmann::json EnemySpawner::SaveData() const
{
	nlohmann::json roomJson = KdGameObject::SaveData();

	m_stageFollower.SaveData(roomJson);

	roomJson["LinkedRoom"] = m_linkedRoomName;

	roomJson["Waves"] = nlohmann::json::array();

	for (const auto& wave : m_waves)
	{
		nlohmann::json waveJson;

		waveJson["SpawnCount"] = wave.m_spawnCount;
		waveJson["SpawnInterval"] = wave.m_spawnInterval;
		waveJson["StartDelay"] = wave.m_startDelay;
		waveJson["Enemies"] = wave.m_enemyNames;

		roomJson["Waves"].push_back(waveJson);
	}

	return roomJson;

}

void EnemySpawner::LoadData(const nlohmann::json& json)
{
	m_stageFollower.LoadData(json);

	if (json.contains("LinkedRoom"))
	{
		m_linkedRoomName = json["LinkedRoom"].get<std::string>();
	}

	m_wpRoom.reset();
	m_warnedMissingRoom = false;

	if (json.contains("Waves") && json["Waves"].is_array())
	{
		m_waves.clear();

		for (const auto& waveJson : json["Waves"])
		{
			Wave wave;

			wave.m_spawnCount = waveJson.value("SpawnCount", 1);
			wave.m_spawnInterval = waveJson.value("SpawnInterval", 0.0f);
			wave.m_startDelay = waveJson.value("StartDelay", 0.0f);

			// 敵の名前を追加
			if (waveJson.contains("Enemies") && waveJson["Enemies"].is_array())
			{
				for (const auto& enemyName : waveJson["Enemies"])
				{
					wave.m_enemyNames.emplace(enemyName.get<std::string>());
				}
			}

			m_waves.push_back(wave);
		}
	}

}

void EnemySpawner::SpawnEnemy()
{
	if (!m_spawnIntroEffect)
	{
		m_spawnIntroEffect = true;
		KdEffekseerManager::GetInstance().Play("EnemySpawner/smoke.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 1.2f,0.7);
		KdEffekseerManager::GetInstance().Play("EnemySpawner/Lightning.efkefc", GetPos() + Math::Vector3(0.0f, -1.0f, 0.0f), 0.6f, 1.0f, false, 20, 90);

		KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Gimmick/EnemySpawner/Spark.wav");

	}

	const auto& src = m_waves[m_currentWave].m_enemyNames;

	std::vector<std::string>enemyNames;

	// std::setをsstd::vectorに移動。
	enemyNames.reserve(src.size());
	enemyNames.insert(enemyNames.end(), src.begin(), src.end());

	if (enemyNames.empty())
	{
		return;
	}

	// 名前リストのサイズでランダムな数字を入れる
	int nameIndex = KdRandom::GetInt(0, static_cast<int>(enemyNames.size() - 1));

	CreateEnemy(enemyNames[nameIndex]);

}

void EnemySpawner::EraseEnemy()
{
	// 倒れた敵のリストを整理
	std::erase_if(m_spawnEnemyList,
		[](const std::weak_ptr<EnemyBase>& enemy)
		{
			auto spEnemy = enemy.lock();

			return !spEnemy || spEnemy->IsExpired();
		}
	);
}

Math::Vector3 EnemySpawner::CreateLaunchDir() const
{

	float power = KdRandom::GetFloat(0.05f, 0.08f);

	Math::Vector3 randomDir;

	randomDir.x = KdRandom::GetFloat(-1.0f, 1.0f);
	randomDir.z = KdRandom::GetFloat(-1.0f, 1.0f);
	randomDir.y = 0;

	randomDir.Normalize();

	return randomDir * power;

}

void EnemySpawner::CreateEnemy(const std::string& enemyName)
{
	if (!KdGameObjectFactory::Instance().GetCreateFunctions().contains(enemyName))
	{
		return;
	}

	auto obj = KdGameObjectFactory::Instance().CreateGameObject(enemyName);

	auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

	if (!enemy)
	{
		return;
	}

	enemy->Init();
	enemy->SetPos(GetPos());
	enemy->SetPrevPos(GetPos());

	float power = KdRandom::GetFloat(0.3f,0.5f);
	Math::Vector3 launchDir = CreateLaunchDir();
	// 飛び出す方向をセット
	enemy->Launch(launchDir,power);

	// 関連付け
	enemy->SetUpReference();

	SceneManager::Instance().AddObject(enemy);

	m_spawnEnemyList.push_back(enemy);

	// スポナーの当たり判定の中から出てくるので、外に出るまでは押し戻さない
	m_emergingEnemies.push_back(enemy);

}

bool EnemySpawner::IsBlockingCharacter(const CharacterBase* character) const
{
	for (const auto& wpEnemy : m_emergingEnemies)
	{
		auto spEnemy = wpEnemy.lock();

		if (spEnemy && spEnemy.get() == character)
		{
			return false;
		}
	}

	return true;
}

void EnemySpawner::UpdateEmergingEnemies()
{
	std::erase_if(m_emergingEnemies,
		[this](const std::weak_ptr<EnemyBase>& wpEnemy)
		{
			auto spEnemy = wpEnemy.lock();

			if (!spEnemy || spEnemy->IsExpired())
			{
				return true;
			}

			Math::Vector3 push = Math::Vector3::Zero;
			Math::Vector3 normal = Math::Vector3::Zero;

			// スポナーの当たり判定と重ならなくなったら、出終わったとみなす
			return !CollisionMath::SphereVsCylinder(
				spEnemy->GetBumpSphere(),
				GetColliderCenter(),
				kColliderRadius,
				kColliderRadius,
				push,
				normal);
		});
}

std::shared_ptr<Room> EnemySpawner::FindRoomByName(const std::string& name) const
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

std::shared_ptr<Room> EnemySpawner::GetLinkedRoom()
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
		KdDebugGUI::Instance().AddLog("EnemySpawner : linked room not found : %s\n", m_linkedRoomName.c_str());

		m_warnedMissingRoom = true;
	}

	return nullptr;
}


