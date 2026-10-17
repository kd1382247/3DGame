#include "EnemyCounter.h"

#include"../../Scene/SceneManager.h"

#include"../Character/Enemy/EnemyBase.h"

#include"../Gimmick/Room/Room.h"
#include"../Gimmick/Room/RoomManager.h"

void EnemyCounter::Init()
{
	InitTexture(m_spEnemyIconTex, "Asset/Textures/EnemyCounter/EnemyIcon.png");
	InitTexture(m_spNumberTex, "Asset/Textures/EnemyCounter/Numbers.png");

}

void EnemyCounter::Update()
{
	// 表示しない間は数えなくてよい
	if (!IsDrawCounter())
	{
		return;
	}

	m_enemyCount = CountEnemies();
}

void EnemyCounter::DrawSprite()
{
	if (!IsDrawCounter())
	{
		return;
	}

	DrawEnemyIcon();
	DrawEnemyNumber();
}

void EnemyCounter::InitTexture(std::shared_ptr<KdTexture>&tex, const std::string & filePath)
{
	if (tex){return;}

	tex = std::make_shared<KdTexture>();
	tex->Load(filePath);
}

bool EnemyCounter::IsDrawCounter() const
{
	auto spRoom = RoomManager::Instance().GetCurrentRoom();

	// どの部屋にも入っていない
	if (!spRoom)
	{
		return false;
	}

	// ボス部屋とSafeの部屋では表示しない
	const Room::RoomType roomType = spRoom->GetRoomType();
	if (roomType == Room::RoomType::Boss || roomType == Room::RoomType::Safe)
	{
		return false;
	}

	// 戦闘前の演出中と戦闘中だけ表示する(未訪問・クリア後の演出中・クリア済みは表示しない)
	const Room::RoomState roomState = spRoom->GetRoomState();
	return roomState == Room::RoomState::Intro
		|| roomState == Room::RoomState::InCombat;
}

int EnemyCounter::CountEnemies() const
{
	auto spRoom = RoomManager::Instance().GetCurrentRoom();

	if (!spRoom)
	{
		return 0;
	}

	int count = 0;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		if (!obj || obj->IsExpired())
		{
			continue;
		}

		// EnemyBaseにキャストできたら敵(BossBaseはEnemyBaseではないので数えない)
		auto spEnemy = std::dynamic_pointer_cast<EnemyBase>(obj);
		if (!spEnemy)
		{
			continue;
		}

		// HPが0になった敵(死亡演出中)は数えない
		if (spEnemy->GetHealth().IsDead())
		{
			continue;
		}

		// 今いる部屋の範囲外にいる敵は数えない
		if (!spRoom->IsInside(spEnemy->GetPos()))
		{
			continue;
		}

		++count;
	}

	return count;
}

void EnemyCounter::DrawEnemyIcon()
{
	if (!m_spEnemyIconTex)
	{
		return;
	}

	// 数字3桁ぶんの左端
	const int numberLeft = kScreenRight - kMarginRight - kDigitW * kMaxDigit;

	// アイコンの左端(数字の左端から、隙間とアイコンの幅ぶん左へ)
	const int iconX = numberLeft - kIconNumberGap - kIconW;

	// 縦の中央(上端から余白とアイコンの高さの半分ぶん下へ)
	const int iconY = kScreenTop - kMarginTop - kIconH / 2;

	Math::Color color = { 1.0f,1.0f,1.0f,1.0f };

	KdShaderManager::Instance().m_spriteShader.DrawTex(
		m_spEnemyIconTex,
		iconX, iconY,
		kIconW, kIconH,
		nullptr, &color, m_pivot);
}

void EnemyCounter::DrawEnemyNumber()
{
	if (!m_spNumberTex)
	{
		return;
	}

	// 3桁を超えないようにする
	int number = std::clamp(m_enemyCount, 0, kMaxCount);

	// 桁数を数える(0のときも1桁)
	int digitCount = 1;
	for (int n = number; n >= 10; n /= 10)
	{
		++digitCount;
	}

	// 数字3桁ぶんの左端(桁数が少なくても、ここを左端にそろえる)
	const int numberLeft = kScreenRight - kMarginRight - kDigitW * kMaxDigit;

	// 縦の中央
	const int numberY = kScreenTop - kMarginTop - kIconH / 2;

	Math::Color color = { 1.0f,1.0f,1.0f,1.0f };

	// 一の位から順に、右の桁から描いていく
	for (int i = digitCount - 1; i >= 0; --i)
	{
		const int digit = number % 10;
		number /= 10;

		// 数字画像から、この数字の部分だけを切り出す
		Math::Rectangle rc = { digit * kNumberTexCellW, 0, kNumberTexCellW, kNumberTexCellH };

		KdShaderManager::Instance().m_spriteShader.DrawTex(
			m_spNumberTex,
			numberLeft + i * kDigitW, numberY,
			kDigitW, kDigitH,
			&rc, &color, m_pivot);
	}
}
