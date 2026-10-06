#include "KdAudio.h"

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// 
// KdAudioManager
// 
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 初期化
// ・DirectXAudioEngineの初期化 
// ・3Dリスナーの設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::Init()
{
	// AudioEngine初期化
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_ReverbUseFilters;

	m_audioEng = std::make_unique<DirectX::AudioEngine>(eflags);
	m_audioEng->SetReverb(DirectX::Reverb_Default);

	m_listener.OrientFront = { 0, 0, 1 };
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 更新
// ・DirectXAudioEngineの更新
// ・プレイリストから不要なサウンドインスタンスを削除
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::Update()
{
	// 実はこれを実行しなくても音はなる:定期的に実行する必要はある
	if (m_audioEng != nullptr)
	{
		m_audioEng->Update();
	}

	// ストップさせたインスタンスは終了したと判断してリストから削除
	for (auto iter = m_playList.begin(); iter != m_playList.end();)
	{
		if (iter->second->IsStopped())
		{
			iter = m_playList.erase(iter);

			continue;
		}

		++iter;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// リスナーの座標と正面方向の設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::SetListnerMatrix(const Math::Matrix& mWorld)
{
	// 座標
	m_listener.SetPosition(mWorld.Translation());

	// 正面方向
	m_listener.OrientFront = mWorld.Backward();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 2Dサウンドの再生
// ・サウンドアセットの取得orロード
// ・再生用インスタンスの生成
// ・管理用プレイリストへの追加
// ・戻り値で再生インスタンスを取得可能（音量・ピッチなどを変更する場合に必要
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
std::shared_ptr<KdSoundInstance> KdAudioManager::Play(std::string_view rName, bool loop)
{
	if (!m_audioEng) { return nullptr; }

	std::shared_ptr<KdSoundEffect> soundData = GetSound(rName);

	if (!soundData) { return nullptr; }

	std::shared_ptr<KdSoundInstance> instance = std::make_shared<KdSoundInstance>(soundData);

	if(!instance->CreateInstance()){ return nullptr; }

	instance->Play(loop);

	AddPlayList(instance);

	return instance;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 3Dサウンドの再生
// ・サウンドアセットの取得orロード
// ・再生用インスタンスの生成、3D座標のセット
// ・管理用プレイリストへの追加
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
std::shared_ptr<KdSoundInstance3D> KdAudioManager::Play3D(std::string_view rName, const Math::Vector3& rPos, bool loop)
{
	if (!m_audioEng) { return nullptr; }

	std::shared_ptr<KdSoundEffect> soundData = GetSound(rName);

	if (!soundData) { return nullptr; }

	std::shared_ptr<KdSoundInstance3D> instance = std::make_shared<KdSoundInstance3D>(soundData, m_listener);

	if (!instance->CreateInstance()) { return nullptr; }

	instance->Play(loop);

	instance->SetPos(rPos);

	AddPlayList(instance);

	return instance;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BGMの再生
// ・鳴っているBGMがあれば止める
// ・BGM用の音量で再生し、再生中のBGMとして覚えておく
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
std::shared_ptr<KdSoundInstance> KdAudioManager::PlayBGM(std::string_view rName, bool loop)
{
	StopBGM();

	m_bgmInstance = PlayWithVolume(rName, loop, m_bgmVolume);

	return m_bgmInstance;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// SEの再生
// ・SE用の音量で再生する (SEは短く何度も鳴るので、インスタンスは覚えない)
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
std::shared_ptr<KdSoundInstance> KdAudioManager::PlaySE(std::string_view rName, bool loop)
{
	return PlayWithVolume(rName, loop, m_seVolume);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BGMの停止
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::StopBGM()
{
	if (!m_bgmInstance) { return; }

	m_bgmInstance->Stop();
	m_bgmInstance = nullptr;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// BGM音量の設定
// ・0.0〜1.0に丸める
// ・再生中のBGMがあれば、その音量もすぐに変更する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::SetBGMVolume(float vol)
{
	m_bgmVolume = std::clamp(vol, 0.0f, 1.0f);

	if (m_bgmInstance)
	{
		m_bgmInstance->SetVolume(m_bgmVolume);
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// SE音量の設定
// ・0.0〜1.0に丸める
// ・次に鳴らすSEから反映される
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::SetSEVolume(float vol)
{
	m_seVolume = std::clamp(vol, 0.0f, 1.0f);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 再生用インスタンスを作り、音量を指定して2Dサウンドを再生する
// ・Play()と違い、再生前に音量を設定する(一瞬だけ大音量で鳴るのを防ぐ)
// ・念のため再生後にも設定する(ライブラリ側の挙動に依存しないため)
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
std::shared_ptr<KdSoundInstance> KdAudioManager::PlayWithVolume(std::string_view rName, bool loop, float vol)
{
	if (!m_audioEng) { return nullptr; }

	std::shared_ptr<KdSoundEffect> soundData = GetSound(rName);

	if (!soundData) { return nullptr; }

	std::shared_ptr<KdSoundInstance> instance = std::make_shared<KdSoundInstance>(soundData);

	if (!instance->CreateInstance()) { return nullptr; }

	instance->SetVolume(vol);

	instance->Play(loop);

	instance->SetVolume(vol);

	AddPlayList(instance);

	return instance;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 再生リストの全ての音を停止する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::StopAllSound()
{
	auto it = m_playList.begin();
	while (it != m_playList.end()) {
		(*it).second->Stop();
		++it;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 再生リストの全ての音を一時停止する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::PauseAllSound()
{
	auto it = m_playList.begin();
	while (it != m_playList.end()) {
		(*it).second->Pause();
		++it;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 再生リストの全ての音を再開する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::ResumeAllSound()
{
	auto it = m_playList.begin();
	while (it != m_playList.end()) {
		(*it).second->Resume();
		++it;
	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 再生中のサウンドの停止・サウンドアセットの解放
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::SoundReset()
{
	StopAllSound();

	m_soundMap.clear();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// サウンドアセットの読み込み
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::LoadSoundAssets(std::initializer_list<std::string_view>& fileNames)
{
	for (std::string_view fileName : fileNames)
	{
		auto itFound = m_soundMap.find(fileName.data());

		// ロード済みならスキップ
		if (itFound != m_soundMap.end()) { continue; }

		// 生成 & 読み込み
		auto newSound = std::make_shared<KdSoundEffect>();
		if (!newSound->Load(fileName, m_audioEng))
		{
			// 読み込み失敗時
			assert(0 && "LoadSoundAssets:ファイル名のデータが存在しません。名前を確認してください。");

			continue;
		}

		// リスト(map)に登録
		m_soundMap.emplace(fileName, newSound);

	}
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 解放
// ・再生中のプレイリストの解放
// ・サウンドアセットの解放
// ・エンジンの解放
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdAudioManager::Release()
{
	StopAllSound();

	// BGMのインスタンスはエンジンより先に解放する
	m_bgmInstance = nullptr;

	m_playList.clear();

	m_soundMap.clear();

	m_audioEng = nullptr;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// サウンドアセットの取得
// ・
// ・サウンドアセットの解放
// ・エンジンの解放
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
std::shared_ptr<KdSoundEffect> KdAudioManager::GetSound(std::string_view fileName)
{
	// filenameのサウンドアセットがロード済みか？
	auto itFound = m_soundMap.find(fileName.data());

	// ロード済み
	if (itFound != m_soundMap.end())
	{
		return (*itFound).second;
	}
	else
	{
		// 生成 & 読み込み
		auto newSound = std::make_shared<KdSoundEffect>();
		if (!newSound->Load(fileName, m_audioEng))
		{
			// 読み込み失敗時は、nullを返す
			return nullptr;
		}
		// リスト(map)に登録
		m_soundMap.emplace(fileName, newSound);

		// リソースを返す
		return newSound;
	}
}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// 
// KdSoundEffect
// 
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// 音データの読み込み
bool KdSoundEffect::Load(std::string_view fileName, const std::unique_ptr<DirectX::AudioEngine>& engine)
{
	if (engine.get() != nullptr)
	{
		try
		{
			// wstringに変換
			std::wstring wFilename = sjis_to_wide(fileName.data());

			// 読み込み
			m_soundEffect = std::make_unique<DirectX::SoundEffect>(engine.get(), wFilename.c_str());
		}
		catch (...)
		{
			assert(0 && "Sound File Load Error");

			return false;
		}
	}

	return true;
}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// 
// KdSoundInstance
// 
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

KdSoundInstance::KdSoundInstance(const std::shared_ptr<KdSoundEffect>& soundEffect)
	:m_soundData(soundEffect){}

bool KdSoundInstance::CreateInstance()
{
	if (!m_soundData) { return false; }

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default;

	m_instance = (m_soundData->CreateInstance(flags));

	return true;
}

void KdSoundInstance::Play(bool loop)
{
	if (!m_instance) { return; }

	// 再生状態がどんな状況か分からないので一度停止してから
	Stop();

	m_instance->Play(loop);
}

void KdSoundInstance::SetVolume(float vol)
{
	if (m_instance == nullptr) { return; }

	m_instance->SetVolume(vol);
}

void KdSoundInstance::SetPitch(float pitch)
{
	if (m_instance == nullptr) { return; }

	m_instance->SetPitch(pitch);
}

bool KdSoundInstance::IsPlaying()
{
	if (!m_instance) { return false; }

	return (m_instance->GetState() == DirectX::SoundState::PLAYING);
}

bool KdSoundInstance::IsPause()
{
	if (!m_instance) { return false; }

	return (m_instance->GetState() == DirectX::SoundState::PAUSED);
}

bool KdSoundInstance::IsStopped()
{
	if (!m_instance) { return false; }

	return (m_instance->GetState() == DirectX::SoundState::STOPPED);
}



// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 
// KdSoundInstance3D
// 
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

KdSoundInstance3D::KdSoundInstance3D(const std::shared_ptr<KdSoundEffect>& soundEffect, const DirectX::AudioListener& ownerListener)
	:KdSoundInstance(soundEffect), m_ownerListener(ownerListener){}

bool KdSoundInstance3D::CreateInstance()
{
	if (!m_soundData) { return false; }

	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Default |
		DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters;

	m_instance = (m_soundData->CreateInstance(flags));

	return true;
}

void KdSoundInstance3D::Play(bool loop)
{
	if (!m_instance) { return; }

	KdSoundInstance::Play(loop);
}

void KdSoundInstance3D::SetPos(const Math::Vector3& rPos)
{
	if (!m_instance) { return; }

	m_emitter.SetPosition(rPos);

	m_instance->Apply3D(m_ownerListener, m_emitter, false);
}

void KdSoundInstance3D::SetCurveDistanceScaler(float val)
{
	if (!m_instance) { return; }

	m_emitter.CurveDistanceScaler = val;

	m_instance->Apply3D(m_ownerListener, m_emitter, false);
}
