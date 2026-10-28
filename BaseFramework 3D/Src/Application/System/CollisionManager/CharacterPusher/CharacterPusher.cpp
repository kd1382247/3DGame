#include "CharacterPusher.h"

#include"../../../GameObject/Character/CharacterBase.h"

#include"../CharacterMover/CharacterMover.h"
#include"../CollisionProfiler/CollisionProfiler.h"

namespace
{
	using Section = CollisionProfiler::Section;
	using Counter = CollisionProfiler::Counter;
}

void CharacterPusher::ResolveCharacterCollision(const Characters& characters)
{
	constexpr int Iteration = 4;

	// 押し戻す前の位置を覚えておく(最後に壁判定を1回にするため)
	std::vector<Math::Vector3> startPositions;
	startPositions.reserve(characters.size());

	for (const auto& character : characters)
	{
		startPositions.push_back(character->GetPos());
	}

	for (int iter = 0; iter < Iteration; iter++)
	{
		// 全キャラの押し戻し量をクリア
		for (auto& character : characters)
		{
			character->ClearPush();
		}

		// 時間計測開始
		{
			// コンストラクタに区間をセット
			CollisionProfiler::ScopedTimer scopedTimer(m_profiler, Section::Pair);

			AccumulatePush(characters);

		}
		// 計測終了

		// 反復の間は、押し戻しを位置に足すだけ(壁判定はしない)
		ApplyPush(characters);
	}

	// 計測開始
	{
		// コンストラクタに区間をセット
		CollisionProfiler::ScopedTimer scopedTimer(m_profiler, Section::Push);

		ResolveWallsAfterPush(characters, startPositions);

	}
	// 計測終了
}

void CharacterPusher::AccumulatePush(const Characters& characters)
{

	// 全ペアと判定
	for (size_t i = 0; i < characters.size(); i++)
	{
		for (size_t j = i + 1; j < characters.size(); j++)
		{
			auto& charaA = characters[i];
			auto& charaB = characters[j];

			if (charaA->GetHealth().IsDead() ||
				charaB->GetHealth().IsDead())
			{
				continue;
			}

			KdCollider::SphereInfo sphereInfo(
				KdCollider::TypeBump,
				charaA->GetBumpSphere());

			std::list<KdCollider::CollisionResult>results;

			charaB->Intersects(sphereInfo, &results);

			if (!results.empty())
			{
				// カウントアップ
				m_profiler.AddCount(Counter::HitCount);
			}

			for (auto& ret : results)
			{
				// 相手から離れる方向
				Math::Vector3 dir = ret.m_hitDir;

				// character同士は水平方向だけ押し合う
				dir.y = 0;

				// 押し出す方向を求められない
				if (dir.LengthSquared() < 0.000001f)
				{
					continue;
				}


				dir.Normalize();

				float overlap = ret.m_overlapDistance;

				if (overlap <= 0.0f)
				{
					continue;
				}

				Math::Vector3 push = dir * overlap;

				float rateA = charaA->GetBumpPushRate();
				float rateB = charaB->GetBumpPushRate();

				float total = rateA + rateB;

				if (total <= 0)
				{
					total = 2.0f;
					rateA = total / 2;
					rateB = total / 2;
				}

				Math::Vector3 pushA = push * (rateA / total);
				charaA->AddPush(pushA);

				Math::Vector3 pushB = -push * (rateB / total);
				charaB->AddPush(pushB);

			}
		}
	}
}

void CharacterPusher::ApplyPush(const Characters & characters)
{
	for (const auto& character : characters)
	{
		character->ApplyPush();
		character->ClearPush();
	}
}

void CharacterPusher::ResolveWallsAfterPush(const Characters & characters, const std::vector<Math::Vector3>&startPositions)
{

	for (size_t i = 0; i < characters.size(); i++)
	{
		const auto& character = characters[i];

		// 押し戻しの合計(今の位置 - 最初の位置)
		const Math::Vector3 totalMove = character->GetPos() - startPositions[i];

		if (totalMove.LengthSquared() <= 0.000001f)
		{
			continue;
		}

		// カウントアップ
		m_profiler.AddCount(Counter::PushCount);

		// 最初の位置から、合計の移動量で壁判定をして、位置を決める
		const Math::Vector3 finalPos =
			m_mover.ResolveCharacterDisplacement(character, startPositions[i], totalMove, false);

		character->SetPos(finalPos);
	}

}
