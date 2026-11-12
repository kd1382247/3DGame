#pragma once

#include"../../../../Framework/GameObject/Component/ParentLink.h"

class StageObject;

class StageOwnedObject:public KdGameObject
{
public:

	StageOwnedObject();


	void PreDraw()override;

	const Math::Vector3& GetLocalPos()const        { return m_spParentLink->GetLocalPos(); }
	void SetLocalPos(const Math::Vector3& pos)
	{
		m_spParentLink->SetLocalPos(pos);
		FollowParent();
	}

	const Math::Vector3& GetLocalScale()const      { return m_spParentLink->GetLocalScale(); }
	void SetLocalScale(const Math::Vector3& scale)
	{
		m_spParentLink->SetLocalScale(scale);
		FollowParent();
	}

	const Math::Vector3& GetLocalRotation()const   { return m_spParentLink->GetLocalRotation(); }
	void SetLocalRotation(const Math::Vector3& rotation)
	{
		m_spParentLink->SetLocalRotation(rotation);
		FollowParent();
	}


	int  GetID()const  { return m_id; }
	void SetID(int id) { m_id = id; }

	// 当たり判定の持ち主(ステージ)のポインタをセット
	std::shared_ptr<StageObject> GetParent()const;
	void SetParent(const std::shared_ptr<StageObject>& owner);

	// 持ち主が決まっているか
	bool HasParent()const { return m_spParentLink->HasParent(); }

	// 持ち主のステージの名前(ステージがあるかは見ない)
	const std::string& GetParentName()const { return m_spParentLink->GetParentName(); }


	// ワールド位置を保ったまま親を付ける
	void SetParentKeepWorld(const std::shared_ptr<StageObject>& stage);

	// オブジェクトがコピーされるときに呼ばれる
	virtual void CopyFrom(const std::shared_ptr<StageOwnedObject>& ownedObj);

	void DrawInspectorHeader()override;

protected:

	void SetSwapScaleOnOddQuarter(bool flg) { m_spParentLink->SetSwapScaleOnOddQuarter(flg); }

	// 回転を追従させるのか
	void SetFollowRotation(bool flg) { m_spParentLink->SetFollowRotation(flg); }

	bool UsesLocalRotation()const { return HasParent() && m_spParentLink->IsFollowRotation(); }

private:


	// Stage(親)のワールド位置・大きさを受け取り、実際の当たり判定の位置・大きさに反映する
	void FollowParent();

	int m_id = -1;

	std::shared_ptr<ParentLink> m_spParentLink = nullptr;

};