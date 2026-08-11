#pragma once

#include"../EnemyBase.h"

#include"Animation/CactasAnimation.h"
#include"State/CactasState.h"
#include"Parameter/CactasParameter.h"

class Cactas :public EnemyBase
{
public:

	Cactas() {}
	~Cactas()override {}

	void Init()override;
	void Update()override;
	void PostUpdate()override;

	void DrawInspector()override;

	void SetPath(const std::vector<int>& path);

private:


	void UpdateDirectChase();

	void UpdateFollowPath();

	void UpdateAnimation();

private:

	CactasActionState m_actionState = CactasActionState::Normal;
	//CactasMoveState   m_moveState = CactasMoveState::Idle;

	// アニメーションクラス
	CactasAnimation   m_animation;

	// パラメータクラス
	CactasParameter   m_parameter;


	// 敵が通るWayPointのID一覧
	std::vector<int>m_path;

	// 現在目指しているWayPointが、m_pathの何番目か
	size_t m_pathIndex = 0;


};