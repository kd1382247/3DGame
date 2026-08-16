#include "WayPointManager.h"

#include"../../GameObject/WayPoint/WayPoint.h"

bool WayPointManager::RegisterWayPoint(const std::shared_ptr<WayPoint>& wayPoint)
{
	// 無効なWayPointは登録しない
	if (!wayPoint)
	{
		return false;
	}

	// IDはWayPointを識別するため、一意でなければならない
	if (FindWayPoint(wayPoint->GetID()))
	{
		return  false;
	}

	m_spWayPoints.push_back(wayPoint);


	return true;
}

bool WayPointManager::RemoveWayPoint(int id)
{
	auto target = FindWayPoint(id);

	if (!target)
	{
		return false;
	}

	// 削除後に存在しないIDへのリンクが残らないよう、
	// 管理リストから消す前に全WayPointの参照を解除する
	for (const auto& wayPoint : m_spWayPoints)
	{
		if (!wayPoint)
		{
			continue;
		}

		wayPoint->RemoveLink(id);
	}

	auto it = std::find(m_spWayPoints.begin(), m_spWayPoints.end(), target);

	if (it == m_spWayPoints.end())
	{
		return false;
	}

	m_spWayPoints.erase(it);

	return true;
}

void WayPointManager::ClearWayPoints()
{
	// 一時的にバックアップリストに移す
	m_spBackupWayPoints = std::move(m_spWayPoints);
	m_spWayPoints.clear();
}

void WayPointManager::RestoreWayPoints()
{
	// バックアップリストから復元
	m_spWayPoints = std::move(m_spBackupWayPoints);
	m_spBackupWayPoints.clear();
}

void WayPointManager::ClearBackup()
{
	m_spBackupWayPoints.clear();
}

std::shared_ptr<WayPoint> WayPointManager::FindWayPoint(int id) const
{
	// 指定されたIDを検索、見つかればそのWayPointを返す
	for (const auto& point : m_spWayPoints)
	{
		if (!point)
		{
			continue;
		}

		if (point->GetID() == id)
		{
			return point;
		}
	}

	return nullptr;
}

std::shared_ptr<WayPoint> WayPointManager::FindNearest(const Math::Vector3& pos) const
{
	// 調べた中で一番近いWayPointを保存
	std::shared_ptr<WayPoint>nearest = nullptr;

	// 今までで一番短かった距離を保存
	float nearestDistanceSq = FLT_MAX;


	// 1個ずつWayPointを確認する
	for (const auto& point : m_spWayPoints)
	{
		if (!point)
		{
			continue;
		}
		// wayPointの位置  -  調べたい位置
		Math::Vector3 difference = point->GetPos() - pos;

		float distanceSq = difference.LengthSquared();

		// 調べたい位置と近ければ記録を更新
		if (distanceSq < nearestDistanceSq)
		{
			nearestDistanceSq = distanceSq;
			nearest = point;
		}
	}

	return nearest;
}

bool WayPointManager::Connect(int idA, int idB, bool bidirectional)
{
	auto pointA = FindWayPoint(idA);
	auto pointB = FindWayPoint(idB);

	if (!pointA || !pointB)
	{
		return false;
	}

	if (idA == idB)
	{
		return false;
	}

	const bool addedA = pointA->AddLink(idB);
	bool addedB = false;

	if (bidirectional)
	{
		addedB = pointB->AddLink(idA);
	}

	return addedA || addedB;

}

bool WayPointManager::Disconnect(int idA, int idB, bool bidirectional)
{
	auto pointA = FindWayPoint(idA);
	auto pointB = FindWayPoint(idB);

	if (!pointA || !pointB)
	{
		return false;
	}

	if (idA == idB)
	{
		return false;
	}

	const bool hadLinkA = std::find(
		pointA->GetLinks().begin(), pointA->GetLinks().end(), idB)
		!= pointA->GetLinks().end();

	pointA->RemoveLink(idB);

	bool hadLinkB = false;
	if (bidirectional)
	{
		hadLinkB = std::find(
			pointB->GetLinks().begin(), pointB->GetLinks().end(), idA)
			!= pointB->GetLinks().end();

		pointB->RemoveLink(idA);
	}

	return hadLinkA || hadLinkB;
}

