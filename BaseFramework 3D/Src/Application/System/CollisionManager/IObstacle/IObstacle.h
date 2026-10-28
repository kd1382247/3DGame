#pragma once

class CharacterBase;

class IObstacle
{
public:


	IObstacle() = default;
	virtual ~IObstacle() = default;

	
	virtual void GetWorldBounds(
		Math::Vector3& outMin,
		Math::Vector3& outMax)const = 0;

	// 球とのめり込み判定
	virtual bool OverlapSphere(
		const DirectX::BoundingSphere& sphere,
		Math::Vector3& outPush,
		Math::Vector3& outNormal)const = 0;

	// 動く球が最初に当たる時刻と法線
	virtual bool SweepSphere(
		const Math::Vector3& start,
		const Math::Vector3& move,
		float radius,
		float& outToi,
		Math::Vector3& outNormal)const = 0;


	// このキャラに対して有効か
	virtual bool IsActiveFor(const CharacterBase* character)const { return character!=nullptr; }

	virtual bool IsWalkableOnly()const { return false; }

private:



};