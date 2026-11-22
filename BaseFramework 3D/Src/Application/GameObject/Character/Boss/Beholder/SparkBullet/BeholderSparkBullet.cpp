#include "BeholderSparkBullet.h"

#include"../../../CharacterBase.h"
#include"../../../Player/Player.h"
#include"../../Beholder/Beholder.h"
#include"../../../AttackInfo.h"

#include"../../../../../System/GameObjectFinder/GameObjectFinder.h"
#include"../../../../../System/TimeManager/TimeManager.h"
#include"../../../../../System/CollisionManager/CollisionManager.h"

namespace
{
 	static const int angleNum = 6;

	float curveAngle[angleNum] = {
		{ 135 },{ 225 },{ 270 },{ 0 },{ 45 },{ 90 }
	};

	float GetCurveAngle(int number)
	{
		number = std::clamp(number, 0, angleNum);

		return curveAngle[number];
	}

	float reflectCurveAngle[angleNum] = {
		{ 45 },{ 315 },{ 270 },{ 180 },{ 135 },{ 90 }
	};

	float GetReflectCurveAngle(int number)
	{
		number = std::clamp(number, 0, angleNum);

		return reflectCurveAngle[number];
	}

}

void BeholderSparkBullet::Init()
{
	SetUpReference();
}

void BeholderSparkBullet::SetUpReference()
{
	m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
}

void BeholderSparkBullet::Setup(const BeholderSparkBulletDesc& desc)
{
	m_desc = desc;

	// 膨らむ方向
	m_desc.curveAngle = GetCurveAngle(desc.nullPointNumber);


	SetPos(desc.pos);
}

void BeholderSparkBullet::Update()
{
	// 撃ったボスがいなくなったら消える
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		Vanish();
		return;
	}

	switch (m_fireState)
	{
	case BeholderSparkBullet::Preparation:

		// 発射準備の更新
		UpdatePreparation();
		break;
	case BeholderSparkBullet::Standby:

		// 発射待機の更新
		UpdateStandby();
		break;
	case BeholderSparkBullet::Firing:

		// 発射中の更新
		UpdateFiring();
		break;
		case BeholderSparkBullet::Reflect:

		// 発射中の更新
		UpdateReflect();
		break;
	}
}

bool BeholderSparkBullet::Move(float deltaTime)
{
	// 進行方向を出す
	Math::Vector3 moveDir = m_endPos - m_startPos;

	float controlDist = moveDir.Length();

	moveDir.Normalize();

	// 右を作る
	Math::Vector3 right = Math::Vector3::Up.Cross(moveDir);
	right.Normalize();

	// 真上を作る
	Math::Vector3 up2 = moveDir.Cross(right);

	// 膨らむ方向 (45°の場合右上に膨らむ)
	Math::Vector3 sideDir = right * std::cos(DirectX::XMConvertToRadians(m_desc.curveAngle)) +
		                      up2 * std::sin(DirectX::XMConvertToRadians(m_desc.curveAngle));



	// 中間点を決める
	m_controlPos = (m_startPos + m_endPos) / 2 + sideDir * controlDist * m_desc.curveRate;

	// m_tが一定以上進んだら弾を消す
	if (m_t >=m_desc.vanishT)
	{
		return false;
	}

	// 開始地点から制御点まで
	Math::Vector3 startToControl = Math::Vector3::Lerp(m_startPos, m_controlPos, m_t);

	// 制御点から終点まで
	Math::Vector3 controlToEnd = Math::Vector3::Lerp(m_controlPos, m_endPos, m_t);
	
	// 開始から終点までの位置
	Math::Vector3 bulletPos = Math::Vector3::Lerp(startToControl,controlToEnd, m_t);

	// 1フレームに進む距離を出す
	float lengthParT = (controlToEnd - startToControl).Length() * 2;
	float moveSpeed = m_desc.speed;

	if (lengthParT >= 0.000001f)
	{
		// 1フレームで進めたい距離を出す
		moveSpeed /= lengthParT;
	}

	SetPos(bulletPos);

	m_t += moveSpeed;

	return true;
}

void BeholderSparkBullet::ReflectSetUp()
{
	// 反射用の角度を入れる
	m_desc.curveAngle=GetReflectCurveAngle(m_desc.nullPointNumber);

	// スタートは弾のいる位置
	m_startPos = GetPos();

	// 目標点は持ち主
	if (auto spOwner = m_wpOwner.lock())
	{
		m_endPos = spOwner->GetBumpSphere().Center;
	}

	m_desc.speed = m_desc.reflectSpeed;

	// 進み具合をリセット
	m_t = 0.0f;
}

bool BeholderSparkBullet::IsHitMap(const KdCollider::SphereInfo& sphereInfo)const
{
	// 壁(AABB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::AABB))
	{
		auto spObj = wpObj.lock();

		if (!spObj) { continue; }

		if (spObj->Intersects(sphereInfo, nullptr)) { return true; }
	}

	// 壁(OBB)
	for (auto& wpObj : CollisionManager::Instance().GetObjects(CollisionLayer::OBB))
	{
		auto spObj = wpObj.lock();

		if (!spObj) { continue; }

		if (spObj->Intersects(sphereInfo, nullptr)) { return true; }
	}

	return false;
}