std::vector<int> WayPointManager::FindPath(int startId, int goalId) const
{
	// スタート地点とゴール地点のWayPointを探す
	auto startPoint = FindWayPoint(startId);
	auto goalPoint = FindWayPoint(goalId);

	// どちらかのWayPointがなければreturn
	if (!startPoint || !goalPoint)
	{
		return{};
	}
	// スタートIDとゴールIDが同じであればスタートIDを返す
	if (startId == goalId)
	{
		return { startId };
	}

	SearchNode startNode;
	// スタート地点のWayPointを入れる
	startNode.wayPointId = startId;
	startNode.g = 0.0f;
	startNode.h = CalculateHeuristic(*startPoint, *goalPoint);

	startNode.f = startNode.g + startNode.h;
	startNode.parentId = -1;

	// これから調べる候補
	std::vector<int>openList;

	// 調べ終わった場所を保存
	std::unordered_set<int>closeList;

	// 各WayPointの探索情報
	std::unordered_map<int, SearchNode>searchNodes;

	searchNodes[startId] = startNode;
	openList.push_back(startId);

	while (!openList.empty())
	{
		// Fが最小のIDのイテレータを返す処理
		auto bestIt = std::min_element(openList.begin(), openList.end(),
			[&](int leftId, int rightId)
			{  // Fが小さいWayPointを比較する
				return searchNodes.at(leftId).f < searchNodes.at(rightId).f;
			});

		// これから調べるWayPointのID
		int currentId = *bestIt;

		// 調べる候補から削除
		openList.erase(bestIt);
		// 調べ終わったリストに追加
		closeList.insert(currentId);

		// ゴールのIDと同じか
		if (currentId == goalId)
		{
			// 
			return ReconstructPath(searchNodes, goalId);
		}

		auto currentPoint = FindWayPoint(currentId);

		if (!currentPoint)
		{
			continue;
		}

		// 現在のWayPointから、その接続先のWayPointを調べる
		for (int linkId : currentPoint->GetLinks())
		{
			// 調査済みリストに入っていたらスキップ
			if (closeList.contains(linkId))
			{
				continue;
			}

			auto linkedPoint = FindWayPoint(linkId);

			if (!linkedPoint)
			{
				continue;
			}

			// 接続先との距離を計算
			Math::Vector3 difference = linkedPoint->GetPos() - currentPoint->GetPos();
			// 一区間の距離を入れる
			float moveCost = difference.Length();

			// スタート地点から、接続先WayPointまで進んだ合計距離
			float newG = searchNodes.at(currentId).g + moveCost;

			// 接続先WayPointを、今回の探索で初めて発見したか？
			bool isNewNode = !searchNodes.contains(linkId);

			// 発見済みの場合
			// 今回見つけた道の方が、以前の道より短いか?
			bool isShorterPath = !isNewNode && newG < searchNodes.at(linkId).g;


			// 初めて発見してない、前より短い経路でもない場合スキップ
			if (!isNewNode && !isShorterPath)
			{
				continue;
			}

			// 接続先のWayPoint情報
			SearchNode linkedSearchNode;

			// 今調べている接続先IDを入れる
			linkedSearchNode.wayPointId = linkId;

			// スタート地点から接続先までの距離
			linkedSearchNode.g = newG;

			// 接続先からゴールまでの予想距離
			linkedSearchNode.h = CalculateHeuristic(*linkedPoint, *goalPoint);

			linkedSearchNode.f = linkedSearchNode.g + linkedSearchNode.h;

			// 接続先の親ID = 現在のWayPointIDを入れる
			linkedSearchNode.parentId = currentId;

			// 接続先Nodeを新たに追加
			searchNodes[linkId] = linkedSearchNode;

			// このWayPointがまだオープンリストに入っていなければ追加
			if (std::find(openList.begin(), openList.end(), linkId) == openList.end())
			{
				openList.push_back(linkId);
			}
		}
	}

	return{};


}

std::shared_ptr<WayPoint> WayPointManager::CreateWayPoint()
{
	const int id = FindAvailableID();

	std::shared_ptr<WayPoint>wayPoint = std::make_shared<WayPoint>();


	wayPoint->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	std::string objName = "WayPoint_" + std::to_string(id);
	wayPoint->SetObjectName(objName);

	if (!RegisterWayPoint(wayPoint))
	{
		return nullptr;
	}

	return wayPoint;

}

int WayPointManager::FindAvailableID() const
{
	int id = 0;

	while (FindWayPoint(id))
	{
		++id;
	}

	return id;
}

