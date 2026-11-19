#include "BeholderSparkAttack.h"

#include"../SparkBullet/BeholderSparkBulletManager.h"
#include"../SparkBullet/BeholderSparkBullet.h"

#include"../../../CharacterBase.h"

void BeholderSparkAttack::CreateSparkBullet(const std::shared_ptr<CharacterBase>& owner,const SparkParam& sParam)
{
	Desc desc;

	// 弾のパラメータをセット
	SetParameter(sParam, desc);

	// どの番号にするのか
	SetNullPointNodeNumber();

	for (int nullPointNum : m_nullPointNodeNumbers)
	{
		// どのnullPointから生成するのか
		desc.nullPointNumber = nullPointNum;

		// 数字を文字に変換
		std::string num = std::to_string(nullPointNum);

		const KdModelWork::Node* pNode = owner->GetModel()->FindNode("AttackPoint" + num);

		// ノードから行列を作る
		Math::Matrix worldMat = pNode->m_worldTransform * owner->GetMatrix();

		desc.pos = worldMat.Translation();

		auto spark = BeholderSparkBulletManager::Instance().CreateBolt(owner, desc);

		if (spark)
		{
			m_sparkBullets.push_back(spark);
		}
	}


	// 生成が終わったら配列をクリア
	m_nullPointNodeNumbers.clear();
}

bool BeholderSparkAttack::IsStandbySparkBullet()
{

	for (auto weakSpark : m_sparkBullets)
	{
		auto spSpark = weakSpark.lock();
		if (!spSpark) { continue; }

		if (!spSpark->IsStandby())
		{
			return false;
		}
	}

	return true;
}

void BeholderSparkAttack::StartFiring()
{
	for (auto weakSpark : m_sparkBullets)
	{
		auto spSpark = weakSpark.lock();
		if (!spSpark) { continue; }

		// 発射状態にする
		spSpark->StartFiring();
	}

	// 発射まで終わったら要素を空にする
	m_sparkBullets.clear();
}

void BeholderSparkAttack::SetParameter(const SparkParam& sParam, Desc& desc)
{
	// pos、curveAngle、nullPointNumberは後で決める

	// パラメータをセット
	desc.curveRate            = sParam.m_curveRate;
	desc.damage               = sParam.m_damage;
	desc.knockBackPower       = sParam.m_knockBack;
	desc.speed                = sParam.m_speed;
	desc.trackingstopDistance = sParam.m_trackingStopDistance;
	desc.vanishT              = sParam.m_vanishT;
	desc.radius               = sParam.m_radius;
	desc.reflectSpeed         = sParam.m_reflectSpeed;

	desc.effect.size          = sParam.m_effect.m_size;
	desc.effect.speed         = sParam.m_effect.m_speed;
	desc.effect.startFrame    = static_cast<float>(sParam.m_effect.m_startFrame);
	desc.effect.endFrame      = static_cast<float>(sParam.m_effect.m_endFrame);
}

void BeholderSparkAttack::SetNullPointNodeNumber()
{
	int spawnNum = KdRandom::GetInt(1, 5);

	std::vector<int>candidates = { 0,1,2,3,4,5 };

	// 0～5の数字をシャッフルする
	for (int i = 0; i < candidates.size(); i++)
	{
		int target = KdRandom::GetInt(i, static_cast<int>(candidates.size() - 1));

		std::swap(candidates[i], candidates[target]);
	}

	for (int i = 0; i < spawnNum; i++)
	{
		m_nullPointNodeNumbers.push_back(candidates[i]);
	}
}