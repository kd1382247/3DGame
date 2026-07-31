#pragma once

#include"CollisionLayer/CollisionLayer.h"

class KdGameObject;

class CollisionManager
{
public:

	using objectList = std::vector<std::weak_ptr<KdGameObject>>;


	void RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object);

	void UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object);

	const objectList& GetObjects(CollisionLayer layer)const;

	void RemoveExpiredObjects();

	void Clear();


private:

	// レイヤーのサイズを取得
	static constexpr size_t LayerCount =
		static_cast<size_t>(CollisionLayer::Max);

	std::array<objectList, LayerCount>m_objectLists;


private: // シングルトンパターン

	CollisionManager(){}
	~CollisionManager(){}

public:

	static CollisionManager& Instance()
	{
		static CollisionManager instance;
		return instance;
	}
};