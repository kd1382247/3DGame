#pragma once

class MageBeam;

class MageBeamManager
{
public:

	// ビームオブジェクトを生成する(発射座標・方向・長さ・幅・ダメージ・持続時間(秒))
	// 生成に失敗した場合はnullptrを返す
	std::shared_ptr<MageBeam> CreateBeam(
		const Math::Vector3& pos,
		const Math::Vector3& dir,
		float length,
		float width,
		float damage,
		float duration,
		const std::string& fileName,
		float size,
		float speed,
		float startFrame,
		float endFrame,
		const Math::Vector3& rotation,
		const Math::Vector3& effectPos);

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
