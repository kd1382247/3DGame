#pragma once

#include"../CharacterBase.h"

class Player;

class WayPoint;

class EnemyBase :public CharacterBase
{
public:


	enum class MoveState
	{
		DirectChase,
		FollowPath
	};

	struct MeleeAttackData
	{
		AttackTiming m_attackTiming    = {};
		float        m_knockBackPower = 0;
		float        m_sphereRadius   = 0;
		float        m_forwardOffset  = 0;
	};

	EnemyBase(){}
	~EnemyBase()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void SetUpReference()override;

	virtual int GetAttackPower()const { return int(); }

	// 飛び出す
	void Launch(const Math::Vector3& dir, float power);

	// 飛び出し中の移動(接地したら飛び出し終了)
	void UpdateLaunch();

	// スポーン方向を作る
	Math::Vector3 CreateSpawnDirection();

	// 追跡/経路移動の共通処理(元は複数のEnemyが同じ内容で個別に持っていたもの)
	// ターゲットに到達するまで追跡し、到達後はノックバック方向を向く
	void UpdateMove();

	// 行動を止めて、待機モーションにする
	// (ゲームオーバー中など、Updateを止めている間、毎フレーム呼ぶ)
	void HoldIdle();

	// ボス撃破などで、強制的に倒す(ダメージ表示・ヒットストップなどの演出なしで、やられ状態にする)
	void ForceKill();

	// ForceKillで倒されたか(スライムの分裂など、倒れた後に増える処理をしないために使う)
	bool IsForceKilled()const { return m_isForceKilled; }

	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	float GetReachDistance()const { return m_reachDistance; }

	void  CreateDeathSmoke();


	// 攻撃系
	void StartAttack();
	void UpdateAttackCollision();
	void EndAttack();

protected:

	std::weak_ptr<Player>m_wpPlayer;

	void PlayHitEffect();

	// 被弾時の共通処理(HPを減らす・ダメージ表示・点滅・ヒットストップ・ヒットエフェクト・ノックバック)
	// 戻り値: この攻撃で死亡したかどうか
	bool ApplyDamage(const AttackInfo& attackInfo);

	// プレイヤーへ攻撃を当てる(自分→プレイヤーの水平方向へノックバックさせる)
	void AttackPlayer(const std::shared_ptr<Player>& spPlayer, float knockBackPower, float damage);

	// 追跡移動
	void UpdateDirectChase();
	// ウェイポイント移動
	void UpdateFollowPath();

	// 追跡か経路探索移動かを判定する
	bool CanDirectChase();

	// プレイヤーまで視線が通っているか(距離で絞り、一定間隔でだけレイを撃つ。結果は保持する)
	bool IsSightClear(const Math::Vector3& playerPos);

	// fromからtoの間にAIBlockがあるか
	bool IsBlockedByAIBlock(const Math::Vector3& from, const Math::Vector3& to) const;

	void SelectStartWayPoint();

	// 移動方法を変更
	void ChangeMoveState(const MoveState nextState);

	// 新しくウェイポイント情報を更新
	void UpdateFlowGoal();

	// 攻撃のクールダウン更新(ターゲットに到達したら攻撃フラグを立て、クールタイムを減らす)
	void UpdateAttack();

	// 近接攻撃の当たり判定。m_attackTimingの範囲内でキャラ前方に球を出し、当たったらノックバック・ダメージを与える
	// 戻り値: プレイヤーに命中したかどうか
	bool UpdateMeleeAttackCollision(float knockBackPower, float damage = 10.0f, float sphereRadius = 0.6f, float forwardOffset = 0.8f);

	// UpdateMoveから呼ばれる。歩き/待機モーションの再生は各Enemyが実装する
	virtual void PlayWalkAnimation() {}
	virtual void PlayIdleAnimation() {}

	// パラメータクラスのインスペクター描画。各Enemyが実装する
	virtual void DrawParameterInspector() = 0;

	// 移動状態
	MoveState m_currentMoveState = MoveState::DirectChase;
	MoveState m_nextMoveState = MoveState::DirectChase;

	// 次に進むWayPointIDを1つだけ持つ
	int m_targetWayPointID = -1;

	// ウェイポイントの経路の更新時間
	float       m_pathRefreshTimer = 0.0f;
	const float PathRefreshDuration = 1.0f;

	// プレイヤーとの間にAIBlock(視線を遮るブロック)があるかのレイ判定
	// Areaが違う敵が、壁越しにまっすぐ追わず、ウェイポイントを使うかを決めるために使う
	float       m_sightCheckTimer = 0.0f;   // 次の判定までの時間
	bool        m_isSightBlocked  = true;   // 直前の判定結果(trueなら遮られている)
	const float SightCheckInterval = 0.2f;  // 判定の間隔(秒)
	const float SightCheckDistance = 8.0f;  // この距離以内のときだけ判定する
	const float SightRayHeight     = 0.5f;  // レイの高さ(足元からの高さ)



	// プレイヤーに到達したかどうか
	bool m_hasReachedTarget = false;

	// 攻撃のクールダウン
	float m_attackCooldown = 0;

	// クールタイムの量
	float m_attackCooldownDuration = 0;

	bool m_attackFlg = false;

	bool m_hitTarget = false;

	// ForceKillで倒されたか
	bool m_isForceKilled = false;

	// 飛び出す方向と勢い
	Math::Vector3 m_launchVec;

	bool          m_launchFlg = false;

	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	float m_reachDistance = 1.5f;

	float m_smokeEffectScale = 0.6f;

	// エフェクトの距離
	float m_effectDist = 0.6f;

	MeleeAttackData m_meleeAttack = {};

	// フライテキストのパス
	const std::string m_flyTextPath = "DamageNumber_Orange.png";

};
