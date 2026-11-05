#pragma once

class StageBackup
{
public:

	StageBackup() = delete;
	~StageBackup() = default;



	// データを退避
	static void Backup();

	// データを復元
	static void Restore();

	// バックアップを捨てる
	static void Discard();

};