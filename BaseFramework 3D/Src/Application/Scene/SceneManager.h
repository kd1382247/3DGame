#pragma once

class BaseScene;

class SceneManager
{
public:

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	template<class T>
	std::shared_ptr<T> FindObject()
	{
		for (const auto& obj : GetObjList())
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

	template<class T>
	std::vector<std::shared_ptr<T>> FindObjects()
	{
		std::vector<std::shared_ptr<T>> result;

		for (const auto& obj : GetObjList())
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

	template<class T>
	std::shared_ptr<T> FindObject(
		const std::string& objectName)
	{
		for (const auto& obj : GetObjList())
		{
			if (!obj)
			{
				continue;
			}

			if (obj->GetObjectName() != objectName)
			{
				continue;
			}

			auto target =
				std::dynamic_pointer_cast<T>(obj);

			if (target)
			{
				return target;
			}

			return nullptr;
		}

		return nullptr;
	}


private:

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init()
	{
		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
	}

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Game;

	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

private:

	SceneManager() { Init(); }
	~SceneManager() {}

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
