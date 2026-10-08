#pragma once

class AIBlockCollision;

class StageObject :public KdGameObject
{

public:
	StageObject(){}
	~StageObject()override{}

	void Init()override;
	void PreDraw()override;
	// 固定表示(スクロールしない):位置・回転・大きさ
	void DrawInspectorHeader()override;

	// スクロール側:ステージモデルの選択
	void DrawInspector()override;
	void DrawLit()override;

	nlohmann::json SaveData()const override;
	void LoadData(const nlohmann::json& json) override;

	const std::string& GetStageModelName() const { return m_stageModelName; }
	void SetStageModelName(const std::string& name) { m_stageModelName = name; }

private:

	// modelNameのステージモデル(レンダー用モデル+GroundCollision+AIBlockCollision)を読み込む
	// Init()(新規オブジェクトのデフォルト読込)とLoadData()(セーブデータからの読込)の両方から呼ばれる
	void LoadStageModel(const std::string& modelName);


	std::shared_ptr<KdModelWork>m_model = nullptr;

	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	std::shared_ptr<AIBlockCollision>m_spAIBlockCollision = nullptr;

	std::string m_stageModelName = "Stage01";

};