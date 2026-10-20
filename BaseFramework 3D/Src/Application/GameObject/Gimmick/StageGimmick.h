#pragma once

#include"../Stage/StageFollower.h"
#include"../../Editor/EditorManager.h"

class StageObject;

// ステージに付随するギミックの親クラス(Room / RoomGate など)
// 持ち主のステージを1つ持ち、そのステージが動いたら一緒に動く
// Hierarchyは持ち主を見て、Active Stageのギミックだけを一覧に出す
class StageGimmick :public KdGameObject
{
public:

	// ステージに追従する(EditModeでもPlayModeでも呼ばれる)
	void PreDraw()override { m_stageFollower.Update(*this); }

	// 持ち主のステージを決める
	void SetOwnerStage(const std::shared_ptr<StageObject>& stage) { m_stageFollower.SetStage(stage); }

	// 持ち主のステージ(いなければnullptr)
	std::shared_ptr<StageObject> GetOwnerStage() { return m_stageFollower.GetStage(); }

	// 持ち主が決まっているか
	bool HasOwnerStage()const { return m_stageFollower.HasStage(); }

	// 持ち主のステージの名前(ステージがあるかは見ない)
	const std::string& GetOwnerStageName()const { return m_stageFollower.GetStageName(); }

	// エディタでクリックしたときの選ばれやすさ(大きいほど優先)
	// 重なっているギミックのうち、優先度が高いほうが先に選ばれる
	// (Roomのように大きな箱は低くして、中にあるスポナーなどを選べるようにする)
	virtual int GetSelectPriority() const { return 0; }

protected:

	// デバッグ表示のチェックがONか(継承先で、別のチェックボックスに切り替えられる)
	virtual bool IsDebugEnabled() const
	{
		return EditorManager::Instance().IsGimmickDebug();
	}

	// デバッグの線を出すか(DrawDebugの最初に呼ぶ)
	// PlayMode : Debugチェックに従う(Play開始でステージが作り直されるので、Active Stageは見ない)
	// EditMode : Active Stageのギミックだけが対象。そのうえで、デバッグ表示がONか、選択中のときに出す
	bool ShouldDrawDebug()
	{
		auto& editor = EditorManager::Instance();

		if (editor.IsPlayMode())
		{
			return IsDebugEnabled();
		}

		if (GetOwnerStage() != editor.GetActiveStage())
		{
			return false;
		}

		return IsDebugEnabled() || IsSelected();
	}

	// 追従するステージ(持ち主)。継承先のInspector・Save・Loadから使う
	StageFollower m_stageFollower;
};
