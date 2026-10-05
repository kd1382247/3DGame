#pragma once

class Mouse
{
public:

	POINT Get2DMousePos();

private:

	const float ScreenWidth = 1280.0f;
	const float ScreenHeight= 720.0f;

private:

	Mouse(){}
	~Mouse(){}


public:

	static Mouse&Instance()
	{
		static Mouse instance;
		return instance;
	}



};