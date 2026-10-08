#pragma once

class StageObject;

class OBBCollision :public KdGameObject
{

public:

	enum class OBBCollisionType
	{
		Solid,
		Walkable
	};

	OBBCollision() {}
	~OBBCollision()override {}

	void Init()override;
	void DrawLit()override {}

	void DrawDebug()override;

	void SetMatrix(const Math::Matrix& mat) { m_mWorld = mat; }

	void SetCollisionType(const OBBCollisionType type) { m_collisionType = type; }
	OBBCollisionType GetCollisionType()const     { return m_collisionType; }

	void SetCollisionType(const std::string typeName);
	std::string GetCollitionTypeName()const { return m_collisionTypeName[static_cast<int>(GetCollisionType())]; }


	DirectX::BoundingOrientedBox GetBox()const;

	// Stageからのローカル位置・大きさ(Inspectorではこちらを編集する)
	// 回転はStageに追従させず、OBB自身の回転をそのまま使う
	const Math::Vector3& GetLocalPos()const { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos) { m_localPos = pos; }

	const Math::Vector3& GetLocalScale()const { return m_localScale; }
	void SetLocalScale(const Math::Vector3& scale) { m_localScale = scale; }

	// Stage(親)のワールド位置・大きさを受け取り、実際の当たり判定の位置・大きさに反映する
	void SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale);


	int GetID() { return m_id; }
	void SetID(int id) { m_id = id; }

	// 位置・大きさ・回転は固定表示(スクロールしない)側に出す
	void DrawInspectorHeader()override;

	void Destroy()override;

	// 当たり判定の持ち主(ステージ)のポインタをセット
	void SetOwner(const std::shared_ptr<StageObject>& owner) { m_wpOwner = owner; }
	std::shared_ptr<StageObject> GetOwner()const { return m_wpOwner.lock(); }

	// 引数のステージが、持ち主として登録したステージと同じか
	bool ShouldFollow(const StageObject* stage)const
	{
		auto spOwner = m_wpOwner.lock();

		if (spOwner &&
			spOwner.get() != stage)
		{
			return false;
		}

		return true;
	}


private:

	int m_id = -1;

	OBBCollisionType m_collisionType = OBBCollisionType::Walkable;

	Math::Vector3 m_localPos = Math::Vector3::Zero;
	Math::Vector3 m_localScale = Math::Vector3::One;


	std::string m_collisionTypeName[2] = { {"Solid"},{"Walkable"} };

	std::weak_ptr<StageObject>m_wpOwner;

};
