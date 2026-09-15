#pragma once

class EffectBase;
class CharacterBase;

class EffectManager
{
public:

	void CreateEffect(const std::string& effectName, const std::shared_ptr<KdGameObject>& character,const Math::Vector3& offsetPos);

private:

	EffectManager(){}
	~EffectManager(){}

public:

	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

};