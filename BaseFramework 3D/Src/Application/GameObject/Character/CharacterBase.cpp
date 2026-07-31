#include "CharacterBase.h"

#include"../../System/CollisionManager/CollisionManager.h"

CharacterBase::CharacterBase()
{}

CharacterBase::~CharacterBase()
{
	Release();
}

// 初期化
void CharacterBase::Init()
{}

// 更新
void CharacterBase::Update()
{}

void CharacterBase::PostUpdate()
{
	UpdateCollision();
}

// 描画
void CharacterBase::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::GenerateDepthMapFromLight()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().
			m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void CharacterBase::UpdateCollision()
{
	UpdateBump();

	UpdateGroundCollision();
}

void CharacterBase::UpdateBump()
{
	// ①当たり判定(球判定)用の情報作成
	DirectX::BoundingSphere sphere;
	sphere.Center = GetPos() + Math::Vector3(0, 0.5f, 0);
	sphere.Radius = 0.5f;
	KdCollider::SphereInfo spherInfo(KdCollider::TypeBump, sphere);

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : CollisionManager::Instance().GetObjects(CollisionLayer::Bump))
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();

		if (spGameObj == shared_from_this())
		{
			continue;
		}

		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retBumpList;
			spGameObj->Intersects(spherInfo, &retBumpList);

			// ③ 結果を使って座標を補完する
			for (auto& ret : retBumpList)
			{
				Math::Vector3 newPos = GetPos() + (ret.m_hitDir * ret.m_overlapDistance);
				SetPos(newPos);
			}
		}
	}
}

void CharacterBase::UpdateGroundCollision()
{
	// 地面判定するよ
	// ----- ----- ----- ----- -----

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高いところから飛ばす(段差の許容範囲)
	static float enableStepHigh = 0.2f;
	rayInfo.m_pos.y += enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さを設定
	rayInfo.m_range = m_Gravity + enableStepHigh;
	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	// ②HIT判定対象オブジェクトに総当たり
	for (auto& wpGameObj : CollisionManager::Instance().GetObjects(CollisionLayer::Ground))
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③ 結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			if (hit)
			{
				// 地面に当たっている
				SetPos(hitPos);
				m_Gravity = 0;
				m_jumpFlg = false;
			}
		}
	}

}

// 解放
void CharacterBase::Release()
{
	m_spModel = nullptr;
}