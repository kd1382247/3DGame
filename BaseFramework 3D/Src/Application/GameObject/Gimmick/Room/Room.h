#pragma once

#include"../StageGimmick.h"

class Player;

class CameraBase;

class RoomIntroUI;

class Room :public StageGimmick
{
public:

	enum class RoomType
	{
		Wave,		// ウェーブを耐え抜く部屋(入った時に「ウェーブを耐え抜け！」と出る)
		AllKill,	// 敵をすべて倒す部屋(入った時に「敵をすべて倒せ！」と出る)
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

	// 部屋は大きな箱なので、中にあるギミックを選べるように優先度を下げる
	int GetSelectPriority() const override { return -1; }

	// 戦闘開始かどうか
	bool IsCombat()const { return m_roomState == RoomState::InCombat; }
	
	// ボス戦かどうか
	bool IsBossCombat()const { return m_roomState == RoomState::InCombat && m_roomType == RoomType::Boss; }

	// 戦闘前の演出中なのか(ミッションの文字・ボスの登場演出。プレイヤーが操作できない)
	bool IsIntro()const { return m_roomState == RoomState::Intro; }

	// ボスのイントロ中なのか
	bool IsBossIntro()const { return m_roomState == RoomState::Intro && m_roomType == RoomType::Boss; }

	// ボスを倒した演出中なのか
	bool IsBossDefeat()const { return m_roomState == RoomState::BossDefeat && m_roomType == RoomType::Boss; }

	RoomState GetRoomState()const { return m_roomState; }
	bool IsCleared()const { return m_roomState == RoomState::Cleared; }

	// 戦闘前の演出が終わったことを知らせる(演出の仕組みができたら、そこから呼ぶ)
	void FinishIntro();

	// ボスを倒したことを知らせる(ボス撃破の演出を始める。戦闘中のボス部屋だけ受け付ける)
	// 部屋の中の敵は、ボスと同時に全員倒される
	void StartBossDefeat();

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

	// ボスに登場演出の開始を伝える
	bool StartBossIntro();

	// ミッションの文字(「ウェーブを耐え抜け！」など)を出す。出す文字がない部屋(ボス部屋など)ではfalse
	bool StartMissionIntro();

	// 戦闘前の演出中、マウスでのカメラ回転を止める / 再開する
	void SetCameraLock(bool isLock);

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

	// 部屋の箱の中にいる敵を、全員倒す(ボスは含まない)
	void KillAllEnemies();

	std::weak_ptr<Player>m_wpPlayer;
	std::weak_ptr<CameraBase>m_wpCamera;
	bool m_isPlayerInside = false;

	// 内側の箱(バトル開始の判定範囲)。部屋の中心からのずれと、大きさ(幅・高さ・奥行き)
	// 壁沿いに歩いても必ず通る場所に置く(抜け道ができないようにする)
	Math::Vector3 m_coreOffset = Math::Vector3::Zero;
	Math::Vector3 m_coreSize = { 8.0f,6.0f,8.0f };

	bool  m_isPlayerInCore = false;

	// プレイヤーの半径(Editではプレイヤーがいないので、内側の箱の表示用に初期値を入れておく)
	float m_bodyRadius=0.5f;

	RoomType m_roomType = RoomType::Wave;

	const std::string m_roomTypeName[4] = { {"Wave"},{"AllKill"},{"Boss"},{"Safe"}};


	RoomState m_roomState = RoomState::Unvisited;

	// 演出の長さ(秒)。演出の仕組みができるまでの仮の待ち時間
	float m_introDuration = 2.0f;		// 戦闘前の演出
	float m_bossDefeatDuration = 3.0f;	// ボスを倒した演出(ボス部屋だけ)
	float m_outroDuration = 2.0f;		// 扉が開く演出

	// 今の状態になってからの経過時間(状態が変わるたびに0に戻る)
	float m_stateTimer = 0.0f;

	bool  m_waitBossIntro = false;

	// ミッションの文字が消え終わる(戦闘開始)のを待っているか
	bool  m_waitMissionUI = false;
	std::weak_ptr<RoomIntroUI> m_wpMissionUI;

	// ボスの撃破演出が終わる(FinishBossDefeatが呼ばれる)のを待っているか
	bool  m_waitBossDefeat = false;

};