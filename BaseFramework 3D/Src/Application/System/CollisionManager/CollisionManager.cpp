#include "CollisionManager.h"

#include"../TimeManager/TimeManager.h"

#include"../../../Framework/GameObject/KdGameObject.h"

#include"../../GameObject/Character/CharacterBase.h"

void CollisionManager::DrawDebug()
{
	m_pDebugWire->Draw();
}

void CollisionManager::Init()
{
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

std::vector<std::shared_ptr<CharacterBase>> CollisionManager::GetCharacters()
{

	std::vector<std::shared_ptr<CharacterBase>> characters;

	for (auto weakChara :m_registry.GetObjects(CollisionLayer::CharacterBump) )
	{
		auto character = std::dynamic_pointer_cast<CharacterBase>(weakChara.lock());

		if (!character)
		{
			continue;
		}

		characters.push_back(character);
	}

	return characters;
}

void CollisionManager::Resolve()
{
	// 破棄済みのオブジェクト(weak_ptrが空になったもの)を登録リストから取り除く
	// (放置すると、リストが増え続けてメモリも使い続ける)
	RemoveExpiredObjects();

	// ヒットストップ中と、一時停止中(設定画面を開いている間など)は、当たり判定を解決しない
	if (TimeManager::Instance().IsHitStop() ||
		TimeManager::Instance().IsPaused())
	{
		return;
	}

	// 処理にかかる時間
	// 計測開始
	{
		// コンストラクタに区間をセット
		CollisionProfiler::ScopedTimer scopedTimer(m_profiler, Section::Total);

		// このフレームで使うキャラリストは1回だけ取得して使い回す
		const std::vector<std::shared_ptr<CharacterBase>> characters = GetCharacters();

		m_mover.BeginFrame(characters);

		// 計測開始
		{
			// 壁側の判定を測る
			// コンストラクタに区間をセット
			CollisionProfiler::ScopedTimer scopedTimer(m_profiler,Section::Movement);

			m_mover.ResolveCharacterMovement(characters);
		}
		// 計測終了

		// ノックバック
		for (const auto& character : characters)
		{
			if (!character)
			{
				continue;
			}

			m_mover.ApplyKnockBack(character);
		}
		
		// 計測開始
		{
			// キャラ同士の判定を測る
			// コンストラクタに区間をセット
			CollisionProfiler::ScopedTimer scopedTimer(m_profiler, Section::Character);

			// 最後にキャラ同士の判定をする
			m_pusher.ResolveCharacterCollision(characters);

		}
		// 計測終了
	}
	// 計測終了

	// フレームを加算し60を越えたら平均ログを更新
	m_profiler.EndFrame();

}
