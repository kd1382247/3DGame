#include "EnemySpawner.h"

#include"../../../System/TimeManager/TimeManager.h"
#include"../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../Scene/SceneManager.h"

#include"../../Character/Enemy/EnemyBase.h"

#include"../../../System/ReferenceManager/ReferenceManager.h"
#include"../../../../Framework/GameObject/KdGameObjectFactory.h"

#include"../../Camera/CameraBase.h"

#include"../../../System/CollisionManager/CollisionManager.h"
#include"../../../System/CollisionManager/CollisionMath/CollisionMath.h"

#include"../../../Editor/InspectorWidgets.h"

#include"../Room/Room.h"

#include"../../../Scene/GameScene/GameScene.h"

namespace
{
	// カメラが近い時にプレイヤーを透けさせる設定
	// この距離より遠いと不透明。近づくほど透け始める
	constexpr float kFadeStartDist = 5.0f;
	// この距離以下で完全に透明になる
	constexpr float kFadeEndDist = 1.0f;
	// 透け具合の変化の速さ(0～1、小さいほどゆっくり)
	constexpr float kFadeSpeed = 0.15f;
	// カメラとの距離を測る、プレイヤーの足元からの高さ
	constexpr float kFadePivotHeight = 0.5f;
	// 透明度の上限
	constexpr float kFadeMaxRate = 0.8f;
}

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

		// 上下の揺れの開始位置をずらす(複数のスポナーが同じ動きにならないように)
		// 1周期ぶんの範囲でランダムにする
		m_sinCurveTime = KdRandom::GetFloat(0.0f, 1.0f / m_sinCurveFrequency);
	}

	SetObjectName("EnemySpawner");
	SetObjectCategory(KdGameObject::ObjectCategory::Gimmick);

	SetPos({ 0.0f,0.0f,0.0f });

}

void EnemySpawner::Update()
{
	// 倒された敵をリストから削除
	EraseEnemy();

	UpdateCameraFade();

	// スポナーから出終わった敵を、押し戻し対象に戻す
	UpdateEmergingEnemies();

	// 上下の揺れの時間と、生成前の回転を進める
	m_sinCurveTime += TimeManager::Instance().GetDeltaTime();
	UpdateSpin();

	auto spRoom = GetLinkedRoom();

	if (!spRoom)
	{
		return;
	}

	// ボス部屋は、ウェーブではなく、ボスからの要求で敵を出す
	// (ウェーブが無いスポナーはIsFinished()が常にtrueになるので、その判定より前に置く)
	if (spRoom->GetRoomType() == Room::RoomType::Boss)
	{
		UpdateRequest(spRoom);
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

	// 最初の1体を出す少し前に、回転を始める(1ウェーブにつき1回)
	if (m_spawnedInWave == 0 && !m_spinTriggered &&
		m_waitTime - m_waveTimer <= kSpinDuration)
	{
		StartSpin();
		m_spinTriggered = true;
	}

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
		m_spinTriggered = false;
	}

}

void EnemySpawner::DrawLit()
{
	if (!m_spModel)
	{
		return;
	}

	// 選択中はリムライトで光らせる(プレイヤーなどと同じ)
	if (IsSelected())
	{
		KdShaderManager::Instance().m_StandardShader.SetSelected(true);
	}


	Math::Matrix localMat = Math::Matrix::Identity;

	UpdateSinCurve(localMat);

	// カメラが近い時はプレイヤーを透けさせる(描画後に自動で元に戻る)
	if (m_fadeRate > 0.0f)
	{
		KdShaderManager::Instance().m_StandardShader.SetDitherRate(m_fadeRate);
	}

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, localMat);

	KdShaderManager::Instance().m_StandardShader.SetSelected(false);

}

