#pragma once

#include"../StageGimmick.h"

class Player;

class Room :public StageGimmick
{
public:

	enum class RoomType
	{
		Normal,
		Boss,
		Safe
	};

	// 部屋の状態
	enum class RoomState
	{
		Unvisited,	// 未訪問
		Intro,		// 戦闘前の演出中(ボス部屋に限らず、全部の部屋で通る)
		InCombat,	// 戦闘中
		BossDefeat,	// ボスを倒した演出中(ボス部屋だけ通る)
		Outro,		// クリア後の演出中(扉が開く演出。全部の部屋で通る)
		Cleared		// クリア済み
	};

	void Init()override;
	void Update()override;

	void DrawDebug()override;

	void DrawInspectorHeader()override;
	void DrawInspector()override;

	void SetUpReference()override;

	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json) override;

	DirectX::BoundingBox GetBox()const;

	// 箱の中にいるか
	bool IsInside(const Math::Vector3& pos)const;
	bool IsInsideCore(const Math::Vector3& pos,float bodyRadius)const;

	// 内側の箱。marginの分だけ、X/Zを両側から縮める(体の半径を入れる)
	DirectX::BoundingBox GetCoreBox(float margin)const;

	void SetRoomType(const RoomType type) { m_roomType = type; }
	RoomType GetRoomType()const { return m_roomType; }

	void SetRoomType(const std::string& typeName);
	std::string GetRoomTypeName()const { return m_roomTypeName[static_cast<int>(GetRoomType())]; }

	// 戦闘開始かどうか
	bool IsCombat()const { return m_roomState == RoomState::InCombat; }

	RoomState GetRoomState()const { return m_roomState; }
	bool IsCleared()const { return m_roomState == RoomState::Cleared; }

	// 戦闘前の演出が終わったことを知らせる(演出の仕組みができたら、そこから呼ぶ)
	void FinishIntro();

	// ボスを倒した演出が終わったことを知らせる(ボス撃破の演出から呼ぶ)
	void FinishBossDefeat();

	// 扉が開く演出が終わったことを知らせる(演出の仕組みができたら、そこから呼ぶ)
	void FinishOutro();

	// 柵(RoomGate)を閉じておく状態か。柵が見る用
	// 閉じる:戦闘前の演出中・戦闘中・ボス撃破の演出中 / 開く:未訪問・扉の演出中・クリア済み
	bool ShouldCloseGate()const
	{
		return m_roomState == RoomState::Intro
			|| m_roomState == RoomState::InCombat
			|| m_roomState == RoomState::BossDefeat;
	}


private:

	// 状態を変える(ログも出す)
	void ChangeState(RoomState next);

	// 状態ごとの毎フレームの処理
	void UpdateIntro();
	void UpdateCombat();
	void UpdateBossDefeat();
	void UpdateOutro();

	bool SpawnerFinished();

	// 部屋の箱の中にいる、生きている敵の数を数える
	int CountAliveEnemies()const;

	std::weak_ptr<Player>m_wpPlayer;
	bool m_isPlayerInside = false;

	// 内側の箱(バトル開始の判定範囲)。部屋の中心からのずれと、大きさ(幅・高さ・奥行き)
	// 壁沿いに歩いても必ず通る場所に置く(抜け道ができないようにする)
	Math::Vector3 m_coreOffset = Math::Vector3::Zero;
	Math::Vector3 m_coreSize = { 8.0f,6.0f,8.0f };

	bool  m_isPlayerInCore = false;

	// プレイヤーの半径(Editではプレイヤーがいないので、内側の箱の表示用に初期値を入れておく)
	float m_bodyRadius=0.5f;

	RoomType m_roomType = RoomType::Normal;

	const std::string m_roomTypeName[3] = { {"Normal"},{"Boss"},{"Safe"}};


	RoomState m_roomState = RoomState::Unvisited;

	// 演出の長さ(秒)。演出の仕組みができるまでの仮の待ち時間
	float m_introDuration = 2.0f;		// 戦闘前の演出
	float m_bossDefeatDuration = 3.0f;	// ボスを倒した演出(ボス部屋だけ)
	float m_outroDuration = 2.0f;		// 扉が開く演出

	// 今の状態になってからの経過時間(状態が変わるたびに0に戻る)
	float m_stateTimer = 0.0f;

};