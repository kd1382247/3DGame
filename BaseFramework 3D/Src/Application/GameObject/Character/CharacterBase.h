#pragma once

class CharacterBase : public KdGameObject
{
public:
	CharacterBase();
	~CharacterBase()	override;

	void Init()			override;
	void Update()		override;
	void PostUpdate()	override;

	void DrawLit()		override;
	void GenerateDepthMapFromLight()	override;


	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

private:
	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();
	// アニメーション関数
	void UpdateAnimation();


	// 解放処理
	void Release();

protected:
	std::shared_ptr<KdModelWork>			    m_spModel = nullptr;

	std::shared_ptr<KdAnimator>                 m_spAnimator;

	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};
	float										m_Gravity = 0;

};