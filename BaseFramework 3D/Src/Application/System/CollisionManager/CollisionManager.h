#pragma once

#include"CollisionLayer/CollisionLayer.h"
#include"CollisionRegistry/CollisionRegistry.h"

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

	struct SweepResult
	{
		bool m_hit = false;
		float m_toi = 1.0f;

		std::vector<Math::Vector3>m_normals;
	};
	

	void ResolveSweepContacts(
		const std::shared_ptr<CharacterBase>& character,
		const SweepResult& sweepResult,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		const Math::Vector3& sourceMove,
		bool updateGroundState);

	SweepResult FindSweepContacts(
		const std::shared_ptr<CharacterBase>& character,
		const Math::Vector3& currentPos,
		const Math::Vector3& remainingMove);

	void AddSweepContact(
		SweepResult& result,
		float toi,
		const Math::Vector3& normal,
		float moveLength);

	void ResolveStartOverlapContact(
		const Math::Vector3& push,
		const Math::Vector3& normal,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		bool isWalkable);

	// 歩行可能面(坂)では、水平方向(X,Z)の移動量は入力のまま保ち、
	// 坂の平面の方程式から必要な高さ(Y)だけを再計算することで、
	// 斜め移動時も方向を歪ませずに水平速度を保つ
	void PreserveHorizontalSpeedOnSlope(
		const Math::Vector3& preProjectMove,
		const Math::Vector3& normal,
		Math::Vector3& move) const;


	void ResolveMultipleSurfaceHit(
		const std::shared_ptr<CharacterBase>& character,
		const SweepResult& sweepResult,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		const Math::Vector3& sourceMove,
		bool updateGroundState);


	void UpdateCharacterSurfaceState(
		const std::shared_ptr<CharacterBase>& character,
		const Math::Vector3& normal,
		const Math::Vector3& sourceMove,
		bool updateGroundState);

	//==============================
	// キャラリストを取得
	//==============================
	std::vector<std::shared_ptr<CharacterBase>> GetCharacters();


	//==============================
	// キャラの移動
	//==============================

	void ResolveCharacterMovement(const std::vector<std::shared_ptr<CharacterBase>>& characters);

	Math::Vector3 ResolveCharacterDisplacement(
		const std::shared_ptr<CharacterBase>& character,
		const Math::Vector3& startPos,
		const Math::Vector3& move,
		bool updateGroundState);

	//==============================
	// めり込みを解決
	//==============================

	void ResolveAABBStartOverlap(
		const std::shared_ptr<CharacterBase>& character,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove);

	void ResolveOBBStartOverlap(
		const std::shared_ptr<CharacterBase>& character,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove);

	//==============================
	// スウィープ判定
	//==============================
	void ResolveSweepHit(
		const std::shared_ptr<CharacterBase>& character,
		float toi,
		const Math::Vector3 &normal,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		const Math::Vector3&sourceMove,
		bool updateGroundState);


	bool IsWalkableSurface(
		const Math::Vector3 &normal,
		const std::shared_ptr<CharacterBase>& character)const;

	// キャラの原点がposにあるとした場合のバンプスフィアの中心座標を求める
	// (ResolveAABBStartOverlap/ResolveOBBStartOverlap/FindSweepContactsで共通利用)
	Math::Vector3 GetBumpSphereCenterAt(
		const std::shared_ptr<CharacterBase>& character,
		const Math::Vector3& pos) const;

	// 坂用(Walkable)OBBとの接触を無視すべきか判定する
	// (FindSweepContacts/ResolveOBBStartOverlapで共通利用)
	// isObbTypeWalkable: そのOBB自体がWalkable種別かどうか
	// outIsWalkable    : 登れる角度の歩行可能面としての接触だったか
	bool ShouldSkipWalkableObbContact(
		bool isObbTypeWalkable,
		const Math::Vector3& normal,
		const std::shared_ptr<CharacterBase>& character,
		bool& outIsWalkable) const;

	//==============================
	// キャラ同士の判定
	//==============================
	void ResolveCharacterCollision(const std::vector<std::shared_ptr<CharacterBase>>& characters);

	void ApplyCharacterPush(const std::shared_ptr<CharacterBase>& character);
	void ApplyKnockBack(const std::shared_ptr<CharacterBase>& character);


	CollisionRegistry m_registry;

	std::unique_ptr<KdDebugWireFrame>m_pDebugWire=nullptr;

private: // シングルトンパターン

	CollisionManager() { Init(); }
	~CollisionManager(){}

public:

	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}
};