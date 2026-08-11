#include "EnemyBase.h"

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
