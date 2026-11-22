#include "BeholderChargeShot.h"

#include"../ChargeSparkBullet/BeholderChargeSparkBullet.h"

#include"../../../../../Scene/SceneManager.h"
#include"../../../CharacterBase.h"

void BeholderChargeShot::CreateChargeSparkBullet(const std::shared_ptr<CharacterBase>& owner, const BulletParam& bParam)
{
	// 参照を解除
	m_wpChargeBullet.reset();

	Desc desc;
	// パラメータをセット
	SetParameter(bParam, desc);

	auto chargeBullet = std::make_shared<BeholderChargeSparkBullet>();

	chargeBullet->Init();
	chargeBullet->Setup(desc);
	chargeBullet->SetOwner(owner);

	SceneManager::Instance().AddObject(chargeBullet);
	
	m_wpChargeBullet = chargeBullet;
	
}

bool BeholderChargeShot::IsStandbySparkBullet()
{
	if (auto spChargeBullet = m_wpChargeBullet.lock())
	{
	 	return spChargeBullet->IsStandby();
	}

	return false;
}

void BeholderChargeShot::StartFiring()
{
	if (auto spChargeBullet = m_wpChargeBullet.lock())
	{
		return spChargeBullet->StartFiring();
	}
}

void BeholderChargeShot::Cancel()
{
	if (auto spBullet = m_wpChargeBullet.lock())
	{
		spBullet->Cancel();
	}
}

void BeholderChargeShot::SetParameter(const BulletParam& bParam, Desc& desc)
{

	// パラメータをセット
	desc.damage            = bParam.m_damage;
	desc.knockBackPower    = bParam.m_knockBack;
	desc.speed             = bParam.m_speed;
	desc.radius            = bParam.m_radius;
	desc.reflectSpeed      = bParam.m_reflectSpeed;
	desc.lifeTime          = bParam.m_lifeTime;
	desc.chargeTime        = bParam.m_chargeTime;
	desc.targetSize        = bParam.m_targetSize;


	desc.effect.size       = bParam.m_effect.m_size;
	desc.effect.speed      = bParam.m_effect.m_speed;
	desc.effect.startFrame = static_cast<float>(bParam.m_effect.m_startFrame);
	desc.effect.endFrame   = static_cast<float>(bParam.m_effect.m_endFrame);



}
