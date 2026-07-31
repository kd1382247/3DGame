#pragma once

class CameraBase;

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

	void SetCamera(const std::shared_ptr<CameraBase>& _camera)
	{
		m_wpCamera = _camera;
	}

private:
	// 当たり判定更新
	void UpdateCollision();

	void UpdateBump();
	void UpdateGroundCollision();

	// 解放処理
	void Release();

protected:

	std::shared_ptr<KdModelWork>			    m_spModel = nullptr;
	// カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	float										m_Gravity = 0;
	bool                                        m_jumpFlg = false;


};