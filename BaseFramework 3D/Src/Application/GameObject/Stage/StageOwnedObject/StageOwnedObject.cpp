#include "StageOwnedObject.h"

#include"../../../Editor/EditorManager.h"

void StageOwnedObject::SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale)
{
	// AABBは回転を表現できないため、位置と大きさだけをStageに追従させる
	SetPos(stagePos + m_localPos * stageScale);
	SetScale(m_localScale * stageScale);
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
