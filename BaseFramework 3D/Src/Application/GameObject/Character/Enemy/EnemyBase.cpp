#include "EnemyBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../Player/Player.h"

void EnemyBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);
}

void EnemyBase::Update()
{

}

void EnemyBase::PostUpdate()
{
	CharacterBase::PostUpdate();
}

void EnemyBase::DrawInspector()
{
	CharacterBase::DrawInspector();
}

void EnemyBase::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

bool EnemyBase::CanDirectChase()
{

	auto player = m_wpPlayer.lock();

	if (!player)
	{
		return false;
	}

	Math::Vector3 startPos = GetPos();
	Math::Vector3 playerPos = player->GetPos();

	// プレイヤーの方向に向くベクトル作成
	Math::Vector3 direction = playerPos - startPos;
	
	// レイの長さをセット
	float rayLength = direction.Length();

	if (rayLength <= 0.0f)
	{
		return true;
	}

	// 正規化
	direction.Normalize();

	
	// レイ情報
	KdCollider::RayInfo rayInfo;

	// 方向
	rayInfo.m_dir = direction;
	// 始点
	rayInfo.m_pos = startPos;
	// 長さ
	rayInfo.m_range = rayLength;

	// 判定対象
	rayInfo.m_type = KdCollider::Type::TypeSight;





	return true;

}


