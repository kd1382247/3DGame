#pragma once

#include"../../Scene/SceneManager.h"

class GameObjectFinder
{
public:

	// 指定のオブジェクトを返す
	template<class T>
	std::shared_ptr<T> FindObject()
	{
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (!obj)
			{
				continue;
			}

			auto target =
				std::dynamic_pointer_cast<T>(obj);

			if (target)
			{
				return target;
			}

		}
		return nullptr;
	}
	


	// 複数の同じ型のオブジェクトを返す
	template<class T>
	std::vector<std::shared_ptr<T>> FindObjects()
	{
		std::vector<std::shared_ptr<T>> result;

		for (const auto& obj : SceneManager::Instance().GetObjList())
		{
			if (!obj)
			{
				continue;
			}

			auto target =
				std::dynamic_pointer_cast<T>(obj);

			if (target)
			{
				result.push_back(target);
			}
		}

		return result;
	}

private: // シングルトンパターン

	GameObjectFinder() {}
	~GameObjectFinder() {}

public:

	static GameObjectFinder& Instance()
	{
		static GameObjectFinder instance;
		return instance;
	}
};