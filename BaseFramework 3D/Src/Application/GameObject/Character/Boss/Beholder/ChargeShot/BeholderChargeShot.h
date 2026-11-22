#pragma once

#include"../ChargeSparkBullet/BeholderChargeSparkBulletDesc.h"
#include"../Parameter/BeholderParameter.h"


class CharacterBase;
class BeholderChargeSparkBullet;

class BeholderChargeShot
{
public:

	using BulletParam = BeholderParameter::ChargeSparkBullet;
	using Desc = BeholderChargeSparkBulletDesc;

	void CreateChargeSparkBullet(const std::shared_ptr<CharacterBase>& owner,const BulletParam& bParam);

	// 生成された弾が待機状態になったのか
	bool IsStandbySparkBullet();

	// 弾を発射
	void StartFiring();

	// 生成済みのものを取り消す(ボスが倒れた時など)
	void Cancel();

private:

	// パラメータをセットする
	void SetParameter(const BulletParam& bParam, Desc& desc);

	// 生成した弾を1つだけ持つ
	std::weak_ptr<BeholderChargeSparkBullet>m_wpChargeBullet;


};