void EnemySpawner::SetUpReference()
{
	if(!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
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
	// ボスなどからの生成要求で出す敵
	if (ImGui::CollapsingHeader("Summon", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID("Summon");

		InspectorUI::Count("MaxAlive", m_maxAlive, 1);

		for (const auto& enemyName : SpawnableEnemies::Names)
		{
			bool checked = m_summonEnemyNames.contains(enemyName);

			if (ImGui::Checkbox(enemyName.c_str(), &checked))
			{
				if (checked)
				{
					m_summonEnemyNames.insert(enemyName);
				}
				else
				{
					m_summonEnemyNames.erase(enemyName);
				}

				EditorManager::Instance().MarkDirty();
			}
		}

		ImGui::PopID();
	}

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

	// 要求(ボスの召喚など)で出す敵
	roomJson["SummonEnemies"] = m_summonEnemyNames;
	roomJson["MaxAlive"] = m_maxAlive;

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

	m_summonEnemyNames.clear();

	if (json.contains("SummonEnemies") && json["SummonEnemies"].is_array())
	{
		for (const auto& enemyName : json["SummonEnemies"])
		{
			m_summonEnemyNames.emplace(enemyName.get<std::string>());
		}
	}

	m_maxAlive = json.value("MaxAlive", 10);

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

void EnemySpawner::UpdateSinCurve(Math::Matrix& worldMat)
{
	Math::Vector3 localPos = GetPos();

	// 上下に揺らすのはプレイ中だけ(エディタ中は、置いた位置のまま見せる)
	// 時間を進める処理はUpdateで行っている
	if (EditorManager::Instance().IsPlayMode()||
		SceneManager::Instance().GetCurrentScene<GameScene>())
	{
		float angle = m_sinCurveTime * m_sinCurveFrequency * DirectX::XM_2PI;

		localPos.y += sinf(angle) * 0.25f;
	}

	// 行列作成(拡大 → 回転 → 移動の順)
	Math::Matrix scaleMat = Math::Matrix::CreateScale(GetScale());
	Math::Matrix rotMat   = Math::Matrix::CreateRotationY(m_spinAngle);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(localPos);

	worldMat = scaleMat * rotMat * transMat;
}

void EnemySpawner::StartSpin()
{
	// 回っている最中なら、そのまま続ける
	if (m_isSpinning)
	{
		return;
	}

	m_isSpinning = true;
	m_spinTimer  = 0.0f;
	m_spinAngle  = 0.0f;
}

void EnemySpawner::UpdateSpin()
{
	if (!m_isSpinning)
	{
		return;
	}

	m_spinTimer += TimeManager::Instance().GetDeltaTime();

	// 進み具合 0〜1
	const float t = std::clamp(m_spinTimer / kSpinDuration, 0.0f, 1.0f);

	// tをそのままlerpに渡すと一定の速さで回るので、
	// 「最初は速く、終わりにかけてゆっくり」になる形に直す
	const float eased = 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);

	// 0度から「kSpinTurns回転ぶん」まで、lerpで進める
	m_spinAngle = std::lerp(0.0f, kSpinTurns * DirectX::XM_2PI, eased);

	// 回り終わり:整数回転なので、0に戻しても見た目は変わらない
	if (t >= 1.0f)
	{
		m_isSpinning = false;
		m_spinAngle  = 0.0f;
	}
}

void EnemySpawner::SpawnEnemy()
{
	PlayIntroEffect();

	CreateEnemy(PickEnemyName(m_waves[m_currentWave].m_enemyNames));

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

void EnemySpawner::UpdateRequest(const std::shared_ptr<Room>& room)
{
	// ボス戦が終わったら(倒された・部屋を出たなど)、残っている要求は捨てる
	if (!room->IsBossCombat())
	{
		m_requestRemaining = 0;
		return;
	}

	if (m_requestRemaining <= 0)
	{
		return;
	}

	m_requestTimer += TimeManager::Instance().GetDeltaTime();

	// 間隔が空いていて、生きている敵が上限未満なら、1体出す
	if (m_requestTimer >= m_requestInterval &&
		static_cast<int>(m_spawnEnemyList.size()) < m_maxAlive)
	{
		// 要求の最初の1体のときだけ、演出を出す
		PlayIntroEffect();

		CreateEnemy(PickEnemyName(m_summonEnemyNames));

		m_requestRemaining--;
		m_requestTimer = 0.0f;
	}
}

std::string EnemySpawner::PickEnemyName(const std::set<std::string>& names) const
{
	if (names.empty())
	{
		return std::string();
	}

	// setの先頭からランダムな番号ぶん進めた要素を選ぶ
	const int index = KdRandom::GetInt(0, static_cast<int>(names.size()) - 1);

	return *std::next(names.begin(), index);
}

void EnemySpawner::PlayIntroEffect()
{
	if (m_spawnIntroEffect)
	{
		return;
	}
	m_spawnIntroEffect = true;
	KdEffekseerManager::GetInstance().Play("EnemySpawner/smoke.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 1.2f, 0.7);
	KdEffekseerManager::GetInstance().Play("EnemySpawner/Lightning.efkefc", GetPos() + Math::Vector3(0.0f, -1.0f, 0.0f), 0.6f, 1.0f, false, 20, 90);

	KdAudioManager::Instance().PlaySE("Asset/Data/Sound/SE/Gimmick/EnemySpawner/Spark.wav");

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

	// 生成元をスポナー(自分)にして、分裂などで生まれた敵にも引き継げるようにする
	enemy->SetSpawner(std::dynamic_pointer_cast<EnemySpawner>(shared_from_this()));

	SceneManager::Instance().AddObject(enemy);

	RegisterSpawnedEnemy(enemy);

	// スポナーの当たり判定の中から出てくるので、外に出るまでは押し戻さない
	m_emergingEnemies.push_back(enemy);

}

void EnemySpawner::RegisterSpawnedEnemy(const std::shared_ptr<EnemyBase>& enemy)
{
	if (!enemy)
	{
		return;
	}

	m_spawnEnemyList.push_back(enemy);
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

void EnemySpawner::RequestSpawn(const EnemySpawnerRequest& request)
{
	if (!CanAcceptRequest())
	{
		return;
	}

	// 何も出していない状態からの要求なら、最初の1体はすぐ出す
	if (m_requestRemaining <= 0)
	{
		// 回転が止まるころに、最初の1体が出るようにタイマーを戻しておく
		m_requestTimer = request.interval - kSpinDuration;
		StartSpin();

		// この要求の最初の1体で、演出を出し直す
		m_spawnIntroEffect = false;
	}

	m_requestRemaining += request.count;

	m_requestInterval = request.interval;
}

bool EnemySpawner::CanAcceptRequest()
{
	auto spRoom = GetLinkedRoom();

	if (!spRoom)
	{
		return false;
	}
	
	return spRoom->IsBossCombat() && static_cast<int>(m_spawnEnemyList.size()) < m_maxAlive;
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

void EnemySpawner::UpdateCameraFade()
{

	float targetRate = 0.0f;

	auto spCamera = m_wpCamera.lock();
	if (spCamera)
	{
		// プレイヤーの体の中心とカメラとの距離
		const Math::Vector3 center = GetPos() + Math::Vector3(0.0f, kFadePivotHeight, 0.0f);
		const float dist = (spCamera->GetPos() - center).Length();

		// 遠い(開始距離以上)なら0、近い(終了距離以下)なら1になる割合
		const float rate = (dist - kFadeEndDist) / (kFadeStartDist - kFadeEndDist);
		targetRate = 1.0f - std::clamp(rate, 0.0f, 1.0f);
	}

	// 急に変わらないよう、少しずつ目標へ近づける
	m_fadeRate += (targetRate - m_fadeRate) * kFadeSpeed;

	if (m_fadeRate > kFadeMaxRate)
	{
		m_fadeRate = kFadeMaxRate;
	}

	// ほぼ0なら0にして、シェーダー側の処理を省く
	if (m_fadeRate < 0.001f)
	{
		m_fadeRate = 0.0f;
	}
}


