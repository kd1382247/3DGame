#include "StageOwnedObject.h"

#include"../../../Editor/EditorManager.h"

#include"../StageObject.h"

void StageOwnedObject::PreDraw()
{
	FollowOwner();
}

void StageOwnedObject::FollowOwner()
{
	auto spOwner = m_wpOwner.lock();

	if (!spOwner)
	{
		return;
	}

	// 位置と大きさだけをStageに追従させる
	SetPos(spOwner->GetPos() + m_localPos * spOwner->GetScale());
	SetScale(m_localScale * spOwner->GetScale());

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
