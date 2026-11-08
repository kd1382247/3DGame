#include "StageOwnedObject.h"

#include"../../../Editor/EditorManager.h"

#include"../StageObject.h"

void StageOwnedObject::PreDraw()
{
	FollowOwner();
}

void StageOwnedObject::FollowOwner()
{
	m_parentLink.Update(*this);
}

std::shared_ptr<StageObject> StageOwnedObject::GetOwner() const
{
	auto stageObj = std::dynamic_pointer_cast<StageObject>(m_parentLink.GetParent());

	return stageObj;
}

void StageOwnedObject::SetOwner(const std::shared_ptr<StageObject>& owner)
{
	m_parentLink.SetParent(owner);
	FollowOwner();
}

void StageOwnedObject::CopyFrom(const std::shared_ptr<StageOwnedObject>& ownedObj)
{
	SetLocalPos(ownedObj->GetLocalPos());
	SetLocalScale(ownedObj->GetLocalScale());
}

void StageOwnedObject::DrawInspectorHeader()
{
	// 座標変更(Stageからのローカル位置)
	Math::Vector3 pos = GetLocalPos();

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		SetLocalPos(pos);
		EditorManager::Instance().MarkDirty();
	}

	Math::Vector3 scale = GetLocalScale();

	// 大きさ変更(Stageからのローカル大きさ)
	if (ImGui::DragFloat3("Size", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
		EditorManager::Instance().MarkDirty();
	}
}
