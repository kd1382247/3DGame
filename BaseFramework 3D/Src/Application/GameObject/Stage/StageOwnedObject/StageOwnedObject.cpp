#include "StageOwnedObject.h"

#include"../StageObject.h"

StageOwnedObject::StageOwnedObject()
{
	// 部品の実体を作って受け取る
	m_spParentLink = AddComponent<ParentLink>();
}

void StageOwnedObject::PreDraw()
{
	// 部品の更新
	UpdateComponents();
}

void StageOwnedObject::FollowParent()
{
	m_spParentLink->Apply(*this);
}

std::shared_ptr<StageObject> StageOwnedObject::GetParent() const
{
	auto stageObj = std::dynamic_pointer_cast<StageObject>(m_spParentLink->GetParent());

	return stageObj;
}

void StageOwnedObject::SetParent(const std::shared_ptr<StageObject>& owner)
{
	m_spParentLink->SetParent(owner);
	FollowParent();
}

void StageOwnedObject::SetParentKeepWorld(const std::shared_ptr<StageObject>& stage)
{
	m_spParentLink->SetParentKeepWorld(stage, *this);
}

void StageOwnedObject::CopyFrom(const std::shared_ptr<StageOwnedObject>& ownedObj)
{
	SetLocalPos     (ownedObj->GetLocalPos());
	SetLocalScale   (ownedObj->GetLocalScale());
	SetLocalRotation(ownedObj->GetLocalRotation());
}

void StageOwnedObject::DrawInspectorHeader()
{
	// 親がいない場合
	if (!HasParent())
	{
		DrawPositionInspector();
		DrawScaleInspector();
	}


}

