#pragma once

#include"json.hpp"

#include"Component/Component.h"

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
		Gimmick,
		Effect,
		AreaVolume,
		CheckPoint
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

	// 位置 
	virtual void SetPos(const Math::Vector3& pos) { m_mWorld.Translation(pos); }
	virtual Math::Vector3 GetPos() const { return m_mWorld.Translation(); }

	// 大きさ
	void SetScale(float scalar);
	virtual void SetScale(const Math::Vector3& scale);
	virtual Math::Vector3 GetScale() const;

	// 回転
	virtual void SetRotation(const Math::Vector3& rotation);
	 Math::Vector3 GetRotation()const { return m_rotation; }

	 // オブジェクトの回転数(0～3で表す)
	 int GetYawQuarterCount()const;

	 // 行列
	const Math::Matrix& GetMatrix() const { return m_mWorld; }


	// オブジェクトが有効か
	virtual bool IsExpired() const { return m_isExpired; }
	virtual void Destroy() { m_isExpired = true; }


	// 描画するのか

	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

	// ============== 当たり判定 ==============//

	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);


	// オブジェクト名(Hierarchy/Inspectorに表示される名前。"Slime_0"のような自動採番込みの名前)
	const std::string& GetObjectName() const { return m_objectName; }
	void SetObjectName(const std::string& name) { m_objectName = name; }

	// クラス名(KdGameObjectFactoryに登録されている生成用の名前。例:"Slime")
	// SaveData()の"Class"に使う。表示名(m_objectName)とは別で持つ
	const std::string& GetFactoryClassName() const { return m_className; }
	void SetFactoryClassName(const std::string& name) { m_className = name; }


	// クラスの関連付け用の関数
	virtual void SetUpReference() {}

	// Inspectorの上部に固定表示する内容(名前・位置・回転・大きさなど)
	// ここはスクロールせず、Deleteボタンも常に見える位置に固定される
	virtual void DrawInspectorHeader() {}

	// Inspectorのスクロール領域に表示させる内容(パラメータなど項目数が多いもの)
	virtual void DrawInspector() {}

	// オブジェクトが選択されたか
	bool IsSelected() { return m_isSelected; }
	void SetSelected(bool selected) { m_isSelected = selected; }


	// カテゴリをセット
	void SetObjectCategory(const ObjectCategory category) { m_objectCategory = category; }
	const ObjectCategory GetObjectCategory() const{ return m_objectCategory; }


	virtual nlohmann::json SaveData()const;

	// SaveData()で保存した、そのクラス固有の追加データを読み込む
	// (共通項目(Class/Name/Position/Scale/Rotation)はStageDataManager側で読み込み済み。
	//  ここでは各クラスが自分で追加したフィールドだけを読み込む)
	virtual void LoadData(const nlohmann::json&) {}

	void LoadComponentData(const nlohmann::json& json);

	//========== コンポーネント ==========//

	// 部品を追加する
	template<class T>
	std::shared_ptr<T> AddComponent();

	// 部品を取り出す
	template<class T>
	std::shared_ptr<T>GetComponent();

	// 各部品の更新
	void UpdateComponents();
	
	// 各部品のInspector
	void DrawComponentsInspector();

	// コピーができるか 既定値はtrue(コピー可能)
	virtual bool IsCopyable()const { return true; }

private:

	// 部品の配列
	std::vector<std::shared_ptr<Component>> m_components;

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

	// KdGameObjectFactoryに登録されているクラス名
	std::string m_className = {};

	// オブジェクトのカテゴリー
	ObjectCategory m_objectCategory=ObjectCategory::None;

	bool m_isSelected = false;
};

template<class T>
inline std::shared_ptr<T> KdGameObject::AddComponent()
{

	// Componentを受け取ったTが継承しているかを調べる
	// 親(Component) 子(T)
	static_assert(std::is_base_of_v<Component, T>, "Componentを継承した型だけ、指定できます");


	// テンプレート型で実物を作る
	auto component = std::make_shared<T>();

	// 配列に追加
	m_components.push_back(component);

	return component;
}

template<class T>
inline std::shared_ptr<T> KdGameObject::GetComponent()
{
	// 配列からキャストできる部品を探す
	for (const auto& component : m_components)
	{
		// キャストに成功したら返す
		if (auto typedComponent = std::dynamic_pointer_cast<T>(component))
		{
			return typedComponent;
		}
	}

	// 見つからなかったら
	return nullptr;
}
