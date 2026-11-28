#pragma once

#include"../CharacterBase.h"

#include"../../Gimmick/Room/RoomLink.h"

class Player;

class WayPoint;

class EnemySpawner;

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
	void PreUpdate()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;
	void DrawInspectorHeader()override;

	void SetUpReference()override;

	void DrawDebug()override;

	virtual int GetAttackPower()const { return int(); }

	//===============Moveing===============//

	// 飛び出す
	void Launch(const Math::Vector3& dir, float height);

	// 飛び出し中の移動(接地したら飛び出し終了)
	void UpdateLaunch();

	// 追跡/経路移動の共通処理
	void UpdateMove();

	// スポーン方向を作る
	Math::Vector3 CreateSpawnDirection();

	// 行動を止めて、待機モーションにする
	// (ゲームオーバー中など、Updateを止めている間、毎フレーム呼ぶ)
	void HoldIdle();

	// ボス撃破などで、強制的に倒す(ダメージ表示・ヒットストップなどの演出なしで、やられ状態にする)
	void ForceKill();

	// ForceKillで倒されたか(スライムの分裂など、倒れた後に増える処理をしないために使う)
	bool IsForceKilled()const { return m_isForceKilled; }

	// この敵を生成したスポナーを覚える(スポナーが生成した直後にセットする)
	void SetSpawner(const std::shared_ptr<EnemySpawner>& spawner) { m_wpSpawner = spawner; }

	// この敵を生成したスポナー(スポナー経由でない敵はnullptr)
	std::shared_ptr<EnemySpawner> GetSpawner()const { return m_wpSpawner.lock(); }

	// 紐づいている部屋(生成した敵・分裂した敵へ引き継ぐ時に使う)
	const RoomLink& GetRoomLink()const { return m_roomLink; }
	void SetRoomLink(const RoomLink& link) { m_roomLink = link; }

	// 自分の生成元のスポナーを、自分から生まれた敵(スライムの分裂など)にも引き継ぐ
	// 子をスポナーの生存リストに登録するので、ウェーブは子が全滅するまで次に進まない
	// (スポナー経由でない敵が呼んでも、何もしない)
	void InheritSpawnerTo(const std::shared_ptr<EnemyBase>& child)const;

	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	// (パラメータの値にScaleを掛けた、ワールド空間での距離)
	float GetReachDistance()const { return m_reachDistance * GetSizeScale(); }

	// 到達後に「離れた」とみなす余白(到達判定の距離にこの分を足した距離より離れたら、また追いかける)
	// (パラメータの値にScaleを掛けた、ワールド空間での距離)
	float GetReachDistanceMargin()const { return m_reachDistanceMargin * GetSizeScale(); }

	void  CreateDeathSmoke();

	// 当たり判定のが有効か
	bool IsCollisionEnabled()const override { return !GetHealth().IsDead() && !m_launchFlg; }


	// 攻撃を始めてよいか(攻撃フラグが立っていて、かつ攻撃前の拡縮を2回終えている)
	// 各Enemyの通常ステートは、これがtrueになったら攻撃ステートへ移る
	bool IsAttackReady()const { return m_attackFlg && m_isScalePulseFinished; }

	// 攻撃系
	void StartAttack();
	void UpdateAttackCollision();
	void EndAttack();

	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json)override;

protected:


	// 紐づいている部屋をクリアしたか
	bool IsRoomCleared();

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
	void CanDirectChase();

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

	// 攻撃の直前に、モデルを大きくして元に戻す動きをkAttackPulseCount回繰り返す
	// (終わったら m_isScalePulseFinished が true になる)
	void UpdateAttackPulse();

	// 拡縮を最初からやり直せる状態に戻す(大きさも元に戻す)
	void ResetAttackPulse();

	// 近接攻撃の当たり判定。m_attackTimingの範囲内でキャラ前方に球を出し、当たったらノックバック・ダメージを与える
	// 戻り値: プレイヤーに命中したかどうか
	bool UpdateMeleeAttackCollision(float knockBackPower, float damage = 10.0f, float sphereRadius = 0.6f, float forwardOffset = 0.8f);

	// UpdateMoveから呼ばれる。歩き/待機モーションの再生は各Enemyが実装する
	virtual void PlayWalkAnimation() {}
	virtual void PlayIdleAnimation() {}

	// パラメータクラスのインスペクター描画。各Enemyが実装する
	virtual void DrawParameterInspector() = 0;


	std::weak_ptr<Player>m_wpPlayer;

	// 紐づいている部屋情報
	RoomLink m_roomLink;

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
	float       m_sightCheckTimer  = 0.0f;  // 次の判定までの時間
	bool        m_isSightBlocked   = true;  // 直前の判定結果(trueなら遮られている)
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

	//---- 攻撃前の拡縮 ----

	// 拡縮の回数(必ずこの回数だけ大きくなって戻ってから、攻撃に入る)
	static constexpr int   kAttackPulseCount = 2;
	// 1回(大きくなって元に戻るまで)にかける時間 [秒]
	static constexpr float kAttackPulseDuration = 0.3f;
	// 一番大きくなった時の、元の大きさに足す倍率(0.25で1.25倍)
	static constexpr float kAttackPulseScaleUp = 0.2f;

	// 拡縮の経過時間 [秒]
	float m_scalePulseTime = 0.0f;

	// 拡縮を、決められた回数分終えたか
	bool  m_isScalePulseFinished = false;

	bool m_hitTarget = false;

	// ForceKillで倒されたか
	bool m_isForceKilled = false;

	// この敵を生成したスポナー(スポナー経由でない敵は空)
	std::weak_ptr<EnemySpawner> m_wpSpawner;

	// 飛び出す方向と勢い
	Math::Vector3 m_launchVec;

	bool          m_launchFlg = false;

	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	float m_reachDistance = 1.5f;

	// 到達後に「離れた」とみなす余白(到達判定の距離にこの分を足した距離より離れたら、また追いかける)
	float m_reachDistanceMargin = 0.5f;

	float m_smokeEffectScale = 0.6f;

	// エフェクトの距離
	float m_effectDist = 0.4f;

	MeleeAttackData m_meleeAttack = {};

	// フライテキストのパス
	const std::string m_flyTextPath = "DamageNumber_Orange.png";

};
