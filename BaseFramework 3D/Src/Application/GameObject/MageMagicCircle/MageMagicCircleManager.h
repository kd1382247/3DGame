#pragma once

class MageMagicCircle;

class MageMagicCircleManager
{
public:

	// 魔法円オブジェクトを生成する(座標・半径・発動までの予備動作時間(秒)・ダメージ)
	void CreateMagicCircle(const Math::Vector3& pos, float radius, float telegraphTime, float damage);

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
