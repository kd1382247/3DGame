#pragma once

// BGMを切り替えるクラス
// ・毎フレームUpdate()で「今流したいBGM」を決め、今のBGMと違えばフェードアウトしてから切り替える
// ・タイトル・リザルトなど、画面が決めるBGMは SetOverride() で上書きする(最優先)
// ・上書きがなければ、部屋の状態(RoomManager)で Field / Battle / Boss を決める
class BGMManager
{
public:

	enum class BGMType
	{
		None,
		Title,
		Field,
		Battle,
		Boss,
		Result,
		GameOver,
		GameClear
	};

	// 毎フレーム呼ぶ(Application::Update)
	void Update();

	// 鳴らしているBGMを止めて、覚えている状態と上書きを消す
	// (シーンが切り替わるとき・Playが終わるときに呼ぶ)
	void Reset();

	// 状況に関係なく、このBGMを最優先で流す(タイトル・リザルトなど、画面が決めるBGM用)
	// Noneを渡すと上書きを解除する
	void SetOverride(const BGMType type) { m_overrideType = type; }
	void ClearOverride() { m_overrideType = BGMType::None; }

	// 今鳴らしているBGMの種類(何も鳴っていなければNone)
	BGMType GetCurrentType() const { return m_currentType; }

private:

	// 今流したいBGMを決める
	BGMType DecideTargetType() const;

	// BGMを鳴らし始める(Noneなら何もしない)
	void PlayType(const BGMType type);

	// 今のBGMを止めて、覚えている状態を空にする
	void StopCurrent();

	// フェード率と設定の音量を合わせて、鳴っているBGMの音量に反映する
	void ApplyFadeVolume();

	// BGMのファイルパス(Noneは空文字)
	std::string GetPath(const BGMType type) const;

	// ループ再生するか(ジングル系はしない)
	bool IsLoop(const BGMType type) const;

private:

	static constexpr const char* kBGMFilePath = "Asset/Data/Sound/BGM/";

	// フェードアウトにかける時間(秒)
	static constexpr float kFadeOutTime = 0.5f;

	// 鳴らしているBGM(戻り値を覚えておく。音量のフェードに使う)
	std::shared_ptr<KdSoundInstance> m_spSoundInst = nullptr;

	// 今鳴らしているBGMの種類
	BGMType m_currentType = BGMType::None;

	// 上書き中のBGM(None以外なら、状況より優先して流す)
	BGMType m_overrideType = BGMType::None;

	// フェードアウト中か
	bool  m_isFadingOut = false;

	// フェード率(1:通常の音量 → 0:無音)
	float m_fadeRate = 1.0f;

private:

	BGMManager() {}
	~BGMManager() {}

public:

	static BGMManager& Instance()
	{
		static BGMManager instance;
		return instance;
	}

};
