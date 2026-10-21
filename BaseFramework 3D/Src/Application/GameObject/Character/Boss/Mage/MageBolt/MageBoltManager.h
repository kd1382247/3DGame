#pragma once

#include"MageBoltDesc.h"

class MageBolt;

class CharacterBase;

class MageBoltManager
{
public:

	// 弾オブジェクトを生成する(owner:撃ったボス)
	// 生成に失敗した場合はnullptrを返す
	std::shared_ptr<MageBolt> CreateBolt(const std::shared_ptr<CharacterBase>& owner, const MageBoltDesc& desc);

	// 生成済みのものを全て取り消す(ボスが倒れた時など)
	void CancelAll();

private:

	MageBoltManager() {}
	~MageBoltManager() {}

public:

	static MageBoltManager& Instance()
	{
		static MageBoltManager instance;
		return instance;
	}

};
