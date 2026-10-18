#include "ChestMonster.h"

#include"../../../../Scene/SceneManager.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../../../HPBar/EnemyHPBar/EnemyHPBarManager.h"


#include"State/States/ChestMonsterNormalState.h"
#include"State/States/ChestMonsterDieState.h"
#include"State/States/ChestMonsterDamageState.h"

void ChestMonster::Init()
{
	if (!m_spModel)
	{
		InitCharacterModel("Asset/Models/Enemy/ChestMonster/ChestMonster.gltf", "ChestMonster",
			Math::Vector3(0.0f, 0.8f, 0.0f), 0.8f, "ChestMonster");

		// アニメーションクラス初期化
		m_animation.Init(m_spModel);
		// パラメータクラス初期化
		m_parameter.Init();

		// Parameterの値を反映(HP・到達距離・召喚間隔)
		ApplyParameter();

		// ノーマルステートで初期化
		m_stateMachine.Start(this);
		m_stateMachine.ChangeState<ChestMonsterNormalState>();

		m_smokeEffectScale = 0.9f;

		m_effectDist = 1.4;
	}

	EnemyBase::Init();

	CollisionManager::Instance().RegisterObject(CollisionLayer::CharacterBump, shared_from_this());


	SetPos({ 0.0f,0.0f,0.0f });
	SetScale(m_parameter.GetParam().m_scale);
	m_bumpPushRate = 0.0f;
}

void ChestMonster::Update()
{
	UpdateGravity();

	m_stateMachine.Update();

	// 紐づけた部屋が戦闘中の間だけ、召喚までの時間を数える
	if (!IsSpawnEnemy() && IsInActiveRoom())
	{
		// 秒で数える
		m_spawnCountDown -= m_deltaTime;

		// 時間が来ても、敵の数が上限なら召喚せず、空くまで待つ
		if (m_spawnCountDown <= 0 && IsUnderEnemyLimit())
		{
			SetIsSpawnEnemy(true);
		}
	}

}

void ChestMonster::PostUpdate()
{
	EnemyBase::PostUpdate();

	UpdateAnimation();

	DrawBumpDebugSphere(Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f);

}

void ChestMonster::DrawInspectorHeader()
{
	EnemyBase::DrawInspectorHeader();
	
	// 対応する部屋(None のときは、部屋に関係なく召喚する)
	m_roomLink.DrawCombo();

}

void ChestMonster::SetUpReference()
{
	EnemyBase::SetUpReference();

	// HPBarを生成
	EnemyHPBarManager::Instance().
		CreateHPBar(shared_from_this(), Math::Vector3(-0.7f, 3.0f, 0.0f));
}

void ChestMonster::DrawParameterInspector()
{
	m_parameter.DrawInspecter();
}

void ChestMonster::UpdateSpawnEnemy()
{


	if (m_animFrameCount <= m_spawnTiming.spawnStart || m_animFrameCount >= m_spawnTiming.spawnEnd)
	{
		return;
	}

	m_spawnWait -= 60 * m_deltaTime;

	if (m_spawnWait <= 0)
	{
		m_spawnFlg = true;
		m_spawnWait = 0;
	}

	if (!m_spawnFlg)
	{
		return;
	}


	// Inspectorで選んだ敵の中から、ランダムに1体選んで生成する
	const auto& enemyNameSet = m_parameter.GetParam().m_spawnEnemyNames;

	// 敵の数が上限の時は、召喚の途中でも出さない
	if (!enemyNameSet.empty() && IsUnderEnemyLimit())
	{
		// std::setはインデックスで選べないので、std::vectorへコピーする
		const std::vector<std::string> enemyNames(enemyNameSet.begin(), enemyNameSet.end());

		const int nameIndex = KdRandom::GetInt(0, static_cast<int>(enemyNames.size()) - 1);

		CreateEnemy(enemyNames[nameIndex]);
	}

	// 次の1体までの待ちフレーム
	m_spawnWait = m_parameter.GetParam().m_spawnWaitFrame;

	m_spawnFlg = false;

}

void ChestMonster::StartSpawnEnemy()
{
	m_spawnFlg = true;

	SetSpawnTiming();
}

void ChestMonster::EndSpawnEnemy()
{
	m_spawnCountDown = m_spawnInterval;
	SetIsSpawnEnemy(false);

}

void ChestMonster::AnimFrame()
{
	m_animFrameCount += m_deltaTime * 60.0f;
}

void ChestMonster::PlayAnimation(ChestMonsterAnimationType type)
{
	m_animation.Play(type);
}

void ChestMonster::RePlayAnimation(ChestMonsterAnimationType type)
{
	m_animation.RePlay(type);
}

