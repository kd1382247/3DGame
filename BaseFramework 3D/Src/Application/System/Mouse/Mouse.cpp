#include "Mouse.h"

#include"../../main.h"

POINT Mouse::Get2DMousePos()
{
	POINT mouse;

	//ディスプレイ上のマウス座標を取得(PC画面左上(0,0))
	GetCursorPos(&mouse);

	//指定のウィンドウ基準のマウス座標に変換(実行画面の左上(0,0))
	ScreenToClient(Application::Instance().GetWindowHandle(), &mouse);

	mouse.x -= ScreenWidth / 2;
	mouse.y -= ScreenHeight / 2;
	mouse.y *= -1;

	return mouse;
}
