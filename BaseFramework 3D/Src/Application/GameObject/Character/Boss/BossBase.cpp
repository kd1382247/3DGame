#include "BossBase.h"

#include"../../../System/GameObjectFinder/GameObjectFinder.h"

#include"../Player/Player.h"

#include"../../FlyText/FlyTextManager.h"

void BossBase::Init()
{
	// カテゴリーをセット
	SetObjectCategory(ObjectCategory::Character);
	m_bumpPushRate = 0.0f;
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

void BossBase::OnHit(const AttackInfo attackInfo)
{
	m_hp -= attackInfo.damage;

	if (m_hp <= 0)
	{
		m_hp = 0;
		m_outroFlg = true;
	}

	FlyTextManager::Instance().CreateDamateText(attackInfo.damage, GetPos(),m_flyTextPath);

	KdEffekseerManager::GetInstance().
		Play("Hit/Hit2.efkefc", GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f), 0.4f, 1.0f, false);
}

bool BossBase::IsSecondPhase()const
{
	return GetCurrentHP() <= GetMaxHP() / 2;
}

int BossBase::LotteryPattern(const std::vector<float>& weights)const
{
	float total = 0.0f;

	for (const float weight : weights)
	{
		if (weight > 0.0f)
		{
			total += weight;
		}
	}

	if (total <= 0.0f)
	{
		return -1;
	}

	float lot = KdRandom::GetFloat(0.0f, total);

	float sum = 0.0f;

	for (size_t i = 0; i < weights.size(); i++)
	{
		if (weights[i] <= 0.0f)
		{
			continue;
		}

		sum += weights[i];

		if (lot <= sum)
		{
			return static_cast<int>(i);
		}
	}

	return -1;
}
