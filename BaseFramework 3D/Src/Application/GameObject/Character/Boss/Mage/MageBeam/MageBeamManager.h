#pragma once

#include"MageBeamDesc.h"

class MageBeam;


class MageBeamManager
{
public:

	// ビームオブジェクトを生成する
	// 生成に失敗した場合はnullptrを返す
	std::shared_ptr<MageBeam> CreateBeam(const MageBeamDesc& desc);


	// 生成済みのものを全て取り消す(ボスが倒れた時など)
	void CancelAll();

private:

	MageBeamManager() {}
	~MageBeamManager() {}

public:

	static MageBeamManager& Instance()
	{
		static MageBeamManager instance;
		return instance;
	}

};
