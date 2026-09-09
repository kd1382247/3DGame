#include "CollisionRegistry.h"


namespace
{
	size_t ToIndex(CollisionLayer layer)
	{
		return static_cast<size_t>(layer);
	}
}

void CollisionRegistry::RegisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
{
	if (!object)
	{
		return;
	}

	objectList& objects = m_objectLists[ToIndex(layer)];

	// 既に登録されていないか調べる
	for (const auto& weakObject : objects)
	{
		const auto registerObject = weakObject.lock();

		if (registerObject == object)
		{
			return;
		}
	}

	objects.emplace_back(object);
}

void CollisionRegistry::UnregisterObject(CollisionLayer layer, const std::shared_ptr<KdGameObject>& object)
{
	if (!object)
	{
		return;
	}

	objectList& objects = m_objectLists[ToIndex(layer)];

	std::erase_if(objects
		, [&object](const std::weak_ptr<KdGameObject>weakObj)
		{
			const auto registeredObj = weakObj.lock();

			return !registeredObj || registeredObj == object;
		});
}

const CollisionRegistry::objectList& CollisionRegistry::GetObjects(CollisionLayer layer) const
{
	return m_objectLists[ToIndex(layer)];
}

void CollisionRegistry::RemoveExpiredObjects()
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

void CollisionRegistry::Clear()
{
	for (auto& objectList : m_objectLists)
	{
		objectList.clear();
	}
}
