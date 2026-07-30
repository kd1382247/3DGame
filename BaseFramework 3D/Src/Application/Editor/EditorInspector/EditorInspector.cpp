#include "EditorInspector.h"

#include"../EditorManager.h"

#include"../../Scene/SceneManager.h"

void EditorInspector::Draw()
{
	ImGui::Begin("Inspector");

	auto& obj = EditorManager::Instance().GetSelectedObject();

	if (obj)
	{

		if (ImGui::Button("Duplicate"))
		{
			auto duplicate = std::make_shared<KdGameObject>();

			duplicate->SetObjectName(obj->GetObjectName() + " Copy");
			duplicate->SetPos(obj->GetPos());
			duplicate->SetScale(obj->GetScale());

			SceneManager::Instance().AddObject(duplicate);
			EditorManager::Instance().SetSelectedObject(duplicate);
		}

		// オブジェクトの名前変更
		char nameBaffer[128];

		strcpy_s(
			nameBaffer,
			sizeof(nameBaffer),
			obj->GetObjectName().c_str()
		);

		if (ImGui::InputText("Name", nameBaffer, sizeof(nameBaffer)))
		{
			obj->SetObjectName(nameBaffer);
		}


		// 現在選択中のオブジェクト名表示
		ImGui::Text(obj->GetObjectName().c_str());

		Math::Vector3 pos = obj->GetPos();

		// 座標変更(0.1fずつ移動)
		if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
		{
			obj->SetPos(pos);
		}

		Math::Vector3 scale = obj->GetScale();

		// 大きさ変更
		if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
		{
			obj->SetScale(scale);
		}

		// 回転
		Math::Vector3 rotation=obj->GetRotation();

		if (ImGui::DragFloat3("Rotation", &rotation.x, 0.01))
		{

			//if (EditorManager::Instance().GetMode() == EditorManager::EditorMode::Edit)
			{
				obj->SetRotation(rotation);
			}

		}


		// オブジェクト削除
		if (ImGui::Button("Delete"))
		{
			obj->Destroy();
			EditorManager::Instance().SetSelectedObject(nullptr);
		}

	}

	ImGui::End();
}

void EditorInspector::NameChange()
{

}

void EditorInspector::UpdatePos()
{}

void EditorInspector::UpdateScale()
{}

void EditorInspector::Delete()
{}
