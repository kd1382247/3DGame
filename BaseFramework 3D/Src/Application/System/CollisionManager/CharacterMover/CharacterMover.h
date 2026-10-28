#pragma once

class CharacterBase;
class CollisionRegistry;

#include<unordered_set>

class CharacterMover
{
public:

	// CollisionRegistryを読み取り専用の参照を受け取る
	CharacterMover(const CollisionRegistry& registry) :m_registry(registry) {}
	~CharacterMover(){}

	void BeginFrame(const std::vector<std::shared_ptr<CharacterBase>>&characters);

	//==============================
	// キャラの移動
	//==============================

	void ResolveCharacterMovement(const std::vector<std::shared_ptr<CharacterBase>>& characters);

	Math::Vector3 ResolveCharacterDisplacement(
		const std::shared_ptr<CharacterBase>& character,
		const Math::Vector3& startPos,
		const Math::Vector3& move,
		bool updateGroundState);

	// ノックバックを適用
	void ApplyKnockBack(const std::shared_ptr<CharacterBase>& character);

private:


	struct SweepResult
	{
		bool m_hit = false;
		float m_toi = 1.0f;

		std::vector<Math::Vector3>m_normals;
	};

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

	// 部屋の柵(RoomGate)とのめり込みを解決
	void ResolveGateStartOverlap(
		const std::shared_ptr<CharacterBase>& character,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove);

	// 敵のスポナー(EnemySpawner)とのめり込みを解決
	void ResolveSpawnerStartOverlap(
		const std::shared_ptr<CharacterBase>& character,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove);


	void ResolveStartOverlapContact(
		const Math::Vector3& push,
		const Math::Vector3& normal,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		bool isWalkable);

	//==============================
	// スウィープ判定
	//==============================

	void ResolveSweepHit(
		const std::shared_ptr<CharacterBase>& character,
		float toi,
		const Math::Vector3& normal,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		const Math::Vector3& sourceMove,
		bool updateGroundState);

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


	void ResolveMultipleSurfaceHit(
		const std::shared_ptr<CharacterBase>& character,
		const SweepResult& sweepResult,
		Math::Vector3& currentPos,
		Math::Vector3& remainingMove,
		const Math::Vector3& sourceMove,
		bool updateGroundState);


	//==========================
	// 面の状態
	//==========================

	// 坂用(Walkable)OBBとの接触を無視すべきか判定する
	// (FindSweepContacts/ResolveOBBStartOverlapで共通利用)
	// isObbTypeWalkable: そのOBB自体がWalkable種別かどうか
	// outIsWalkable    : 登れる角度の歩行可能面としての接触だったか
	bool ShouldSkipWalkableObbContact(
		bool isObbTypeWalkable,
		const Math::Vector3& normal,
		const std::shared_ptr<CharacterBase>& character,
		bool& outIsWalkable) const;


	void UpdateCharacterSurfaceState(
		const std::shared_ptr<CharacterBase>& character,
		const Math::Vector3& normal,
		const Math::Vector3& sourceMove,
		bool updateGroundState);

	// 下り坂などで地面から離れてしまった場合に、真下の歩行可能面へ吸い付かせる
	// (前フレーム接地 かつ 今フレーム未接地 かつ 上昇中でない 場合だけ働く)
	// sourceMove: このフレームの元の移動量(水平移動量から吸い付く距離を決める)
	void SnapToGround(
		const std::shared_ptr<CharacterBase>& character,
		Math::Vector3& currentPos,
		const Math::Vector3& sourceMove);

	// 登れる面かを返す
	bool IsWalkableSurface(
		const Math::Vector3& normal,
		const std::shared_ptr<CharacterBase>& character)const;
	

	const CollisionRegistry& m_registry;


	// 接地したキャラの記録(SnapToGroundで「前フレームに接地していたか」を判定するため)
	// m_groundedPrev: 前フレームに接地していたキャラ
	// m_groundedCurr: 今フレーム接地したキャラ
	std::unordered_set<const CharacterBase*> m_groundedPrev;
	std::unordered_set<const CharacterBase*> m_groundedCurr;

	// 調べる範囲の余白
	static constexpr float CullMargin = 0.05f;


};