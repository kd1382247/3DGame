#pragma once

#include"../../CharacterBase.h"
#include"../../../../System/CollisionManager/CollisionManager.h"

#include"../Parameter/PlayerParameter.h"

// 攻撃が「誰に当たったか」の記録と、球による攻撃判定をまとめたクラス
// Playerのことは知らない(攻撃する側はCharacterBaseとして受け取る)
class PlayerHitChecker
{
public:

	// 当たった相手の記録を消す(攻撃の開始時に呼ぶ)
	void Clear()
	{
		m_hitTargets.clear();
	}

	// 多段ヒット用タイマーを初期化する
	void ResetRehitTimer(const float intervalFrame)
	{
		m_rehitTimer = intervalFrame;
	}

	// intervalFrame(60fps換算)ごとに、当たった相手の記録を消す(同じ相手に何度も当てられるようにする)
	void UpdateRehit(const float deltaTime, const float intervalFrame)
	{
		m_rehitTimer -= 60.0f * deltaTime;

		if (m_rehitTimer <= 0.0f)
		{
			m_hitTargets.clear();
			m_rehitTimer = intervalFrame;
		}
	}

	// 球の範囲にいるキャラクターへダメージを与える
	// 自分自身・退場中のキャラ・すでに当たった相手は対象外
	void Check(const CharacterBase& attacker, const DirectX::BoundingSphere& sphere,
		const PlayerParameter::HitParam& hitParam)
	{
		const auto& characters =
			CollisionManager::Instance().GetObjects(CollisionLayer::CharacterBump);

		for (const auto& weakObj : characters)
		{
			auto obj = weakObj.lock();

			if (!obj)
			{
				continue;
			}

			// 自分自身は攻撃しない
			if (obj.get() == &attacker)
			{
				continue;
			}

			// キャラクター(Enemy・Boss)だけ取得
			auto target = std::dynamic_pointer_cast<CharacterBase>(obj);

			if (!target)
			{
				continue;
			}

			if (target->GetHealth().IsDead())
			{
				continue;
			}

			// 一度当たった相手はスキップ
			if (IsAlreadyHit(target))
			{
				continue;
			}

			KdCollider::SphereInfo sphereInfo(KdCollider::TypeBump, sphere);

			std::list<KdCollider::CollisionResult> result;

			if (target->Intersects(sphereInfo, &result) && !result.empty())
			{
				// ノックバック方向を作る
				Math::Vector3 knockBackDir = target->GetPos() - attacker.GetPos();
				knockBackDir.y = 0.0f;

				if (knockBackDir.LengthSquared() > 0.000001f)
				{
					knockBackDir.Normalize();
				}

				AttackInfo attackInfo;
				attackInfo.m_knockBackDir        = knockBackDir;
				attackInfo.m_damage              = hitParam.m_attackPower;
				attackInfo.m_knockBackPower      = hitParam.m_knockBackPower;
				attackInfo.m_hitStop             = hitParam.m_hitStop;
				attackInfo.m_killHitStop         = hitParam.m_killHitStop;
				attackInfo.m_SlowScale           = hitParam.m_SlowScale;
				attackInfo.m_SlowDuration        = hitParam.m_SlowDuration;


				target->OnHit(attackInfo);
				m_hitTargets.emplace_back(target);
			}
		}
	}

private:

	bool IsAlreadyHit(const std::shared_ptr<CharacterBase>& target) const
	{
		for (const auto& weakTarget : m_hitTargets)
		{
			auto hitTarget = weakTarget.lock();

			if (!hitTarget)
			{
				continue;
			}

			if (hitTarget == target)
			{
				return true;
			}
		}

		return false;
	}

private:

	// 攻撃が当たった相手のリスト
	std::vector<std::weak_ptr<CharacterBase>> m_hitTargets = {};

	// 多段ヒット用のタイマー(フレーム)
	float m_rehitTimer = 0.0f;
};
