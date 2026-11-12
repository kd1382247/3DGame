#pragma once

#include"Component.h"

class ParentLink:public Component
{
public:

	enum class FollowMode
	{
		Always,
		PlaceOnce
	};

	void SetFollowMode(FollowMode mode) { m_followMode = mode; }

	void DrawInspector(KdGameObject& obj)override;

	void Update(KdGameObject& obj)override;

	bool Apply(KdGameObject& obj);

	// 置いたフラグを立てる
	void MarkPlaced() { m_isPlaced = true; }

	// 親を決める
	void SetParent(const std::shared_ptr<KdGameObject>& parent);

	// 親を返す
	std::shared_ptr<KdGameObject>GetParent()const;

	// 親を付けるときに、今のワールド位置を保つ
	void SetParentKeepWorld(const std::shared_ptr<KdGameObject>& parent, const KdGameObject& obj );

	// 親が見つかった最初のフレームで、逆算を一回だけ行う
	void SetParentNameKeepWorld(const std::string& parentName);


	// ローカルの座標
	const Math::Vector3& GetLocalPos()const        { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos)     { m_localPos = pos;  }

	// ローカルの大きさ
	const Math::Vector3& GetLocalScale()const      { return m_localScale;  }
	void SetLocalScale(const Math::Vector3& scale) { m_localScale = scale; }

	// ローカルの回転
	const Math::Vector3& GetLocalRotation()const   { return m_localRotation; }
	void SetLocalRotation(const Math::Vector3& rot){ m_localRotation = rot;  }


	void SaveData(nlohmann::json& json)const  override;
	void LoadData(const nlohmann::json& json) override;


	using ParentResolver = std::shared_ptr<KdGameObject>(const std::string&);
	static void SetParentResolver(const std::function<ParentResolver>& findFunk);


	// 持ち主が決まっているか(名前が入っているか。ステージを探せるかは見ない)
	bool HasParent()const { return !m_parentName.empty(); }

	// 持ち主のステージの名前(ステージを探せるかは見ない)
	const std::string& GetParentName()const { return m_parentName; }

	// ワールド位置から、ローカルを逆算する機能
	void CalcLocalFromWorld(const KdGameObject& obj);


	// 大きさを追従させるのか trueで追従
	void SetFollowScale(bool flg) { m_followScale = flg; }

	bool IsFollowRotation()const { return m_followRotation; }

	// 回転を追従させるのか trueで追従
	void SetFollowRotation(bool flg) { m_followRotation = flg; }

	void SetSwapScaleOnOddQuarter(bool flg) { m_swapScaleOnOddQuarter = flg; }


private:


	// 親の回転数が奇数かを返す
	bool IsOddQuarter(const KdGameObject& parent)const;

	// 親の名前を表示する
	void DrawInspectorParentName();

	void DrawInspectorLocalPos     (KdGameObject &obj);
	void DrawInspectorLocalScale   (KdGameObject& obj);
	void DrawInspectorLocalRotation(KdGameObject& obj);


	// ローカルからワールドで追従
	void FollowWorldPos     (const KdGameObject& parent,KdGameObject& obj);
	void FollowWorldScale   (const KdGameObject& parent,KdGameObject& obj);
	void FollowWorldRotation(const KdGameObject& parent,KdGameObject& obj);

	// ワールドからローカルを逆算
	void FollowLocalPos     (const KdGameObject& parent,Math::Vector3 pos);
	void FollowLocalScale   (const KdGameObject& parent,Math::Vector3 scale);
	void FollowLocalRotation(const KdGameObject& parent,Math::Vector3 rotation);
	

	static inline std::function<ParentResolver> m_findParentName;

	
	// 親の参照
	mutable std::weak_ptr<KdGameObject>m_wpParent;

	// 親の名前
	std::string                m_parentName = {};

	// ローカルの位置と大きさ
	Math::Vector3              m_localPos      = {};
	Math::Vector3              m_localScale    = Math::Vector3::One;
	Math::Vector3              m_localRotation = {};

	// ワールドからローカルの逆算をしたか
	bool                       m_isCalcLocalWorld = false;

	bool                       m_isPlaced = false;

	FollowMode                 m_followMode = FollowMode::Always;

	// 親ステージの大きさ、回転に追従するのか
	bool                       m_followScale = true;
	bool                       m_followRotation = false;

	// 大きさを入れ替えるのか
	bool                       m_swapScaleOnOddQuarter = false;

};