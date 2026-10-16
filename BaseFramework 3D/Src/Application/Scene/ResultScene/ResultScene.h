#pragma once

#include"../BaseScene/BaseScene.h"

// リザルト画面(枠組みだけ。表示物はこれから追加する)
class ResultScene : public BaseScene
{
public :

	ResultScene()  { Init(); }
	~ResultScene() {}

private :

	void Event() override;
	void Init()  override;
};
