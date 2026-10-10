#pragma once

class Room :KdGameObject
{
public:

	enum class RoomType
	{
		Normal,
		Boss
	};

	void Init()override;

	void DrawDebug()override;

	void DrawInspectorHeader()override;
	void DrawInspector()override;

	void SaveData();
	void LoadData();

	DirectX::BoundingBox GetBox()const;

	// 箱の中にいるか
	bool IsInside(const Math::Vector3& pos)const;

	void SetCollisionType(const RoomType type) { m_roomType = type; }
	RoomType GetCollisionType()const { return m_roomType; }

	void SetCollisionType(const std::string typeName);
	std::string GetCollitionTypeName()const { return m_roomTypeName[static_cast<int>(GetCollisionType())]; }


private:

	RoomType m_roomType = RoomType::Normal;


	std::string m_roomTypeName[2] = { {"Normal"},{"Boss"} };


};