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


	void RegistHitObject(const std::shared_ptr<KdGameObject>& _object)
	{
		m_wpHitObjectList.push_back(_object);
	}

	void SetCamera(const std::shared_ptr<CameraBase>& _camera)
	{
		m_wpCamera = _camera;
	}

private:
	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	// 解放処理
	void Release();

protected:

	std::shared_ptr<KdModelWork>			    m_spModel = nullptr;
	// カメラ
	std::weak_ptr<CameraBase>m_wpCamera;

	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};
	float										m_Gravity = 0;
	bool                                        m_jumpFlg = false;


};