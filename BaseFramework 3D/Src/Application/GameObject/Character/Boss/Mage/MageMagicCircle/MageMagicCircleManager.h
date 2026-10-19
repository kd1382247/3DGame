#pragma once

#include"MageMagicCircleDesc.h"

class MageMagicCircle;

class MageMagicCircleManager
{
public:

	// 魔法円オブジェクトを生成する(失敗した場合はnullptr)
	std::shared_ptr<MageMagicCircle> CreateMagicCircle(const MageMagicCircleDesc& desc);


	// 生成済みのものを全て取り消す(ボスが倒れた時など)
	void CancelAll();

private:

	MageMagicCircleManager(){}
	~MageMagicCircleManager(){}

public:

	static MageMagicCircleManager& Instance()
	{
		static MageMagicCircleManager instance;
		return instance;
	}

};
