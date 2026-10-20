#pragma once

class StageObject;

// ステージモデルに追従する機能(Room / RoomGate が持つ)
// 追従先のステージが動いたり、大きさが変わったりしたとき、自分も同じだけ動かす
// 自分の位置や大きさをInspectorで直接変えることもできる(次にステージが動いたときも、その位置からの差分で動く)
class StageFollower
{
public:

	// 毎フレーム(PreDraw)に呼ぶ。targetは追従する側(自分自身)
	void Update(KdGameObject& target);

	// Inspector用。追従しているステージ(持ち主)の名前を表示する
	void DrawInspector();

	// 追従するステージ(持ち主)を決める
	void SetStage(const std::shared_ptr<StageObject>& stage);

	// 追従しているステージ(持ち主)を返す(いなければnullptr)
	std::shared_ptr<StageObject> GetStage();

	// 持ち主が決まっているか(名前が入っているか。ステージを探せるかは見ない)
	bool HasStage()const { return !m_stageName.empty(); }

	// 持ち主のステージの名前(ステージを探せるかは見ない)
	const std::string& GetStageName()const { return m_stageName; }

	// 追従先のステージ名を保存・読み込みする
	void SaveData(nlohmann::json& json)const;
	void LoadData(const nlohmann::json& json);

private:

	// 追従先のステージを決める(今のステージの位置・大きさを、動いた分を測る基準として覚える)
	void Link(const std::shared_ptr<StageObject>& stage);

	// 名前からステージを探す(いなければnullptr)
	std::shared_ptr<StageObject> FindStageByName()const;

	// 追従するステージのオブジェクト名(保存するのはこの名前)
	std::string m_stageName;
	std::weak_ptr<StageObject> m_wpStage;

	// 前のフレームでのステージの位置と大きさ(今との差が、動いた分)
	Math::Vector3 m_lastStagePos = Math::Vector3::Zero;
	Math::Vector3 m_lastStageScale = Math::Vector3::One;

	bool m_warnedMissingStage = false;
};
