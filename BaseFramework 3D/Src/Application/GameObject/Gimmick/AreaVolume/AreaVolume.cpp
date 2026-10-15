#include "AreaVolume.h"

#include"../../../System/CollisionManager/CollisionManager.h"

void AreaVolume::Init()
{

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("AreaVolume",GetBox(),KdCollider::TypeBump);

	// エリアIDが未割り当てなら、使われていない最小のIDを自動で割り振る
	// (自分自身はまだ登録前なので、数えられない)
	if (m_areaID < 0)
	{
		m_areaID = FindAvailableAreaID();
	}

	CollisionManager::Instance().RegisterObject(CollisionLayer::Area, shared_from_this());

	SetObjectName("AreaVolume");
	SetObjectCategory(KdGameObject::ObjectCategory::AreaVolume);

}

void AreaVolume::Update()
{


}

void AreaVolume::DrawDebug()
{
	// Active StageのAreaVolumeだけ、AreaVolumeのデバッグ表示がONのとき(または選択中)に箱を出す
	if (!ShouldDrawDebug())
	{
		KdGameObject::DrawDebug();
		return;
	}

	Math::Color color;

	// 選択中は黄色
	if (IsSelected())
	{
		color = kRedColor + kGreenColor;
	}
	else
	{
		// 通常はエリアIDごとの色
		color = GetAreaColor(GetAreaID());
	}

	m_pDebugWire->AddDebugBox(m_mWorld, { 0.5f,0.5f,0.5f }, Math::Vector3::Zero, true, color);

	KdGameObject::DrawDebug();
}

void AreaVolume::DrawInspectorHeader()
{
	KdGameObject::DrawBasicInspecter();

	// 追従するステージ
	m_stageFollower.DrawInspector();
}

void AreaVolume::DrawInspector()
{
	// AreaIDは自動で割り振るため、編集欄は無い
}

DirectX::BoundingBox AreaVolume::GetBox() const
{


	DirectX::BoundingBox box;

	box.Center = GetPos();

	box.Extents = GetScale() * 0.5;

	return box;
}

nlohmann::json AreaVolume::SaveData() const
{
	nlohmann::json areaJson = KdGameObject::SaveData();

	m_stageFollower.SaveData(areaJson);


	return areaJson;

}

void AreaVolume::LoadData(const nlohmann::json& json)
{
	m_stageFollower.LoadData(json);
}

int AreaVolume::FindAvailableAreaID()
{
	// すでに存在するAreaVolumeが使っているIDを集める
	std::unordered_set<int> usedIDs;

	for (const auto& obj : CollisionManager::Instance().GetObjects(CollisionLayer::Area))
	{
		auto areaVolume = std::dynamic_pointer_cast<AreaVolume>(obj.lock());

		if (!areaVolume)
		{
			continue;
		}

		usedIDs.insert(areaVolume->GetAreaID());
	}

	// 使われていない最小のIDを探す
	int id = 0;

	while (usedIDs.contains(id))
	{
		++id;
	}

	return id;
}

Math::Color AreaVolume::GetAreaColor(const int areaID)
{
	// 未確定(-1など)は灰色
	if (areaID < 0)
	{
		return Math::Color(0.5f, 0.5f, 0.5f, 1.0f);
	}

	// エリアIDごとの色。IDが色の数を超えたら先頭に戻って繰り返す
	// (選択中の黄色と紛らわしくならないよう、黄色は入れていない)
	static const Math::Color palette[] =
	{
		Math::Color(1.0f, 0.2f, 0.2f, 1.0f), // 0 赤
		Math::Color(0.2f, 1.0f, 0.2f, 1.0f), // 1 緑
		Math::Color(0.3f, 0.5f, 1.0f, 1.0f), // 2 青
		Math::Color(1.0f, 0.6f, 0.1f, 1.0f), // 3 オレンジ
		Math::Color(0.1f, 0.9f, 0.9f, 1.0f), // 4 水色
		Math::Color(1.0f, 0.2f, 1.0f, 1.0f), // 5 マゼンタ
		Math::Color(0.6f, 0.3f, 1.0f, 1.0f), // 6 紫
		Math::Color(1.0f, 0.55f, 0.7f, 1.0f),// 7 ピンク
	};

	const int paletteCount = static_cast<int>(std::size(palette));

	return palette[areaID % paletteCount];
}

bool AreaVolume::IsInside(const Math::Vector3& pos) const
{
	// Boxの中にいたらtrue
	return GetBox().Contains(DirectX::XMLoadFloat3(&pos)) == DirectX::CONTAINS;
}

float AreaVolume::CalcCoverage(const DirectX::BoundingSphere& sphere) const
{
	
	DirectX::BoundingBox box = GetBox();
	
	Math::Vector3 boxCenter  = box.Center;
	Math::Vector3 boxExtents = box.Extents;

	// ボックスの中心とスフィアの点の距離
	Math::Vector3 offset = sphere.Center - boxCenter;

	// 壁までの距離の絶対値
	float distX = boxExtents.x - std::abs(offset.x);
	float distY = boxExtents.y - std::abs(offset.y);
	float distZ = boxExtents.z - std::abs(offset.z);

	float depth = std::min({ distX,distY,distZ });

	float sphereRadius = std::max(sphere.Radius, 0.01f);

	float coverage = std::clamp((depth + sphereRadius) / (2 * sphereRadius), 0.0f,1.0f);

	return coverage;

}
