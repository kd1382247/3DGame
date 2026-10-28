#pragma once

namespace EffekseerAssets
{
	// エフェクトのパス
	// パスは"Asset/Data/Effect/"以降から
	inline const std::array<std::string, 20> EffekseerPaths =
	{
		// Player関連
		"Player/Charge/Charge.efkefc",
		"Player/Guard/Guard.efkefc",
		"Player/GuardBreak/GuardBreak.efkefc",
		"Player/Heal/Heal.efkefc",
		"Player/Parry/Parry.efkefc",

		// Enemy関連
		"Enemy/EnemySpawner/Lightning_spawner.efkefc",
		"Enemy/EnemySpawner/Smoke_spawner.efkefc",
		"Enemy/Energy/Energy.efkefc",
		"Enemy/Explosion/Explosion.efkefc",

		// Common
		"Common/Hit/Flash_hit.efkefc",
		"Common/Hit/Slash_hit.efkefc",
		"Common/Hit/Star_hit.efkefc",
		"Common/Smoke/Smoke_purple.efkefc"
		"Common/Smoke/Smoke_white.efkefc",

		// ボス関連
		"Boss/Mage/Beam/Beam.efkefc",
		"Boss/Mage/Bolt/Bolt.efkefc",
		"Boss/Mage/BoltBreak/BoltBreak.efkefc",
		"Boss/Mage/Roar/Roar.efkefc",
		"Boss/Mage/Salamander/Salamander.efkefc",
		"Boss/Mage/Sword/Sword.efkefc",
		"Boss/Mage/Tornado/Tornado.efkefc",
	};

}