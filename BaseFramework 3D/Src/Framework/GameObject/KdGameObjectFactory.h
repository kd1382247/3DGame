#pragma once

class KdGameObject;

class KdGameObjectFactory
{
public:

	const auto& GetCreateFunctions()const
	{
		return m_createFunctions;
	}

	template<class T>
	void Register(const std::string name)
	{
		RegisterCreateFunction
		(
			name, 
			[]() {return std::make_shared<T>(); }
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

	void RegisterCreateFunction(const std::string_view, const std::function <std::shared_ptr<KdGameObject>(void)> func);

	// GameObjectのインスタンスリスト
	std::list<std::shared_ptr<KdGameObject>> m_objects;

	// GameObjectの生成関数
	std::unordered_map<std::string, std::function<std::shared_ptr<KdGameObject>(void)>> m_createFunctions;


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