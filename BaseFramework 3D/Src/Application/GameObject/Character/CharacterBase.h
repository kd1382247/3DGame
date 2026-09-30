#pragma once

#include"AttackInfo.h"

#include"Health.h"

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

	// 固定表示(スクロールしない)側:名前・位置・回転・大きさ
	void DrawInspectorHeader()override;

	// スクロール側:キャラ共通のパラメータ(当たり判定など)
	void DrawInspector()override;

	void SetRotation(const Math::Vector3& rotation)override;

	// スフィア情報
	DirectX::BoundingSphere GetBumpSphere()const;

	// 当たり判定(押し戻し)球の半径
	float GetBumpSphereRadius()const { return m_bumpSphereRadius; }

	float GetMaxWalkableSlopeAngle()const { return m_maxWalkableSlopeAngle; }


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

	// 地面に着いているか
	void SetIsGrounded(const bool flg) { m_isGrounded = flg; }
	bool IsGrounded()const { return m_isGrounded; }

	// ノックバック
	Math::Vector3 GetKnockBack()const { return m_knockBack; }
	void SetKnockBack(const Math::Vector3& knockBack) { m_knockBack = knockBack; }

	void AddKnockBack(const Math::Vector3& dir, const float power);

	//int         GetMaxHP()    const { return m_health.GetMaxHP(); }
	//int         GetCurrentHP()const { return m_health.GetCurrentHP(); }

	// 被弾時の処理。各キャラクターが必要に応じてoverrideする
	virtual void OnHit(const AttackInfo attackInfo) {}

	virtual float GetTurnSpeed()const = 0;
	virtual float GetMoveSpeed()const = 0;

	// 重力加速度(既定値。キャラごとに変えたい時はoverrideする)
	virtual float GetGravityAcceleration()const { return 72.0f; }

	// キャラの移動量をセット
	void          ClearPendingMove(const Math::Vector3& move) { m_pendingMove = move; }
	Math::Vector3 GetPendingMove()const { return m_pendingMove; }

	void AddPendingMove(const Math::Vector3& move) { m_pendingMove += move; }

	// 移動前の位置をセット
	void  SetPrevPos(const Math::Vector3& pos) { m_prevPos = pos; }
	Math::Vector3 GetPrevPos()const { return m_prevPos; }

	// デルタタイム
	void SetDeltaTime(const float deltaTime) { m_deltaTime = deltaTime; }
	float GetDeltaTime()       const         { return m_deltaTime; }

	// 移動方向
	void SetMoveDir(const Math::Vector3& moveDir) { m_moveDir = moveDir; }
	Math::Vector3 GetMoveDir() { return m_moveDir; }


	void  UpdateFacingDirection();
	void  UpdateMatrix();

	// 現在のアニメーション経過フレーム数(攻撃判定・トレイル表示区間などのフレーム指定に使う)
	float GetAnimFrame()const { return m_animFrame; }

	void StartOverlay(const Math::Vector3& color, const float rate, const float duration)
	{
		m_overlayColor = color;
		m_maxOverlayRate = rate;
		m_overlayTime = duration;
	}

	float GetOverlayRate();

	void UpdateOverlay();

	const Health& GetHealth()const { return m_health; }

private:

	// 解放処理
	void Release();


protected:

	// キャラクターの初期化で共通する処理をまとめる
	// (モデル生成・当たり判定コライダー登録・デバッグワイヤー生成・オブジェクト名設定)
	void InitCharacterModel(const std::string& modelPath, const std::string& colliderName,
		const Math::Vector3& colliderOffset, float colliderRadius, const std::string& objectName);

	// デバッグ用の当たり判定球を描画する
	void DrawBumpDebugSphere(const Math::Vector3& offset, float radius);

	// 重力を加算して、落下分の移動量を追加する(毎フレーム、ステートの更新より前に呼ぶ)
	void UpdateGravity();


	Math::Vector3                 m_moveDir = Math::Vector3::Zero;

	std::shared_ptr<KdModelWork>  m_spModel = nullptr;

	float		                  m_gravity = 0;
	bool                          m_isGrounded = false;

	// 押し戻り量の割合
	float         m_bumpPushRate = 1.0f;
	// 押し戻し量をためる
	Math::Vector3 m_totalPush = {};



	Health m_health;

	// エリアID
	int m_currentAreaID = 0;

	// 攻撃判定のタイミング
	float        m_animFrame = 0;
	AttackTiming m_attackTiming = {};

	// ノックバック
	Math::Vector3 m_knockBack = {};

	AttackInfo m_attackInfo;

	// キャラの移動量
	Math::Vector3 m_pendingMove = {};

	// 移動前の位置を保存
	Math::Vector3 m_prevPos = {};

	// キャラが登れる坂の角度
	float m_maxWalkableSlopeAngle=45;

	Math::Vector3 m_groundNormal = Math::Vector3::Zero;

	// デルタタイム
	float m_deltaTime = 0.0f;

	// 当たり判定(押し戻し)球の半径
	float m_bumpSphereRadius = 0.5f;

	// キャラの点滅
	float m_overlayTime          = 0.0f;
	float m_overlayDuration      = 1.0f;
	float m_maxOverlayRate       = 0.0f;
	Math::Vector3 m_overlayColor = {};

};
