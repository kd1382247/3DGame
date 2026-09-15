#pragma once

class EnemyBase;

class EnemyHPBarManager
{
public:


	bool CreateHPBar(const std::shared_ptr<KdGameObject>&enemy,const Math::Vector3& offsetPos);

private:


private:

	EnemyHPBarManager(){}
	~EnemyHPBarManager(){}

public:

	static EnemyHPBarManager& Instance()
	{
		static EnemyHPBarManager instance;
		return instance;
	}

};