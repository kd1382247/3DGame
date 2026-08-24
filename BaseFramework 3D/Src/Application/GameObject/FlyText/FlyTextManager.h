#pragma once


class FlyTextManager
{
public:

	
	void CreateDamateText(const int damage, const Math::Vector3& pos);

private:

private:

	FlyTextManager() {}
	~FlyTextManager() {}


public:

	static FlyTextManager& Instance()
	{
		static FlyTextManager instance;
		return instance;
	}

};