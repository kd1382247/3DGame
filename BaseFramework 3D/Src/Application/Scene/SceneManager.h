#pragma once

class BaseScene;

#include"SceneTransition/SceneTransition.h"

class SceneManager
{
public:

	// シーン情報
	enum class SceneType
	{
		Title,
		Game,
		Editor
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (暗転しきってから変わる)
	void SetNextScene(SceneType _nextScene)
	{
		if(!m_transition.IsTransitioning())
		{
			m_nextSceneType = _nextScene;

			// シーンが変わるときにFadeOutする
			m_transition.StartFadeOut();
		}
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	bool UseProcess()const;

	// 現在のシーンを取得
	template<class T>
	std::shared_ptr<T>GetCurrentScene()const
	{
		return std::dynamic_pointer_cast<T>(m_currentScene);
	}

private:

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init()
	{
		m_transition.Init();

		// 開始シーンに切り替え
		ChangeScene(m_currentSceneType);
	}

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);

	// シーン切り替え時に、前のシーンの情報を持ち越さないようリセットする
	// (シングルトンのマネージャーが持っているデータを空にする)
	void ResetSystems();

	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::Title;

	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	// シーン遷移
	SceneTransition m_transition;

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
