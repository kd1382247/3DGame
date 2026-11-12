#include "StageGimmick.h"


void StageGimmick::DrawInspectorHeader()
{
	DrawNameInspector();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		StageOwnedObject::DrawInspectorHeader();

		if(!UsesLocalRotation())
		{
			DrawRotationInspector();
		}
	}

}