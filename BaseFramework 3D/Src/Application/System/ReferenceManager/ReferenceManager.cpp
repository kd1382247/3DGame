#include "ReferenceManager.h"



void ReferenceManager::AssociateObject(const std::shared_ptr<KdGameObject>& obj)
{
	if (!obj)
	{
		return;
	}

	obj->SetUpReference();

}

void ReferenceManager::AssociateObjects(const std::list<std::shared_ptr<KdGameObject>>& objList)
{
	for (const auto& obj : objList)
	{
		AssociateObject(obj);
	}
}