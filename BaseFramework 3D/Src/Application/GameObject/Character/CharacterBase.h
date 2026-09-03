#pragma once

#include"AttackInfo.h"

class CharacterBase : public KdGameObject
{
public:

	struct AttackTiming
	{
		float hitStart=0.0f;
		float hitEnd=0.0f;
	};

	CharacterBase();
	~CharacterBase()	override;

	void Init()			override;
	void PreUpdate()    override;
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

	void SetIsGrounded(const bool flg) { m_isGrounded = flg; }
	bool IsGrounded()const { return m_isGrounded; }


	Math::Vector3 GetKnockBack()const { return m_knockBack; }
	void SetKnockBack(const Math::Vector3& knockBack) { m_knockBack = knockBack; }

	void AddKnockBack(const Math::Vector3& dir, const float power)
	{
		if (m_knockBack.Length() >= 0.6)
		{
			return;
		}
		m_knockBack += dir * power;
	}

	int  GetCurrentHP()const { return m_hp; }
	virtual int GetMaxHP()const = 0;

	virtual float GetTurnSpeed()const = 0;

	// キャラの移動量をセット
	void ClearPendingMove(const Math::Vector3& move) { m_pendingMove = move; }
	Math::Vector3 GetPendingMove()const { return m_pendingMove; }

	void AddPendingMove(const Math::Vector3& move) { m_pendingMove += move; }

	// 移動前の位置をセット
	void  SetPrevPos(const Math::Vector3& pos) { m_prevPos = pos; }
	Math::Vector3 GetPrevPos()const { return m_prevPos; }


	bool IsInOutro()const { return m_outroFlg; }

private:

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
	bool                          m_isGrounded = false;

	// 押し戻り量の割合
	float         m_bumpPushRate = 1.0f;
	// 押し戻し量をためる
	Math::Vector3 m_totalPush = {};


	float         m_hp = {};

	float         m_charaAngle = 0;

	// エリアID
	int m_currentAreaID = 0;

	// 攻撃判定のタイミング
	float        m_animFrame = 0;
	AttackTiming m_attackTiming = {};
	
	// ノックバック
	Math::Vector3 m_knockBack = {};

	// キャラの移動量
	Math::Vector3 m_pendingMove = {};

	// 移動前の位置を保存
	Math::Vector3 m_prevPos = {};

	// キャラが死亡したときに行う処理
	bool m_outroFlg = false;

};