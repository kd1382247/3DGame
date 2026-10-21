#include "CheckPoint.h"

#include"CheckPointManager.h"

#include"../../../Editor/InspectorWidgets.h"
#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../../Character/Player/Player.h"
#include"../Room/Room.h"
#include"../../../Scene/SceneManager.h"
#include"../../../Scene/GameScene/GameScene.h"

void CheckPoint::Init()
{
	SetObjectName("CheckPoint");
	SetObjectCategory(KdGameObject::ObjectCategory::CheckPoint);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	// 当たり判定は球(エディタでクリックして選ぶためだけに使う。ゲーム中の判定は距離で行う)
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("CheckPoint", Math::Vector3(0, 0, 0), kRadius, KdCollider::TypeBump);

}

void CheckPoint::Update()
{
	CheckPointManager& manager = CheckPointManager::Instance();

	// 到達済みか (ゲームオーバーのリトライでシーンが作り直されても、記録から分かる)
	m_isReached = manager.HasCheckPoint() && manager.GetReachedOrder() >= m_order;

	if (m_isReached)
	{
		return;
	}

	auto spPlayer = m_wpPlayer.lock();

	// プレイヤーが倒れている間は、記録も回復もしない
	if (!spPlayer || spPlayer->GetHealth().IsDead())
	{
		return;
	}

	// 範囲の球に、プレイヤーの当たり判定の球が触れたか
	// (ステージのクリアや部屋には関係なく、距離だけで決まる)
	DirectX::BoundingSphere rangeSphere;
	rangeSphere.Center = GetPos();
	rangeSphere.Radius = m_radius;

	if (!rangeSphere.Intersects(spPlayer->GetBumpSphere()))
	{
		return;
	}

	// 今の記録より大きい順番のときだけ、記録される
	if (!manager.TryReach(m_order, GetPos()))
	{
		return;
	}

	m_isReached = true;

	// そのときのプレイ時間と、クリア条件を満たした部屋も覚える
	SaveProgress();

	// 記録できたときだけ、HPを全回復する (一度だけ)
	spPlayer->Heal(spPlayer->GetHealth().GetMaxHP());
}

bool CheckPoint::ShouldDrawDebug()
{
	EditorManager& editor = EditorManager::Instance();

	// PlayMode : Debugチェックに従う
	if (editor.IsPlayMode())
	{
		return editor.IsCheckPointDebug();
	}

	// EditMode : DebugチェックがONか、選択中のときに出す
	return editor.IsCheckPointDebug() || IsSelected();
}

void CheckPoint::DrawDebug()
{
	// CheckPointのデバッグ表示がONのとき(または選択中)に球を出す
	if (!ShouldDrawDebug())
	{
		KdGameObject::DrawDebug();
		return;
	}

	// 内側の球は水色、範囲の球は緑(到達済みは青)。選択中はどちらも黄色
	Math::Color coreColor = Math::Color(0.0f, 1.0f, 1.0f, 1.0f);
	Math::Color rangeColor = m_isReached ? Math::Color(0.3f, 0.5f, 1.0f, 1.0f) : kGreenColor;

	if (IsSelected())
	{
		coreColor = kRedColor + kGreenColor;
		rangeColor = kRedColor + kGreenColor;
	}

	// スケールは見た目に影響させず、半径はそのままの値で表示する
	m_pDebugWire->AddDebugSphere(GetPos(), kRadius, coreColor);
	m_pDebugWire->AddDebugSphere(GetPos(), m_radius, rangeColor);

	KdGameObject::DrawDebug();
}

void CheckPoint::DrawInspectorHeader()
{
	KdGameObject::DrawBasicInspecter();
}

void CheckPoint::DrawInspector()
{
	// 順番(小さいほうが先。同じ順番や飛ばした番号があっても動く)
	InspectorUI::Count("Order", m_order, 0);

	// 範囲の半径(内側の球より小さくならないようにする)
	if (InspectorUI::Meter("Radius", m_radius))
	{
		m_radius = (std::max)(m_radius, kRadius);
	}
}

void CheckPoint::SaveProgress() const
{
	// プレイ時間 (ゲームシーンだけが数えている。エディタのPlayでは0)
	float playTime = 0.0f;

	const auto gameScene = SceneManager::Instance().GetCurrentScene<GameScene>();

	if (gameScene)
	{
		playTime = gameScene->GetPlayTime();
	}

	// クリア条件を満たした部屋の名前を集める
	std::vector<std::string> clearedRoomNames;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		const auto room = std::dynamic_pointer_cast<Room>(obj);

		if (room && room->IsClearConditionMet())
		{
			clearedRoomNames.push_back(room->GetObjectName());
		}
	}

	CheckPointManager::Instance().SaveProgress(playTime, clearedRoomNames);
}

void CheckPoint::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

nlohmann::json CheckPoint::SaveData() const
{
	nlohmann::json checkPointJson = KdGameObject::SaveData();

	checkPointJson["Order"] = m_order;
	checkPointJson["Radius"] = m_radius;

	return checkPointJson;
}

void CheckPoint::LoadData(const nlohmann::json& json)
{
	m_order = json.value("Order", 0);
	m_radius = json.value("Radius", kDefaultRadius);
}
