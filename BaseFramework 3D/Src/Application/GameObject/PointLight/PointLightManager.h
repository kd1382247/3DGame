#pragma once

class StageObject;

class PointLightObject;

// PointLightの作成・保存・読み込みを行う
// PointLight自体はシーンのオブジェクトリストに載せる(Updateなどはシーンが呼ぶ)ので、
// バックアップ/復元(ステージ切り替え・Play開始)もオブジェクトリストと一緒に行われる
class PointLightObjectManager
{

public:

	// ライトを作ってシーンに追加する(ownerのステージに追従する)
	std::shared_ptr<PointLightObject> CreatePointLight(const std::shared_ptr<StageObject>& owner);

	// シーン上のライトを返す(削除済みのものは含まない)
	std::vector<std::shared_ptr<PointLightObject>> GetPointLights() const;

	// ownerが持ち主のライトを全て消す(ステージの削除・ステージモデルの変更で呼ぶ)
	void RemoveByOwner(const StageObject* owner);

	// 全てのライトの ApplyLight() を呼ぶ
	// (Edit中はオブジェクトのUpdateが止まるため、シーン側から毎フレーム呼んでライトを登録する)
	void ApplyAllLights();

	// 現在使われていない最小のIDを返す
	int FindAvailableID() const;

	// ownerが持ち主のライトをJsonファイルに保存する
	bool Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	// Jsonファイルからライトを作り、ownerを持ち主にする
	bool Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner);

	// デバッグの表示切り替えフラグ(選択中のライトは、OFFでも表示する)
	bool IsDebug() const { return m_isDebug; }
	void SetIsDebug(const bool flg) { m_isDebug = flg; }


private:

	bool m_isDebug = false;


private:

	PointLightObjectManager() {}
	~PointLightObjectManager() {}

public:

	static PointLightObjectManager& Instance()
	{
		static PointLightObjectManager instance;
		return instance;
	}

};
