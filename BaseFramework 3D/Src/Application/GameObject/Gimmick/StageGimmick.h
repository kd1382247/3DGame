#pragma once

#include"../Stage/StageOwnedObject/StageOwnedObject.h"
#include"../../Editor/EditorManager.h"

class StageObject;

// ステージに付随するギミックの親クラス(Room / RoomGate など)
// 持ち主のステージを1つ持ち、そのステージが動いたら一緒に動く
// Hierarchyは持ち主を見て、Active Stageのギミックだけを一覧に出す
class StageGimmick :public StageOwnedObject
{
public:

	// エディタでクリックしたときの選ばれやすさ(大きいほど優先)
	// 重なっているギミックのうち、優先度が高いほうが先に選ばれる
	// (Roomのように大きな箱は低くして、中にあるスポナーなどを選べるようにする)
	virtual int GetSelectPriority() const { return 0; }
	
	void DrawInspectorHeader()override;


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

		if (GetParent() != editor.GetActiveStage())
		{
			return false;
		}

		return IsDebugEnabled() || IsSelected();
	}
};
