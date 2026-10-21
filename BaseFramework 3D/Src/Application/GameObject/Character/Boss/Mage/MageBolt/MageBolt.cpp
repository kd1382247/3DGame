#include "MageBolt.h"

#include"../../../CharacterBase.h"
#include"../../../Player/Player.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"
#include"../../../../../System/CollisionManager/CollisionManager.h"

void MageBolt::Init()
{
	SetUpReference();
}

void MageBolt::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void MageBolt::Setup(const MageBoltDesc& desc)
{
	m_desc = desc;

	SetPos(m_desc.pos);

	m_dir = m_desc.dir;
	m_speed = m_desc.speed;
	m_lifeTime = m_desc.lifeTime;
}

void MageBolt::Update()
{
	// 撃ったボスがいなくなったら消える
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		Vanish();
		return;
	}

	const float deltaTime = TimeManager::Instance().GetDeltaTime();

	Move(deltaTime);

	UpdateEffect();

	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos();
	sphere.Radius = m_desc.radius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

	// マップ(壁)に当たったら消える
	if (IsHitMap(sphereInfo))
	{
		Vanish();
		return;
	}

	// 一定時間で消える
	m_lifeTime -= deltaTime;

	if (m_lifeTime <= 0.0f)
	{
		Vanish();
		return;
	}

	// 打ち返された後は、プレイヤーとは判定せず、ボスに当たるかを見る
	if (m_isReflect)
	{
		HitOwner(sphereInfo);
		return;
	}

	HitPlayer(sphereInfo);
}

void MageBolt::Move(float deltaTime)
{
	// 速さは1フレームあたりの値なので、60fps換算にして時間を掛ける
	const Math::Vector3 move = m_dir * (m_speed * 60.0f) * deltaTime;

	SetPos(GetPos() + move);
}

bool MageBolt::IsHitMap(const KdCollider::SphereInfo& sphereInfo)const
{
	// 壁(AABB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::AABB))
	{
		auto spObj = wpObj.lock();

		if (!spObj)
		{
			continue;
		}

		if (spObj->Intersects(sphereInfo, nullptr))
		{
			return true;
		}
	}

	// 壁(OBB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::OBB))
	{
		auto spObj = wpObj.lock();

		if (!spObj)
		{
			continue;
		}

		if (spObj->Intersects(sphereInfo, nullptr))
		{
			return true;
		}
	}

	return false;
}

void MageBolt::HitPlayer(const KdCollider::SphereInfo& sphereInfo)
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
	{
		return;
	}

	if (!spPlayer->Intersects(sphereInfo, nullptr))
	{
		return;
	}

	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
	knockBackDir.y = 0;

	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir = knockBackDir;
	attackInfo.m_knockBackPower = m_desc.knockBackPower;
	attackInfo.m_damage = m_desc.damage;
	attackInfo.m_canReflect = true;

	spPlayer->OnHit(attackInfo);

	// パリィされたら打ち返す。そうでなければ、当たって消える
	if (spPlayer->GetParry().GetIsParrySuccess())
	{
		Reflect();
	}
	else
	{
		Vanish();
	}
}

void MageBolt::HitOwner(const KdCollider::SphereInfo& sphereInfo)
{
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		return;
	}

	if (!spOwner->Intersects(sphereInfo, nullptr))
	{
		return;
	}

	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spOwner->GetPos() - GetPos();
	knockBackDir.y = 0;

	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir = knockBackDir;
	attackInfo.m_knockBackPower = m_desc.knockBackPower;
	attackInfo.m_damage = m_desc.damage;
	attackInfo.m_canReflect = true;

	spOwner->OnHit(attackInfo);

	Vanish();
}

void MageBolt::Reflect()
{
	m_isReflect = true;

	// 打ち返したぶん、寿命を延ばす
	m_lifeTime += m_desc.lifeTime;

	m_speed += 0.1f;

	// ボスの胸のあたりへ向ける
	auto spOwner = m_wpOwner.lock();

	if (spOwner)
	{
		m_dir = (spOwner->GetPos() + Math::Vector3(0.0f, 0.5f, 0.0f)) - GetPos();
		m_dir.Normalize();
	}
}

void MageBolt::PlayEffect(const Math::Vector3& pos)
{
	// IsLoop=falseで単発再生し、区間再生が終わるたびにUpdateEffect()側で再生し直す
	m_wpEffect = KdEffekseerManager::GetInstance().Play(
		m_desc.effect.fileName,
		pos,
		m_desc.effect.size,
		m_desc.effect.speed,
		false,
		m_desc.effect.startFrame,
		m_desc.effect.endFrame);
}

void MageBolt::UpdateEffect()
{
	auto spEffect = m_wpEffect.lock();

	if (!spEffect || !spEffect->IsPlaying())
	{
		PlayEffect(GetPos());
		spEffect = m_wpEffect.lock();
	}

	if (spEffect)
	{
		spEffect->SetPos(GetPos());
	}
}

void MageBolt::Vanish()
{
	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

	Destroy();
}
