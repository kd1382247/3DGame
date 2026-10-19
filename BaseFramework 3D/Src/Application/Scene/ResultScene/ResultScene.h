#pragma once

#include"../BaseScene/BaseScene.h"

class ResultUI;

// リザルト画面 (クリアタイムを表示して、リトライ / タイトルを選ぶ)
class ResultScene : public BaseScene
{
public :

	ResultScene()  { Init(); }
	~ResultScene() {}

private :

	void Event() override;
	void Init()  override;

	// リザルトのUI (オブジェクトリストにも入っている。選択結果を聞くために持っておく)
	std::shared_ptr<ResultUI> m_spResultUI = nullptr;

	// 次のシーンへ進むことを、もう頼んだか
	bool m_isNextSceneRequested = false;
};