void ChestMonster::OnHit(const AttackInfo attackInfo)
{
	// ノックバックは受けないようにする
	AttackInfo info = attackInfo;

	info.m_knockBackDir = Math::Vector3::Zero;

	if (ApplyDamage(info))
	{
		m_stateMachine.ChangeState<ChestMonsterDieState>();
	}
	else if (!IsSpawnEnemy())
	{
		m_stateMachine.ChangeState<ChestMonsterDamageState>();
		RePlayAnimation(ChestMonsterAnimationType::GetHit);
	}
}



void ChestMonster::CreateEnemy(const std::string& enemyName)
{
	// 未登録の名前をファクトリーに渡すとassertで止まるので、先に調べる
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

	const auto& param = m_parameter.GetParam();

	// 敵を出す位置(自分の位置から少し上)
	const Math::Vector3 spawnPos = GetPos() + Math::Vector3(0.0f, param.m_spawnHeight, 0.0f);

	enemy->Init();
	enemy->SetPos(spawnPos);
	enemy->SetPrevPos(spawnPos);
	enemy->SetRotation(GetRotation());


	// 飛び上がる勢い
	const float power = param.m_spawnLaunchPower;

	Math::Vector3 launchDir = m_mWorld.Backward();

	// 正面へ飛び出す速さ
	launchDir *= param.m_spawnLaunchSpeed;
	// 飛び出す方向をセット
	enemy->Launch(launchDir, power);

	// 関連付け
	enemy->SetUpReference();

	SceneManager::Instance().AddObject(enemy);

	// 自分が出した敵として覚えておく
	m_spawnedEnemies.push_back(enemy);
}

void ChestMonster::UpdateAnimation()
{

	m_animation.Update(m_deltaTime);

}

nlohmann::json ChestMonster::SaveData() const
{
	// 位置・回転・大きさなどの共通データ
	nlohmann::json json = EnemyBase::SaveData();

	// この1体だけの個別パラメータ
	json["Parameter"] = m_parameter.ToJson();

	// 対応する部屋
	m_roomLink.SaveData(json);

	return json;
}

void ChestMonster::LoadData(const nlohmann::json& json)
{
	EnemyBase::LoadData(json);

	m_roomLink.LoadData(json);

	// キーが無い古いステージは、共通JSONのデフォルト値のまま動く
	if (json.contains("Parameter"))
	{
		m_parameter.FromJson(json["Parameter"]);

		// Initで反映済みの値を、個別の値で上書きし直す
		ApplyParameter();
	}
}

bool ChestMonster::IsInActiveRoom()
{
	// 部屋に紐づけていないときは、今まで通り常に有効
	if (!m_roomLink.HasLink())
	{
		return true;
	}

	// 紐づけた部屋が見つからない・戦闘中ではない時は無効
	auto spRoom = m_roomLink.GetRoom();

	return spRoom && spRoom->IsCombat();
}

bool ChestMonster::IsUnderEnemyLimit()
{
	const auto& param = m_parameter.GetParam();

	// 自分が出した敵が、上限まで生きているか
	EraseSpawnedEnemy();

	if (static_cast<int>(m_spawnedEnemies.size()) >= param.m_maxSpawnedAlive)
	{
		return false;
	}

	// 全体の敵が、上限まで生きているか
	return CountTotalEnemies() < param.m_maxTotalEnemies;
}

void ChestMonster::EraseSpawnedEnemy()
{
	std::erase_if(m_spawnedEnemies,
		[](const std::weak_ptr<EnemyBase>& enemy)
		{
			auto spEnemy = enemy.lock();

			return !spEnemy || spEnemy->IsExpired();
		}
	);
}

int ChestMonster::CountTotalEnemies() const
{
	int count = 0;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto enemy = std::dynamic_pointer_cast<EnemyBase>(obj);

		if (!enemy || enemy->IsExpired())
		{
			continue;
		}

		// ChestMonster本体は、召喚した敵ではないので数えない
		if (std::dynamic_pointer_cast<ChestMonster>(enemy))
		{
			continue;
		}

		++count;
	}

	return count;
}

void ChestMonster::ApplyParameter()
{
	const auto& param = m_parameter.GetParam();

	m_health.Init(param.m_maxHP);

	m_reachDistance = param.m_reachDistance;

	// 敵を呼び出す間隔(秒)
	m_spawnInterval = param.m_spawnInterval;
	m_spawnCountDown = m_spawnInterval;
}

void ChestMonster::SetSpawnTiming()
{
	m_spawnTiming.spawnStart = m_parameter.GetParam().m_spawnStartFrame;
	m_spawnTiming.spawnEnd = m_parameter.GetParam().m_spawnEndFrame;

	m_animFrameCount = 0.0f;
}
