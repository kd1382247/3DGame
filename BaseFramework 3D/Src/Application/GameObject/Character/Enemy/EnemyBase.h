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

	EnemyBase(){}
	~EnemyBase()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void SetUpReference()override;

	virtual void OnHit(const AttackInfo attackInfo) {}

	// 飛び出す
	void Launch(const Math::Vector3& dir, float power);

	// スポーン方向を作る
	Math::Vector3 CreateSpawnDirection();

	// 追跡/経路移動の共通処理(元は複数のEnemyが同じ内容で個別に持っていたもの)
	// ターゲットに到達するまで追跡し、到達後はノックバック方向を向く
	void UpdateMove();

	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	float GetReachDistance()const { return m_reachDistance; }

protected:

	std::weak_ptr<Player>m_wpPlayer;

	void UpdateGravity();

	// 追跡移動
	void UpdateDirectChase();
	// ウェイポイント移動
	void UpdateFollowPath();

	// 追跡か経路探索移動かを判定する
	bool CanDirectChase();
	void CreatePath();

	// 移動方法を変更
	void ChangeMoveState(const MoveState nextState);

	// 新しくウェイポイント情報を更新
	void UpdatePath();
	void SetPath(const std::vector<int>& path,const int goalID);

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

	// 敵が通るWayPointのID一覧
	std::vector<int>m_path;

	// 現在目指しているWayPointが、m_pathの何番目か
	size_t m_pathIndex = 0;
	int m_goalWayPointID = 0;

	// プレイヤーに到達したかどうか
	bool m_hasReachedTarget = false;

	// 攻撃のクールダウン
	float m_attackCooldown = 0;

	// クールタイムの量
	float m_attackCooldownDuration = 0;

	bool m_attackFlg = false;

	bool m_hitTarget = false;

	// 飛び出す方向と勢い
	Math::Vector3 m_launchVec;

	bool          m_launchFlg = false;

	// 到達判定の距離(この距離まで近づいたら「到達」とみなし、攻撃を開始する)
	float m_reachDistance = 1.5f;

};
