#include "CollisionManager.h"

#include"../../../Framework/GameObject/KdGameObject.h"



namespace
{
	size_t ToIndex(CollisionLayer layer)
	{
		return static_cast<size_t>(layer);
	}
}


void CollisionManager::RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
{
	if (!object)
	{
		return;
	}

	objectList& objectList = m_objectLists[ToIndex(layer)];

	// 同じオブジェクトの二重登録を防ぐ
	for (const auto& weakObject : objectList)
	{
		const auto registerObject = weakObject.lock();

		if (registerObject == object)
		{
			return;
		}
	}

	objectList.emplace_back(object);
}

void CollisionManager::UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
{
	objectList& objectList = m_objectLists[ToIndex(layer)];

	std::erase_if(objectList,
		[&object](const std::weak_ptr<KdGameObject>& weakObject)
		{
			const auto registerObject = weakObject.lock();

			return !registerObject || registerObject == object;
		});
}

const CollisionManager::objectList& CollisionManager::GetObjects(CollisionLayer layer) const
{
	return m_objectLists[ToIndex(layer)];
}

void CollisionManager::RemoveExpiredObjects()
{
	for (auto& objectList : m_objectLists)
	{
		std::erase_if(objectList,
			[](const std::weak_ptr<KdGameObject>& weakObject)
			{
				return weakObject.expired();
			});
	}
}

void CollisionManager::Clear()
{
	for (auto& objectList : m_objectLists)
	{
		objectList.clear();
	}
	
}
