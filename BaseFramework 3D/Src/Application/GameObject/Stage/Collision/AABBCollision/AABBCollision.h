#pragma once

class AABBCollision :public KdGameObject
{

public:
	AABBCollision() {}
	~AABBCollision()override {}

	void Init()override;

	void DrawDebug()override;

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }


	DirectX::BoundingBox GetBox()const;

	// Stageからのローカル位置・大きさ(Inspectorではこちらを編集する)
	const Math::Vector3& GetLocalPos()const { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos) { m_localPos = pos; }

	const Math::Vector3& GetLocalScale()const { return m_localScale; }
	void SetLocalScale(const Math::Vector3& scale) { m_localScale = scale; }

	// Stage(親)のワールド位置・大きさを受け取り、実際の当たり判定の位置・大きさに反映する
	// (AABBは回転を表現できないため、回転は受け取らない)
	void SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale);


	int GetID() { return m_id; }
	void SetID(int id) { m_id = id; }

	void DrawInspector()override;

	void Destroy()override;


private:

	int m_id = -1;

	Math::Vector3 m_localPos = Math::Vector3::Zero;
	Math::Vector3 m_localScale = Math::Vector3::One;
};
