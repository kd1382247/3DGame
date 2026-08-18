#pragma once

#include"json.hpp"

// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};

	// オブジェクトのカテゴリー
	enum class ObjectCategory
	{
		None,
		Character,
		Camera,
		Stage,
		Gimmick
	};

	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init() {}

	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate() {}

	// それぞれの状況で描画する関数
	virtual void GenerateDepthMapFromLight() {}
	virtual void PreDraw() {}
	virtual void DrawLit() {}
	virtual void DrawUnLit() {}
	virtual void DrawEffect() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}
	virtual void DrawDebug();

	virtual void SetAsset(const std::string&) {}

	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 拡大率を変更する関数
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	// 回転角度
	void SetRotation(const Math::Vector3& rotation);
	Math::Vector3 GetRotation()const { return m_rotation; }


	const Math::Matrix& GetMatrix() const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }
	virtual void Destroy() { m_isExpired = true; }


	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);


	// オブジェクト名
	const std::string& GetObjectName() const { return m_objectName; }
	void SetObjectName(const std::string& name) { m_objectName = name; }


	// クラスの関連付け用の関数
	virtual void SetUpReference() {}

	// Inspectorに表示させる
	virtual void DrawInspector() {}

	// オブジェクトが選択されたか
	bool IsSelected() { return m_isSelected; }
	void SetSelected(bool selected) { m_isSelected = selected; }



	// カテゴリをセット
	void SetObjectCategory(const ObjectCategory category) { m_objectCategory = category; }
	const ObjectCategory GetObjectCategory() const{ return m_objectCategory; }


	virtual nlohmann::json SaveData()const;

protected:

	void Release() {}

	// 名前を含めすべてInspecterに表示
	void DrawBasicInspecter();

	// 座標、回転、大きさのInspectorをまとめて表示
	void DrawTransformInspector();

	// Inspectorに表示させる内容
	void DrawNameInspector();
	void DrawPositionInspector();
	void DrawRotationInspector();
	void DrawScaleInspector();



	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld;

	// 回転
	Math::Vector3 m_rotation = {};

	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	// エディターに表示するオブジェクト名
	std::string m_objectName = {};

	// オブジェクトのカテゴリー
	ObjectCategory m_objectCategory=ObjectCategory::None;

	bool m_isSelected = false;
};
