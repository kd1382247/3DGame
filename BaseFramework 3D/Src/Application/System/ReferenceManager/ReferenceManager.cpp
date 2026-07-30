#include "ReferenceManager.h"

void ReferenceManager::AssociateClass()
{

	for (auto& obj : m_referenceList)
	{
		if (!obj)
		{
			continue;
		}
		// 必要に応じて関連付けを行う
		obj->SetUpReference();
	}

	// 関連付を終えた後は配列をクリアする
	m_referenceList.clear();

}
