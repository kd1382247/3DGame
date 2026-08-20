#pragma once

class CharacterBase : public KdGameObject
{
public:
	CharacterBase();
	~CharacterBase()	override;

	void Init()			override;
	void Update()		override;
	void PostUpdate()	override;

	void DrawLit()		override;
	void GenerateDepthMapFromLight()	override;

	void DrawInspector()override;


	// スフィア情報
	DirectX::BoundingSphere GetBumpSphere()const;

	// 現在の押し戻しの影響を受ける割合
	float GetBumpPushRate()const { return m_bumpPushRate; }


	// 押し戻しの量を加算
	void AddPush(const Math::Vector3& push) { m_totalPush += push; }

	void SetTotalPush(const Math::Vector3& push) { m_totalPush = push; }
	const Math::Vector3& GetTotalPush()const { return m_totalPush; }

	// 押し戻し量を適用する
	void ApplyPush(){SetPos(GetPos() + m_totalPush);}
	void ClearPush(){ m_totalPush = Math::Vector3::Zero; }

	void SetWallHit(bool hit) { m_wallHit = hit; }

	bool IsWallHit()const { return m_wallHit; }


	// 重力を返す
	float GetGravity()const { return m_gravity; }
	void SetGravity(float gravity) { m_gravity = gravity; }

	void SetJumpFlg(bool flg) { m_jumpFlg = flg; }




private:

	// 当たり判定更新
	void UpdateCollision();

	// 解放処理
	void Release();

	
protected:

	std::shared_ptr<KdModelWork>			    m_spModel = nullptr;

	float										m_gravity = 0;
	bool                                        m_jumpFlg = false;

	
	float m_bumpPushRate = 1.0f;

	Math::Vector3 m_totalPush = {};

	bool m_wallHit = false;

};