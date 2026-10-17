#include "PointLight.h"

#include "PointLightManager.h"

#include "../Stage/StageObject.h"
#include "../../Editor/EditorManager.h"
#include "../../Editor/InspectorWidgets.h"

void PointLightObject::Init()
{
	SetObjectName("PointLight");

	// Noneはステージデータ(StageData.json)には保存しない。
	// PointLightはPointLightManagerがステージモデルごとのJsonに保存する
	SetObjectCategory(KdGameObject::ObjectCategory::None);

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape
	("PointLight", Math::Vector3(0, 0, 0), kRadius, KdCollider::TypeBump);

}

void PointLightObject::Update()
{
	ApplyLight();
}

void PointLightObject::ApplyLight()
{
	if (m_hasOwner)
	{
		auto owner = m_wpOwner.lock();

		// 持ち主のステージが消えたら、ライトも一緒に消す
		if (!owner || owner->IsExpired())
		{
			Destroy();
			return;
		}
	}

	// Stageに追従
	UpdateWorldPos();

	// ポイントライト
	KdShaderManager::Instance().WorkAmbientController().AddPointLight
	(
		{ m_color.x,m_color.y,m_color.z },
		m_range,
		GetPos()
	);

}

void PointLightObject::UpdateWorldPos()
{
	auto owner = m_wpOwner.lock();

	if (!owner)
	{
		// 持ち主がいない間は、ローカル位置をそのままワールド位置にする
		SetPos(m_localPos);
		return;
	}

	// ステージの位置・回転・大きさを反映したワールド位置
	SetPos(Math::Vector3::Transform(m_localPos, owner->GetMatrix()));
}

void PointLightObject::DrawDebug()
{
	// Play中は表示しない
	if (!EditorManager::Instance().IsEditMode())
	{
		return;
	}

	// 選択中のライトは、デバッグ表示がOFFでも表示する
	if (!IsSelected() &&
		!PointLightObjectManager::Instance().IsDebug())
	{
		return;
	}

	// Active Stageのライトだけ表示する
	if (GetOwner() != EditorManager::Instance().GetActiveStage())
	{
		return;
	}

	Math::Color color;

	// 球の表示
	if (IsSelected())
	{
		color = kRedColor + kGreenColor;

		// 選択中は、ライトが届く範囲も表示する
		m_pDebugWire->AddDebugSphere(GetPos(), m_range, color);
	}
	else
	{
		color = kGreenColor;
	}

	m_pDebugWire->AddDebugSphere(GetPos(), kRadius, color);
	KdGameObject::DrawDebug();
}

void PointLightObject::DrawInspectorHeader()
{
	// 座標変更(Stageからのローカル位置)
	Math::Vector3 pos = m_localPos;

	if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
	{
		m_localPos = pos;
		EditorManager::Instance().MarkDirty();
	}


	// Stageに追従
	UpdateWorldPos();
}

void PointLightObject::DrawInspector()
{
	// 色(1を超える明るさも指定できる)
	InspectorUI::Notify(ImGui::ColorEdit3(
		"Color",
		&m_color.x,
		ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float));

	// ライトの届く範囲
	InspectorUI::Meter("Range", m_range);
}
