#pragma once

#include"../SparkBullet/BeholderSparkBulletDesc.h"
#include"../Parameter/BeholderParameter.h"

class CharacterBase;

class BeholderSparkBullet;

class BeholderSparkAttack
{
public:


	using SparkParam    = BeholderParameter::SparkBullet;
	using Desc          = BeholderSparkBulletDesc;


	void CreateSparkBullet(const std::shared_ptr<CharacterBase>& owner, const SparkParam& sParam);

	// 生成された弾が待機状態になったのか
	bool IsStandbySparkBullet();

	// 弾を発射
	void StartFiring();

	// 生成した弾を消す(ボスが倒れた時のため)
	void CancelAll();

private:
 
	// パラメータをセットする
	void SetParameter(const SparkParam& sParam, Desc& desc);

	// NullPointNodeの番号をセット
	void SetNullPointNodeNumber();


	std::vector<std::weak_ptr<BeholderSparkBullet>>m_sparkBullets;

	// 生成するnullPointNodeの番号の配列
	std::vector<int>m_nullPointNodeNumbers;

};