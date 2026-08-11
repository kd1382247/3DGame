#pragma once

class KdGameObject;

class KdGameObjectFactory
{
public:

	using CreateFunction = std::function <std::shared_ptr<KdGameObject>(void)>;

	struct FactoryEntry
	{
		CreateFunction               createFunc;
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

	template<class T>
	std::shared_ptr<T> CreateGameObject()
	{
		std::shared_ptr<T> spObj = std::make_shared<T>();

		spObj->Init();

		m_objects.push_back(spObj);

		return spObj;
	}

	std::shared_ptr<KdGameObject> CreateGameObject(const std::string objName) const;

	const std::list<std::shared_ptr<KdGameObject>>& GetObjects() { return m_objects; }

	void RemoveExpiredObjects();

private:

	void Init();

	void RegisterCreateFunction(const std::string_view str, const CreateFunction func,const KdGameObject::ObjectCategory category);

	// GameObjectのインスタンスリスト
	std::list<std::shared_ptr<KdGameObject>> m_objects;

	// GameObjectの生成関数
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