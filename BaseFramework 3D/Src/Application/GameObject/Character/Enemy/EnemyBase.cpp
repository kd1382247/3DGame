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
