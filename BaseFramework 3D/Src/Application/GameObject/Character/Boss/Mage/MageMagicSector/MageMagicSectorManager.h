#pragma once

#include"MageMagicSectorDesc.h"

class MageMagicSector;

class MageMagicSectorManager
{
public:

	// 扇形攻撃オブジェクトを生成する
	std::shared_ptr<MageMagicSector> CreateMagicSector(const MageMagicSectorDesc& desc);


	// 生成済みのものを全て取り消す(ボスが倒れた時など)
	void CancelAll();

private:

	MageMagicSectorManager() {}
	~MageMagicSectorManager() {}

public:

	static MageMagicSectorManager& Instance()
	{
		static MageMagicSectorManager instance;
		return instance;
	}

};
