#pragma once

class CharacterMover;
class CollisionProfiler;

class CharacterBase;

class CharacterPusher
{
public:

	using Characters = std::vector<std::shared_ptr<CharacterBase>>;


	// コンストラクタで参照で受け取る
	CharacterPusher(CharacterMover& mover,CollisionProfiler& profiler)
		:m_mover(mover), m_profiler(profiler) {}

	//==============================
	// キャラ同士の判定
	//==============================
	void ResolveCharacterCollision(const Characters& characters);


private:

	// 全ペアを調べてAddPushで押し量を溜める
	void AccumulatePush(const Characters& characters);

	// 溜まった押し量を位置に足してクリアする
	void ApplyPush(const Characters& characters);

	// 最初の位置から、動いたキャラだけ壁判定をやり直す
	void ResolveWallsAfterPush(const Characters& characters,const std::vector<Math::Vector3>&startPositions);


	// 書き込みもする参照
	CharacterMover   & m_mover;
	CollisionProfiler& m_profiler;
};