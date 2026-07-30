#pragma once

class GameObjectFactory
{
public:

	
	void Init();
	// ファクトリーに生成したいクラスを登録
	void AddClassFactory();
	// オブジェクトを生成
	std::shared_ptr<KdGameObject> CreateObject(const std::string_view className);

	// 関連付け

private:


	KdGameObjectFactory m_gameObjectFactory;


private:// シングルトンパターン

	GameObjectFactory() { Init(); }
	~GameObjectFactory(){}

public:

	static GameObjectFactory& Instance()
	{
		static GameObjectFactory instance;
		return instance;
	}


};