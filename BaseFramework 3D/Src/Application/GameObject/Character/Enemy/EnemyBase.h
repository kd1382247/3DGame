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

	void OnHit(const AttackInfo attackInfo)override;

	// 飛び出す
	void Launch(const Math::Vector3& dir, float power);

	// スポーン方向を作る
	Math::Vector3 CreateSpawnDirection();

	virtual float GetMoveSpeed()const = 0;


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

	// 新しくワイポイント情報を更新
	void UpdatePath();
	void SetPath(const std::vector<int>& path,const int goalID);

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

};