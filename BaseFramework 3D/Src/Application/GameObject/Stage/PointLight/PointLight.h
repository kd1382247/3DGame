#pragma once

#include"../StageOwnedObject/StageOwnedObject.h"

class StageObject;

class PointLightObject :public StageOwnedObject
{
public:

	void Init()override;
	void Update()override;

	void DrawDebug()override;

	// 位置(Stageからのローカル位置)は、Inspectorの固定部分に出す
	void DrawInspectorHeader()override;

	// 色・範囲は、Inspectorのスクロールする部分に出す
	void DrawInspector()override;

	// 持ち主の確認・シェーダーへのライト登録をまとめて行う
	// (Play中はUpdateから、Edit中はBaseScene::EditorUpdateから、毎フレーム呼ばれる)
	void ApplyLight();

	// ライトの色(1を超える値も使える)
	const Math::Vector3& GetColor()const { return m_color; }
	void SetColor(const Math::Vector3& color) { m_color = color; }

	// ライトの届く範囲(m)
	float GetRange()const { return m_range; }
	void SetRange(float range) { m_range = range; }

	// コピーされたときに呼ばれる
	void CopyFrom(const std::shared_ptr<StageOwnedObject>& ownedObj)override;

private:

	// デバッグスフィア・クリック選択用の当たり判定の半径
	static constexpr float kRadius = 0.5f;

	// ポイントライトの色と範囲
	Math::Vector3 m_color = { 3.0f,3.0f,3.0f };
	float m_range = 3.0f;

};
