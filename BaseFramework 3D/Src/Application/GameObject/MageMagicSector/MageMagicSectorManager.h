#pragma once

class MageMagicSector;

class MageMagicSectorManager
{
public:

	// 魔法円オブジェクトを生成する(座標・半径・発動までの予備動作時間(秒)・ダメージ)
	void CreateMagicSector(
		const Math::Vector3& pos,
		const Math::Vector3& dir,
		float angleDeg,
		float radius,
		float telegraphTime,
		float damage,
		const std::string& fileName,
		float size,
		float speed,
		float startFrame,
		float endFrame);

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
