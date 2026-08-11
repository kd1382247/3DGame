#pragma once

class KdGameObject;

class ReferenceManager
{
public:

	// ゲームプレイ中に関連付けする
	void AssociateObject(const std::shared_ptr<KdGameObject>& obj);
	void AssociateObjects(const std::list< std::shared_ptr<KdGameObject>>& obj);


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