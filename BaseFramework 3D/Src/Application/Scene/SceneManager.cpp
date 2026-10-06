#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "TitleScene/TitleScene.h"
#include "GameScene/GameScene.h"
#include "EditorScene/EditorScene.h"

#include "../System/CollisionManager/CollisionManager.h"
#include "../System/WayPointManager/WayPointManager.h"
#include "../System/TimeManager/TimeManager.h"
#include "../Editor/EditorManager.h"
#include "../../Framework/Effekseer/KdEffekseerManager.h"
#include "../GameObject/Stage/Collision/AABBCollision/AABBCollisionManager.h"
#include "../GameObject/Stage/Collision/OBBCollision/OBBCollisionManager.h"

void SceneManager::PreUpdate()
{
	// 暗転してからシーンを切り替える
	if (m_transition.IsBlackOut())
	{
		// 前のシーンの情報を持ち越さないようにリセットする
		// (新しいシーンを作る前に行う。後だと新シーンが登録した情報まで消えてしまう)
		ResetSystems();

		ChangeScene(m_nextSceneType);

		// シーンが切り替わったらFadeIn
		m_transition.StartFadeIn();
	}

	m_currentScene->PreUpdate();

	m_transition.Update();
}

void SceneManager::Update()
{
	m_currentScene->Update();
}

void SceneManager::PostUpdate()
{
	m_currentScene->PostUpdate();
}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();

	// シーン遷移を描画
	m_transition.Draw();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}

const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _obj)
{
	m_currentScene->AddObject(_obj);
}

bool SceneManager::UseProcess() const
{
	return m_currentScene->UsePostProcess();
}

void SceneManager::ChangeScene(SceneType _sceneType)
{
	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::Title:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::Game:
		m_currentScene = std::make_shared<GameScene>();
		break;
	case SceneType::Editor:
		m_currentScene = std::make_shared<EditorScene>();
		break;
	}

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;
}

void SceneManager::ResetSystems()
{
	// 当たり判定の登録リスト
	CollisionManager::Instance().Clear();

	// ステージの当たり判定(AABB / OBB)
	// ClearXXXList()は退避用リストへ移すだけなので、ClearBackup()も呼んで完全に空にする
	AABBCollisionManager::Instance().ClearAABBCollisionList();
	AABBCollisionManager::Instance().ClearBackup();

	OBBCollisionManager::Instance().ClearOBBCollisionList();
	OBBCollisionManager::Instance().ClearBackup();

	// ウェイポイント(同じく退避用リストも空にする)
	WayPointManager::Instance().ClearWayPoints();
	WayPointManager::Instance().ClearBackup();

	// ヒットストップ・スローモーションの状態
	TimeManager::Instance().Init();

	// 再生中のエフェクトを止める(読み込み済みのエフェクトデータは残す)
	KdEffekseerManager::GetInstance().StopAllEffect();

	// 攻撃範囲表示(カラースフィア)を消す
	KdShaderManager::Instance().ClearColorSphere();

	// 音を全て止める
	KdAudioManager::Instance().StopAllSound();

	// エディタの状態(モード・選択・編集中のステージ)
	EditorManager::Instance().ResetState();
}
