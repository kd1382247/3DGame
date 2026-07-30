#pragma once

#include"../CameraBase.h"

class EditorCamera:public CameraBase
{
public:

	EditorCamera(){}
	~EditorCamera()override{}

	void Init()override;
	void Update()override;

private:

	bool m_isCameraControl = false;

};
