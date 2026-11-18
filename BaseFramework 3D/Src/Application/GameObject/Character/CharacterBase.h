#pragma once

#include"../../../Framework/GameObject/Component/ParentLink.h"

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
	void PreDraw()      override;
	void PreUpdate()    override;
	void Update()		override;
	void PostUpdate()	override;

	void DrawDebug()    override;

	void DrawLit()		override;
	void GenerateDepthMapFromLight()	override;

	// 固定表示(スクロールしない)側:名前・位置・回転・大きさ
	void DrawInspectorHeader()override;

	// スクロール側:キャラ共通のパラメータ(当たり判定など)
	void DrawInspector()override;

	//===========================================================
	// Transform
	//===========================================================

	void SetRotation(const Math::Vector3& rotation)override;
	void  UpdateMatrix();

	//===========================================================
	// SphereInfo
	//===========================================================
	
	// スフィア情報
	DirectX::BoundingSphere GetBumpSphere()const;
	DirectX::BoundingSphere GetBumpSphereAt(const Math::Vector3& pos)const;

	// 当たり判定(押し戻し)球の半径(キャラのScaleを掛けた、ワールド空間での大きさ)
	float GetBumpSphereRadius()const;

	// 当たり判定(押し戻し)球の、足元からの高さ(キャラのScaleを掛けた、ワールド空間での高さ)
	float GetBumpSphereOffsetY()const;

	// Scaleを掛ける前の、パラメータの値(Editorで編集する値)
	float GetBumpSphereBaseRadius()const  { return m_bumpSphereRadius; }
	float GetBumpSphereBaseOffsetY()const { return m_bumpSphereOffsetY; }

	// キャラの大きさの倍率(Scaleの最大の成分)。半径や距離などの「長さ」に掛ける
	float GetSizeScale()const;

	// 胴体の中心(足元から一定の高さ × Scale.y)。攻撃判定やエフェクトの基準点
	Math::Vector3 GetBodyCenter()const;

	// 当たり判定(押し戻し)球の大きさをセットする(各キャラのInitで、パラメータの値を渡す)
	// 渡す値はScaleを掛ける前の値。値が変わった時は、KdColliderの形状も作り直す
	void SetBumpSphere(float offsetY, float radius);

	//===========================================================
	// Collision
	//===========================================================

	// 現在の押し戻しの影響を受ける割合
	float GetBumpPushRate()const                 { return m_bumpPushRate; }
	// 押し戻しの量を加算
	void  AddPush(const Math::Vector3& push)     { m_totalPush += push; }

	void  SetTotalPush(const Math::Vector3& push){ m_totalPush = push; }
	const Math::Vector3& GetTotalPush()const     { return m_totalPush; }

	// 押し戻し量を適用する
	void ApplyPush()                             { SetPos(GetPos() + m_totalPush); }
	void ClearPush()                             { m_totalPush = Math::Vector3::Zero; }

	float GetMaxWalkableSlopeAngle()const { return m_maxWalkableSlopeAngle; }

	// 被弾時の処理。各キャラクターが必要に応じてoverrideする
	virtual void  OnHit(const AttackInfo attackInfo) {}

	//===========================================================
	// Area
	//===========================================================

	int  GetAreaID()const { return m_currentAreaID; };
	// エリアIDを更新
	void UpdateCurrentAreaID();
	// 現在のエリアIDに対応するデバッグ表示色
	Math::Color GetAreaDebugColor()const;
	

	//===========================================================
    // Parameter
    //===========================================================

	// ノックバック
	Math::Vector3 GetKnockBack()const                         { return m_knockBack; }
	void          SetKnockBack(const Math::Vector3& knockBack){ m_knockBack = knockBack; }

	void          AddKnockBack(const Math::Vector3& dir, const float power);


	//===========================================================
	// Parameter
	//===========================================================

	// 移動のパラメータ
	virtual float GetTurnSpeed()const = 0;
	virtual float GetMoveSpeed()const = 0;

	// 重力加速度(既定値。キャラごとに変えたい時はoverrideする)
	virtual float GetGravityAcceleration()const { return 72.0f; }


	//===========================================================
	// Movement
	//===========================================================

	// 移動方向
	void          SetMoveDir(const Math::Vector3& moveDir)    { m_moveDir = moveDir; }
	Math::Vector3 GetMoveDir()                                { return m_moveDir; }

	// キャラの移動量をセット
	void          ClearPendingMove(const Math::Vector3& move) { m_pendingMove = move; }
	Math::Vector3 GetPendingMove()const                       { return m_pendingMove; }

	void          AddPendingMove(const Math::Vector3& move)   { m_pendingMove += move; }

	// 移動前の位置をセット
	void          SetPrevPos(const Math::Vector3& pos)        { m_prevPos = pos; }
	Math::Vector3 GetPrevPos()const                           { return m_prevPos; }

	// 重力を返す
	float         GetGravity()const                           { return m_gravity; }
	void          SetGravity(float gravity)                   { m_gravity = gravity; }

	// 地面に着いているか
	void SetIsGrounded(const bool flg)                        { m_isGrounded = flg; }
	bool IsGrounded()const                                    { return m_isGrounded; }

	// キャラの向く方向を更新
	void  UpdateFacingDirection();


	//===========================================================
	// その他
	//===========================================================

	void  SetDeltaTime(const float deltaTime)                 { m_deltaTime = deltaTime; }
	float GetDeltaTime() const                                { return m_deltaTime; }


	// 現在のアニメーション経過フレーム数(攻撃判定・トレイル表示区間などのフレーム指定に使う)
	float GetAnimFrame()const { return m_animFrame; }


	//===========================================================
	// Visual
	//===========================================================

	void  StartOverlay(const Math::Vector3& color, const float rate, const float duration)
	{
		m_overlayColor = color;
		m_maxOverlayRate = rate;
		m_overlayTime = duration;
	}

	float GetOverlayRate();

	void  UpdateOverlay();

	//===========================================================
	// HP
	//===========================================================

	const Health& GetHealth()const { return m_health; }

	// HPを回復する (上限は最大HP。倒れた後に復活させる用途には使わない)
	void Heal(const int amount) { m_health.Heal(amount); }