void BeholderSparkBullet::HitPlayer(const KdCollider::SphereInfo& sphereInfo)
{
	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer) { return; }

	if (!spPlayer->Intersects(sphereInfo, nullptr)) { return; }

	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spPlayer->GetPos() - GetPos();
	knockBackDir.y = 0;

	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir   = knockBackDir;
	attackInfo.m_knockBackPower = m_desc.knockBackPower;
	attackInfo.m_damage         = m_desc.damage;
	attackInfo.m_canReflect     = true;

	spPlayer->OnHit(attackInfo);

	// パリィされたら打ち返す。そうでなければ、当たって消える
	if (spPlayer->GetParry().GetIsParrySuccess())
	{
		m_fireState = FireState::Reflect;
		ReflectSetUp();
	}
	else
	{
		Vanish();
	}
}

void BeholderSparkBullet::HitOwner(const KdCollider::SphereInfo& sphereInfo)
{
	auto spOwner = std::dynamic_pointer_cast<Beholder>( m_wpOwner.lock());

	if (!spOwner) { return; }

	if (!spOwner->Intersects(sphereInfo, nullptr)) { return; }

	// ノックバックの方向を作る
	Math::Vector3 knockBackDir = spOwner->GetPos() - GetPos();
	knockBackDir.y = 0;

	if (knockBackDir.LengthSquared() > 0.000001f)
	{
		knockBackDir.Normalize();
	}

	AttackInfo attackInfo;

	attackInfo.m_knockBackDir   = knockBackDir;
	attackInfo.m_knockBackPower = m_desc.knockBackPower;
	attackInfo.m_damage         = m_desc.damage;
	attackInfo.m_canReflect     = true;

	spOwner->OnHit(attackInfo);

	spOwner->DizzyCountUp();

	Vanish();
}

void BeholderSparkBullet::PlayEffect(const Math::Vector3& pos)
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

void BeholderSparkBullet::UpdatePreparationEffect()
{
	// 一度だけエフェクト再生を再生
	if (!m_isPlayEffect)
	{
		auto spEffect = m_wpEffect.lock();
		if (!spEffect || !spEffect->IsPlaying())
		{
			m_wpEffect = KdEffekseerManager::GetInstance().Play(
				m_desc.effect.fileName,
				GetPos(),
				m_desc.effect.size,
				0.2f,
				false,
				0, 10);

			m_isPlayEffect = true;
		}
	}
	else
	{
		// エフェクトの再生が終わったら待機状態にする
		auto spEffect = m_wpEffect.lock();

		if (!spEffect || !spEffect->IsPlaying())
		{
			PlayEffect(GetPos());
			m_fireState = FireState::Standby;
		}
		else
		{
			spEffect->SetPos(GetPos());
		}
	}
}

void BeholderSparkBullet::UpdateEffect()
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

void BeholderSparkBullet::UpdateNullPointNodePos()
{
	auto spOwner = m_wpOwner.lock();

	if (!spOwner) { return; }

	// 数字を文字に変換
	std::string number = std::to_string(m_desc.nullPointNumber);

	const KdModelWork::Node* pNode = spOwner->GetModel()->FindNode("AttackPoint" + number);

	if (!pNode) { return; }

	// 行列作成
	Math::Matrix worldMat = pNode->m_worldTransform * spOwner->GetMatrix();

	m_startPos = worldMat.Translation();

	SetPos(worldMat.Translation());
}

void BeholderSparkBullet::UpdatePreparation()
{
	// NullPointの座標を更新
	UpdateNullPointNodePos();

	// エフェクト更新
	UpdatePreparationEffect();
}

void BeholderSparkBullet::UpdateStandby()
{
	UpdateNullPointNodePos();

	// 終着地点
	if (auto spPlayer = m_wpPlayer.lock())
	{
		Math::Vector3 playerPos = spPlayer->GetPos();
		Math::Vector3 offsetPos = {};

		offsetPos.y = spPlayer->GetBumpSphereOffsetY();

		m_endPos = playerPos + offsetPos;
	}

	UpdateEffect();
}

void BeholderSparkBullet::UpdateFiring()
{
	const float deltaTime = TimeManager::Instance().GetDeltaTime();

	if (!Move(deltaTime))
	{
		Vanish();
		return;
	}

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

	HitPlayer(sphereInfo);

	UpdateEffect();
}

void BeholderSparkBullet::UpdateReflect()
{
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		return;
	}

	const float deltaTime = TimeManager::Instance().GetDeltaTime();

	if (!Move(deltaTime))
	{
		Vanish();
		return;
	}

	// 反射された弾の持ち主のSphereを作る
	DirectX::BoundingSphere sphere;

	sphere.Center = GetPos();
	sphere.Radius = m_desc.radius;

	KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump,sphere);

	// ボスに当たるかを見る
	HitOwner(sphereInfo);

	UpdateEffect();
}

void BeholderSparkBullet::Vanish()
{
	if (auto spEffect = m_wpEffect.lock())
	{
		spEffect->StopEffect();
	}

	KdEffekseerManager::GetInstance().Play("Boss/Beholder/Spark/SparkBreak.efkefc", GetPos(),0.6, 1.0f, false, 30, 60);

	Destroy();
}
