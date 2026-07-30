#pragma once

class KdGameObject;

class ReferenceManager
{
public:

	void AssociateClass();

	// 関連付けを行いたいクラスをリストに追加
	void AddReference(std::shared_ptr<KdGameObject>& obj)
	{
		m_referenceList.push_back(obj);
	}

private:

	// 関連付けを行いたいクラスを格納するリスト
	std::vector<std::shared_ptr<KdGameObject>>m_referenceList;


private: // シングルトンパターン

	ReferenceManager(){}
	~ReferenceManager(){}

public:

	static ReferenceManager& Instance()
	{
		static ReferenceManager instance;
		return instance;

	}
};