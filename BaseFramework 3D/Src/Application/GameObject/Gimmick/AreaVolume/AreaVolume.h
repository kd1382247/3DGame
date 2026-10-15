#pragma once

#include"../StageGimmick.h"

class AreaVolume :public StageGimmick
{
public:

	AreaVolume(){}
	~AreaVolume(){}

	void Init()override;
	void Update()override;

	void DrawDebug()override;

	void DrawInspectorHeader()override;
	void DrawInspector()override;

	DirectX::BoundingBox GetBox()const;


	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json) override;

	int GetAreaID()const { return m_areaID; }

	bool IsInside(const Math::Vector3& pos)const;

	float CalcCoverage(const DirectX::BoundingSphere&sphere)const;

	// エリアIDごとのデバッグ表示色(ボリューム・WayPoint・キャラクターで共通)
	// -1(未確定)は灰色
	static Math::Color GetAreaColor(const int areaID);


protected:

	// AreaVolumeは、Gimmickとは別のデバッグ表示チェックを使う
	bool IsDebugEnabled() const override
	{
		return EditorManager::Instance().IsAreaVolumeDebug();
	}

private:

	// 使われていない最小のエリアIDを返す(WayPointManager::FindAvailableIDと同じ考え方)
	static int FindAvailableAreaID();

	// エリアID。Initで自動的に割り振る(-1は未割り当て)
	int m_areaID = -1;


};