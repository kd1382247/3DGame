#pragma once

class EnemyBase;

class EnemyHPBarManager
{
public:


	bool CreateHPBar(const std::shared_ptr<EnemyBase>&enemy);

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