void WayPointManager::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void WayPointManager::DrawDebug()
{

	for (const auto& point : m_spWayPoints)
	{
		if (!point)
		{
			continue;
		}

		// WayPointの位置を表す球を表示
		point->DrawDebug();

		// 接続関係を表す線を描画
		for (int linkId : point->GetLinks())
		{
			auto linkedPoint = FindWayPoint(linkId);

			if (!linkedPoint)
			{
				continue;
			}
			// 小さいID側だけ登録
			if (point->GetID() > linkId)
			{
				continue;
			}

			m_pDebugWire->AddDebugLine(point->GetPos(), linkedPoint->GetPos(), kGreenColor);
		}
	}

	m_pDebugWire->Draw();
}

bool WayPointManager::Save(const std::string& filePath)
{

	nlohmann::json rootJson;
	rootJson["WayPoints"] = nlohmann::json::array();

	for (const auto& wayPoint : m_spWayPoints)
	{
		if (!wayPoint)
		{
			continue;
		}

		nlohmann::json wayPointJson;

		wayPointJson["ID"] = wayPoint->GetID();

		wayPointJson["Name"] = wayPoint->GetObjectName();

		const auto& pos = wayPoint->GetPos();

		wayPointJson["Position"]["x"] = pos.x;
		wayPointJson["Position"]["y"] = pos.y;
		wayPointJson["Position"]["z"] = pos.z;

		wayPointJson["Links"] = wayPoint->GetLinks();

		rootJson["WayPoints"].push_back(wayPointJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("WayPointDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool WayPointManager::Load(const std::string& filePath)
{

	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA(
			"WayPointDataを開けませんでした\n"
		);

		return false;
	}

	nlohmann::json rootJson;

	try
	{
		file >> rootJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA(
			"WayPointDataの読み込みに失敗しました\n"
		);

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");

		return false;
	}

	if (!rootJson.contains("WayPoints") ||
		!rootJson["WayPoints"].is_array())
	{
		OutputDebugStringA(
			"WayPoints配列がありません\n"
		);

		return false;
	}


	// 既存のWayPointを消す
	ClearWayPoints();

	// Jsonに保存されてる情報でWayPointを生成
	for (const auto& wpJson : rootJson["WayPoints"])
	{

		auto obj = KdGameObjectFactory::Instance().CreateGameObject("WayPoint");

		auto wayPoint = std::dynamic_pointer_cast<WayPoint>(obj);

		if (!wayPoint)
		{
			continue;
		}

		wayPoint->Init();
		// ID
		wayPoint->SetID(wpJson["ID"].get<int>());
		//Name
		wayPoint->SetObjectName(wpJson["Name"].get<std::string>());
		//Position
		wayPoint->SetPos({
			wpJson["Position"]["x"].get<float>(),
			wpJson["Position"]["y"].get<float>(),
			wpJson["Position"]["z"].get<float>()
			});

		// WayPointsに登録
		if (!RegisterWayPoint(wayPoint))
		{
			ClearWayPoints();
			return false;
		}

	}

	// 接続設定
	for (auto& wpJson : rootJson["WayPoints"])
	{
		int id = wpJson["ID"].get<int>();

		// IDからWayPointを探す
		auto wayPoint = FindWayPoint(id);

		if (!wayPoint)
		{
			continue;
		}

		for (const auto& linkJson : wpJson["Links"])
		{
			int linkID = linkJson.get<int>();
			wayPoint->AddLink(linkID);
		}
	}

	return true;
}

float WayPointManager::CalculateHeuristic(const WayPoint& current, const WayPoint& goal) const
{

	Math::Vector3 difference = goal.GetPos() - current.GetPos();

	return difference.Length();
}

std::vector<int> WayPointManager::ReconstructPath(const std::unordered_map<int, SearchNode>& searchNodes, int goalId) const
{
	// スタートからゴールまでのWayPointのID一覧
	std::vector<int>path;

	// ゴールから始める
	int currentId = goalId;

	// 親が無くなるまでたどる
	while (currentId != -1)
	{
		path.push_back(currentId);

		// 現在のIDのscerchNodesを探す
		auto nodeIt = searchNodes.find(currentId);


		if (nodeIt == searchNodes.end())
		{
			return{};
		}

		// 親情報を更新
		currentId = nodeIt->second.parentId;

	}

	// ゴールから親を逆にたどり、それを反転して
	// 敵が実際に歩く順番へ直す
	std::reverse(path.begin(), path.end());

	return path;
}

