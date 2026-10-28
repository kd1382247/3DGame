#pragma once

#include"CollisionLayer/CollisionLayer.h"
#include"CollisionRegistry/CollisionRegistry.h"
#include"CollisionProfiler/CollisionProfiler.h"
#include"CharacterMover/CharacterMover.h"
#include"CharacterPusher/CharacterPusher.h"

class KdGameObject;
class CharacterBase;

class CollisionManager
{
public:

	void DrawDebug();

	using objectList = std::vector<std::weak_ptr<KdGameObject>>;

	// 当たり判定リストに追加
	void RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
	{
		m_registry.RegisterObject(layer, object);
	}

	// 当たり判定リストから削除
	void UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
	{
		m_registry.UnregisterObject(layer, object);
	}

	// 指定のリストを取得
	const objectList& GetObjects(CollisionLayer layer)const
	{
		return m_registry.GetObjects(layer);
	}

	
	void RemoveExpiredObjects()
	{
		m_registry.RemoveExpiredObjects();
	}

	void Clear()
	{
		m_registry.Clear();
	}

	void Resolve();

private:

	void Init();

	//==============================
	// キャラリストを取得
	//==============================
	std::vector<std::shared_ptr<CharacterBase>> GetCharacters();
	
	std::unique_ptr<KdDebugWireFrame>m_pDebugWire=nullptr;


	using Section = CollisionProfiler::Section;

	// characterMoverにregistryの参照を渡すので
	// 宣言の順番に注意
	CollisionRegistry m_registry;

	CharacterMover   m_mover;

	// 当たり判定の時間計測
	CollisionProfiler m_profiler;


	CharacterPusher  m_pusher;



private: // シングルトンパターン

	CollisionManager():m_mover(m_registry),m_pusher(m_mover,m_profiler) { Init(); }
	~CollisionManager(){}

public:

	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}
};