#pragma once


#include"AttackInfo.h"

class CharacterBase : public KdGameObject
{
public:


	struct AttackTiming
	{
		float hitStart;
		float hitEnd;
	};

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
	void ApplyPush() { SetPos(GetPos() + m_totalPush); }
	void ClearPush() { m_totalPush = Math::Vector3::Zero; }

	// 現在のエリアIDを返す
	int   GetCurrentAreaID(const Math::Vector3& pos);


	// 重力を返す
	float GetGravity()const { return m_gravity; }
	void SetGravity(float gravity) { m_gravity = gravity; }

	void SetJumpFlg(bool flg) { m_jumpFlg = flg; }

	Math::Vector3 GetKnockBack()const { return m_knockBack; }
	void SetKnockBack(const Math::Vector3& knockBack) { m_knockBack = knockBack; }

	virtual void OnHit(const AttackInfo& attackInfo) {}
	void AddKnockBack(const Math::Vector3& dir, const float power)
	{
		m_knockBack += dir * power;
	}


private:

	// 当たり判定更新
	void UpdateCollision();

	// 解放処理
	void Release();

protected:


	void  UpdateFacingDirection();
	void  UpdateMatrix();

	void SetMoveDir(const Math::Vector3& moveDir) { m_moveDir = moveDir; }
	Math::Vector3 GetMoveDir() { return m_moveDir; }

	Math::Vector3                 m_moveDir = Math::Vector3::Zero;


	std::shared_ptr<KdModelWork>  m_spModel = nullptr;

	float		                  m_gravity = 0;
	bool                          m_jumpFlg = false;

	// 押し戻り量の割合
	float         m_bumpPushRate = 1.0f;
	// 押し戻し量をためる
	Math::Vector3 m_totalPush = {};

	// キャラクターの回転スピード
	float         m_turnSpeed = {};
	// 移動スピード
	float         m_moveSpeed = {};

	float         m_hp = {};

	float         m_angle = 0;

	// エリアID
	int m_currentAreaID = 0;

	// 攻撃判定のタイミング
	float        m_animFrame = 0;
	AttackTiming m_attackTiming = {};
	
	Math::Vector3 m_knockBack = {};

};