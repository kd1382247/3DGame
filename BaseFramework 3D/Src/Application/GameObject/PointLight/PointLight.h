#pragma once

class StageObject;

class PointLightObject :public KdGameObject
{
public:

	void Init()override;
	void Update()override;

	void DrawDebug()override;

	// 位置(Stageからのローカル位置)は、Inspectorの固定部分に出す
	void DrawInspectorHeader()override;

	// 色・範囲は、Inspectorのスクロールする部分に出す
	void DrawInspector()override;

	// Stageのワールド行列から、ライトのワールド位置を求めて反映する
	void UpdateWorldPos();

	// 持ち主の確認・ステージへの追従・シェーダーへのライト登録をまとめて行う
	// (Play中はUpdateから、Edit中はBaseScene::EditorUpdateから、毎フレーム呼ばれる)
	void ApplyLight();

	int GetID()const { return m_id; }
	void SetID(int id) { m_id = id; }

	// Stageからのローカル位置(Inspectorではこちらを編集する)
	const Math::Vector3& GetLocalPos()const { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos) { m_localPos = pos; }

	// ライトの色(1を超える値も使える)
	const Math::Vector3& GetColor()const { return m_color; }
	void SetColor(const Math::Vector3& color) { m_color = color; }

	// ライトの届く範囲(m)
	float GetRange()const { return m_range; }
	void SetRange(float range) { m_range = range; }

	// ライトの持ち主(ステージ)。ステージに追従し、ステージが消えると一緒に消える
	void SetOwner(const std::shared_ptr<StageObject>& owner)
	{
		m_wpOwner = owner;
		m_hasOwner = (owner != nullptr);
	}
	std::shared_ptr<StageObject> GetOwner()const { return m_wpOwner.lock(); }


private:

	// デバッグスフィア・クリック選択用の当たり判定の半径
	static constexpr float kRadius = 0.5f;

	int m_id = -1;

	// Stageからのローカル位置
	Math::Vector3 m_localPos = Math::Vector3::Zero;

	// ポイントライトの色と範囲
	Math::Vector3 m_color = { 3.0f,3.0f,3.0f };
	float m_range = 3.0f;

	// 持ち主となるステージのポインタ
	std::weak_ptr<StageObject> m_wpOwner;

	// 持ち主を設定したか(設定後にステージが消えたことを見分けるため)
	bool m_hasOwner = false;

};
