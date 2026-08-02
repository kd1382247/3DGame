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


private:
	// 当たり判定更新
	void UpdateCollision();

	void UpdateBumpCollision();
	void UpdateGroundCollision();

	// 解放処理
	void Release();

protected:

	std::shared_ptr<KdModelWork>			    m_spModel = nullptr;

	float										m_Gravity = 0;
	bool                                        m_jumpFlg = false;


};