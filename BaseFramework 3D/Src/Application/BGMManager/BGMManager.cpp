#include "BGMManager.h"

#include"../Scene/SceneManager.h"
#include"../Scene/EditorScene/EditorScene.h"
#include"../Editor/EditorManager.h"
#include"../GameObject/Gimmick/Room/RoomManager.h"
#include"../System/TimeManager/TimeManager.h"

void BGMManager::Update()
{
	const BGMType target = DecideTargetType();

	// ヒットストップやスローの影響を受けないように、実時間の経過を使う
	const float deltaTime = TimeManager::Instance().GetUnscaleeDeltaTime();

	// フェードアウト中
	if (m_isFadingOut)
	{
		// 流したいBGMが今のBGMに戻ったら、フェードアウトをやめて音量を戻す
		if (target == m_currentType)
		{
			m_isFadingOut = false;
			m_fadeRate = 1.0f;

			ApplyFadeVolume();

			return;
		}

		m_fadeRate -= deltaTime / kFadeOutTime;

		if (m_fadeRate > 0.0f)
		{
			ApplyFadeVolume();

			return;
		}

		// 無音になったので止めて、下で新しいBGMを鳴らす
		StopCurrent();
	}

	// 今のBGMのままなら何もしない(毎フレームPlayBGMを呼ぶと、頭から鳴り直してしまう)
	if (target == m_currentType)
	{
		return;
	}

	// 鳴っているBGMがあれば、まずフェードアウトさせる
	// (ジングルがすでに鳴り終わっている場合は、待たずに切り替える)
	if (m_currentType != BGMType::None && m_spSoundInst && !m_spSoundInst->IsStopped())
	{
		m_isFadingOut = true;
		m_fadeRate = 1.0f;

		return;
	}

	StopCurrent();

	PlayType(target);
}

void BGMManager::Reset()
{
	StopCurrent();

	m_overrideType = BGMType::None;
}

BGMManager::BGMType BGMManager::DecideTargetType() const
{
	// 上書き中(タイトル・リザルトなど)は、それを最優先で流す
	if (m_overrideType != BGMType::None)
	{
		return m_overrideType;
	}

	// エディタのEditモードでは鳴らさない
	if (SceneManager::Instance().GetCurrentScene<EditorScene>() &&
		EditorManager::Instance().IsEditMode())
	{
		return BGMType::None;
	}

	if (RoomManager::Instance().IsBossIntro())
	{
		return BGMType::None;
	}

	// ボスを倒した演出中も、BGMを止める
	if (RoomManager::Instance().IsBossDefeat())
	{
		return BGMType::None;
	}

	// 部屋の状態で決める
	if (RoomManager::Instance().IsBossCombat())
	{
		return BGMType::Boss;
	}

	if (RoomManager::Instance().IsCombat())
	{
		return BGMType::Battle;
	}

	return BGMType::Field;
}

void BGMManager::PlayType(const BGMType type)
{
	if (type == BGMType::None)
	{
		return;
	}

	// 読み込みに失敗しても毎フレーム読み直さないよう、先に今の種類として覚えておく
	m_currentType = type;
	m_fadeRate = 1.0f;

	const std::string path = GetPath(type);

	m_spSoundInst = KdAudioManager::Instance().PlayBGM(path, IsLoop(type));

	if (!m_spSoundInst)
	{
		KdDebugGUI::Instance().AddErrorLog("BGM play failed : %s\n", path.c_str());
	}
}

void BGMManager::StopCurrent()
{
	KdAudioManager::Instance().StopBGM();

	m_spSoundInst = nullptr;
	m_currentType = BGMType::None;
	m_isFadingOut = false;
	m_fadeRate = 1.0f;
}

void BGMManager::ApplyFadeVolume()
{
	if (!m_spSoundInst)
	{
		return;
	}

	// フェード率 * 現在の音量 * マスター音量　を掛ける
	m_spSoundInst->SetVolume(m_fadeRate * KdAudioManager::Instance().GetBGMVolume()*KdAudioManager::Instance().GetMasterVolume());
}

std::string BGMManager::GetPath(const BGMType type) const
{
	switch (type)
	{
	case BGMType::Title:    return std::string(kBGMFilePath) + "Title/Title.wav";
	case BGMType::Field:    return std::string(kBGMFilePath) + "Field/Field.wav";
	case BGMType::Battle:   return std::string(kBGMFilePath) + "Battle/Battle.wav";
	case BGMType::Boss:     return std::string(kBGMFilePath) + "Boss/Boss.wav";
	case BGMType::Result:   return std::string(kBGMFilePath) + "Result/Result.wav";
	case BGMType::GameOver: return std::string(kBGMFilePath) + "Result/GameOver.wav";
	case BGMType::GameClear:return std::string(kBGMFilePath) + "Result/GameClear.wav";
	case BGMType::None:     break;
	}

	return "";
}

bool BGMManager::IsLoop(const BGMType type) const
{
	switch (type)
	{
	// 一度だけ鳴らすジングル
	case BGMType::GameOver:
	case BGMType::GameClear:
		return false;

	default:
		break;
	}

	return true;
}
