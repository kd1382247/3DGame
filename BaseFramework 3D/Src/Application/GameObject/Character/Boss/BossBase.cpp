#include "BossBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../Player/Player.h"

void BossBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);
	m_bumpPushRate = 1.0f;
}

void BossBase::PostUpdate()
{
	CharacterBase::PostUpdate();
}

void BossBase::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void BossBase::DrawInspector()
{
	DrawBasicInspecter();

	DrawParameterInspector();
}
