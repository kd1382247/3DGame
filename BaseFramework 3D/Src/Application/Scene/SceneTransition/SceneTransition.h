#pragma once

class SceneTransition
{
public:

	SceneTransition() {}
	~SceneTransition(){}

	void Init();

	void StartFadeOut();
	void StartFadeIn();
	void Update();
	void Draw();

	// 完全に暗転下か
	bool IsBlackOut()const { return m_state == State::BlackOut; }

	// いまフェード中か
	bool IsTransitioning()const { return m_state != State::Idle; }

private:


	enum class State
	{
		Idle,
		BlackOut,
		FadeOut,
		FadeIn
	};


	std::shared_ptr<KdTexture>m_spBlackFadeMask = nullptr;

	// シーン遷移の状態
	State m_state = State::Idle;

	float m_alpha = 0.0f;

	// フェードイン、アウトのスピード
	const float m_fadeSpeed = 0.04f;



};