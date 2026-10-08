#pragma once

class StageObject;

class WayPoint : public KdGameObject
{
public:
	WayPoint() { Init(); }
	~WayPoint() override {}

	void Init() override;
	void DrawDebug() override;

	// 固定表示(スクロールしない):位置
	void DrawInspectorHeader() override;

	// スクロール側:エリアID・接続関係
	void DrawInspector() override;

	int GetID() const { return m_id; }
	void SetID(int id);

	int GetAreaID()const { return m_areaID; }
	void SetAreaID(const int areaID) { m_areaID = areaID; }

	// Stageからのローカル位置(Inspectorではこちらを編集する)
	const Math::Vector3& GetLocalPos()const { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos) { m_localPos = pos; }

	// Stage(親)のワールド位置・大きさを受け取り、実際の位置に反映する
	// (WayPointは大きさを持たないため、大きさはオフセット位置のスケーリングにのみ使う)
	void SetStageTransform(const Math::Vector3& stagePos, const Math::Vector3& stageScale);


	// このWayPointから直接移動できる接続先ID
	const std::vector<int>& GetLinks() const
	{
		return m_linkIDs;
	}

	// WayPoint削除
	void Destroy()override;

	bool AddLink(int id);
	bool RemoveLink(int id);
	bool HasLink(int id) const;



	// 当たり判定の持ち主(ステージ)のポインタをセット
	void SetOwner(const std::shared_ptr<StageObject>& owner) { m_wpOwner = owner; }
	std::shared_ptr<StageObject> GetOwner()const { return m_wpOwner.lock(); }

	// 引数のステージが、持ち主として登録したステージと同じか
	bool ShouldFollow(const StageObject* stage)const
	{
		auto spOwner = m_wpOwner.lock();

		if (spOwner &&
			spOwner.get() != stage)
		{
			return false;
		}

		return true;
	}



private:

	void DrawID();
	void SetUpDrawID();


	std::weak_ptr<StageObject>m_wpOwner;

	// エリアID
	int m_areaID = 0;

	// WayPointManager内でWayPointを識別する一意な番号
	int m_id = -1;

	// 直接移動可能なWayPointのID
	std::vector<int> m_linkIDs;

	// Stageからのローカル位置
	Math::Vector3 m_localPos = Math::Vector3::Zero;

	// ウェイポイントのIDを描画する
	unsigned long    m_wayPointID = {};
	static const int maxDigits = 3;
	std::shared_ptr<KdSquarePolygon>m_spPoly[maxDigits] = {};

	int              m_digits[maxDigits] = {}; // 各桁の数値を格納

	int              m_renderDigitCount = {};


	bool             m_digitDrawFlg[maxDigits] = {};

	Math::Vector3    m_localNumberPos[maxDigits] = {};
};