private:

	// 解放処理
	void Release();

protected:

	// キャラクターの初期化で共通する処理をまとめる
	// (モデル生成・当たり判定コライダー登録・デバッグワイヤー生成・オブジェクト名設定)
	void InitCharacterModel(const std::string& modelPath, const std::string& colliderName,
		const Math::Vector3& colliderOffset, float colliderRadius, const std::string& objectName);

	// デバッグ用に、当たり判定(押し戻し)球を描画する
	void DrawBumpDebugSphere();

	// m_bumpSphereRadius・m_bumpSphereOffsetYから、KdColliderの形状を作り直す
	// (KdCollider::RegisterCollisionShapeは同じ名前を上書きしないので、コライダーごと作り直す)
	void RebuildBumpCollider();

	// 重力を加算して、落下分の移動量を追加する(毎フレーム、ステートの更新より前に呼ぶ)
	void UpdateGravity();



	//===========================================================
	// モデル
	//===========================================================

	std::shared_ptr<KdModelWork>  m_spModel = nullptr;

	// デルタタイム
	float m_deltaTime = 0.0f;

	//===========================================================
	// Movement
	//===========================================================

	Math::Vector3 m_moveDir = Math::Vector3::Zero;

	// １フレームのキャラの移動量
	Math::Vector3 m_pendingMove = {};

	// 移動前の位置を保存
	Math::Vector3 m_prevPos = {};

	float		  m_gravity = 0;
	bool          m_isGrounded = false;

	// エリアID
	int m_currentAreaID = -1;
	static constexpr float AreaSwitchMargin = 0.2f;

	//===========================================================
	// Collision
	//===========================================================

	// 当たり判定(押し戻し)球の半径(Scaleを掛ける前の値)
	float m_bumpSphereRadius = 0.5f;

	// 当たり判定(押し戻し)球の、足元からの高さ(Scaleを掛ける前の値)
	float m_bumpSphereOffsetY = 0.5f;

	// KdColliderに登録している、当たり判定(押し戻し)球の名前
	std::string m_bumpColliderName;

	// 押し戻り量の割合
	float         m_bumpPushRate = 1.0f;
	// 押し戻し量をためる
	Math::Vector3 m_totalPush = {};

	// キャラが登れる坂の角度
	float m_maxWalkableSlopeAngle = 45;

	// 地面の法線
	Math::Vector3 m_groundNormal = Math::Vector3::Zero;

	//===========================================================
	// Component
	//===========================================================

	Health m_health;

	std::shared_ptr<ParentLink> m_spParentLink = nullptr;

	//===========================================================
	// Combat
	//===========================================================

	// 攻撃判定のタイミング
	float         m_animFrame = 0;
	AttackTiming  m_attackTiming = {};

	// ノックバック
	Math::Vector3 m_knockBack = {};

	AttackInfo    m_attackInfo;

	//===========================================================
	// Visual
	//===========================================================

	// キャラの点滅
	float m_overlayTime          = 0.0f;
	float m_overlayDuration      = 1.0f;
	float m_maxOverlayRate       = 0.0f;
	Math::Vector3 m_overlayColor = {};

	Math::Vector3 m_emissiveColor = {};


};
