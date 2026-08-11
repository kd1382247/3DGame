#pragma once

class KdGameObject;

class KdGameObjectFactory
{
public:

	using CreateFunction = std::function <std::shared_ptr<KdGameObject>(void)>;

	struct FactoryEntry
	{
		// オブジェクトを生成する関数
		CreateFunction               createFunc;
		// Hierarchyなどで分類するためのカテゴリ
		KdGameObject::ObjectCategory category;
	};

	const auto& GetCreateFunctions()const
	{
		return m_createFunctions;
	}

	template<class T>
	void Register(const std::string name,KdGameObject::ObjectCategory category)
	{
		RegisterCreateFunction
		(
			name, 
			[]() {return std::make_shared<T>(); },
			category
		);
	}

	std::shared_ptr<KdGameObject> CreateGameObject(const std::string& objName) const;

private:

	void Init();

	void RegisterCreateFunction(const std::string_view str, const CreateFunction func,const KdGameObject::ObjectCategory category);

	// クラス名に対応する生成情報
	// 生成関数とオブジェクトカテゴリを保持する
	std::unordered_map<std::string, FactoryEntry> m_createFunctions;


private:

	KdGameObjectFactory() { Init(); }
	~KdGameObjectFactory() {}

public:

	static KdGameObjectFactory& Instance()
	{
		static KdGameObjectFactory instance;
		return instance;
	}


};