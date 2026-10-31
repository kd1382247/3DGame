#pragma once

#include"../StageGimmick.h"
#include"../../../System/CollisionManager/IObstacle/IObstacle.h"
class Room;

// 部屋の入口・出口に置く柵
// 紐づけた部屋(Room)の状態に合わせて、上下に動いて開閉する
//   閉じる:戦闘前の演出が始まったとき
//   開く  :戦闘が終わって、扉が開く演出が始まったとき
class RoomGate :public StageGimmick,public IObstacle
{
public:

	void Init()override;
	void Update()override;

	void DrawLit()override;
	void DrawDebug()override;

	void DrawInspectorHeader()override;
	void DrawInspector()override;

	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json) override;

	// 柵が通行を邪魔しているか(完全に開いたらfalse)
	bool IsBlocking()const { return m_openRate < 1.0f; }

	// 当たり判定の箱(回転あり)
	DirectX::BoundingOrientedBox GetCollisionBox()const;


	//=====================================================
	// 当たり判定
	//=====================================================

	// ワールドのBoxのMinとMaxを求める
	void GetWorldBounds(
		Math::Vector3& outMin,
		Math::Vector3& outMax)const override;

	// 球とのめり込み判定
	bool OverlapSphere(
		const DirectX::BoundingSphere& sphere,
		Math::Vector3& outPush,
		Math::Vector3& outNormal)const override;

	// 動く球が最初に当たる時刻と法線
	bool SweepSphere(
		const Math::Vector3& start,
		const Math::Vector3& move,
		float radius,
		float& outToi,
		Math::Vector3& outNormal)const override;

	// このキャラに対して有効か
	bool IsActiveFor(const CharacterBase* character)const override{ return IsBlocking(); }



private:

	// 名前から部屋を探す
	std::shared_ptr<Room> FindRoomByName(const std::string& name)const;

	// 紐づけた部屋を返す(無ければnullptr)
	std::shared_ptr<Room> GetLinkedRoom();

	void LoadModel();

	// 今の持ち上がり量(m_openRateに合わせて滑らかに動く)
	float GetLift()const;

	// 当たり判定の箱の中心(Posは柵の足元の中心)
	Math::Vector3 GetBoxCenter()const;

	Math::Quaternion GetRotationQuaternion()const;

private:

	std::shared_ptr<KdModelWork> m_spModel = nullptr;
	std::string m_modelPath = "Asset/Models/Gimmick/RoomGate.gltf";

	// 紐づける部屋のオブジェクト名(保存するのはこの名前)
	std::string m_linkedRoomName;
	std::weak_ptr<Room> m_wpRoom;
	bool m_warnedMissingRoom = false;

	// 開いている割合(0:完全に閉じている 〜 1:完全に開いている)
	float m_openRate = 0.0f;

	// 開いたときに持ち上がる高さ
	float m_openHeight = 3.0f;

	// 開く・閉じるのにかかる時間(秒)
	float m_openDuration = 1.5f;
	float m_closeDuration = 0.6f;

	// Playを始めた最初の1フレームは、動かさずに今の状態へ合わせる
	bool m_needSnap = true;

	// ゲートのフラグを保存
	bool m_shouldClose = false;

	bool m_isPlaySE = false;

};
