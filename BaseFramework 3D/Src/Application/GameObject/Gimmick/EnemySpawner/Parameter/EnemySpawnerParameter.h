#pragma once

class EnemySpawnerParameter
{

public:

	struct Parameter
	{
		float m_spawnInterval = 0.0f;
		int   m_spawnCount = 1;
	};

	EnemySpawnerParameter(){}
	~EnemySpawnerParameter(){}

	void Init();

	void DrawInspector();
	
	Parameter GetParameter()const { return m_parameter; }

private:

	void Load();
	void Save();

	Parameter m_parameter = {};
	
};