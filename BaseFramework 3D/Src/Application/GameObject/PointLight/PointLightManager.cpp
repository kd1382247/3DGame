#include "PointLightManager.h"

#include "PointLight.h"

#include "../Stage/StageObject.h"
#include "../../Scene/SceneManager.h"

std::shared_ptr<PointLightObject> PointLightObjectManager::CreatePointLight(const std::shared_ptr<StageObject>& owner)
{
	const int id = FindAvailableID();

	std::shared_ptr<PointLightObject> light = std::make_shared<PointLightObject>();

	light->Init();

	light->SetID(id);

	// オブジェクトの名前をセット 後ろにID
	light->SetObjectName("PointLight_" + std::to_string(id));

	light->SetOwner(owner);

	// 持ち主のステージに合わせた位置にしておく
	light->UpdateWorldPos();

	SceneManager::Instance().AddObject(light);

	return light;
}

std::vector<std::shared_ptr<PointLightObject>> PointLightObjectManager::GetPointLights() const
{
	std::vector<std::shared_ptr<PointLightObject>> lights;

	for (const auto& obj : SceneManager::Instance().GetObjList())
	{
		auto light = std::dynamic_pointer_cast<PointLightObject>(obj);

		if (!light || light->IsExpired())
		{
			continue;
		}

		lights.push_back(light);
	}

	return lights;
}

void PointLightObjectManager::RemoveByOwner(const StageObject* owner)
{
	for (const auto& light : GetPointLights())
	{
		if (light->GetOwner().get() == owner)
		{
			light->Destroy();
		}
	}
}

void PointLightObjectManager::ApplyAllLights()
{
	for (const auto& light : GetPointLights())
	{
		light->ApplyLight();
	}
}

int PointLightObjectManager::FindAvailableID() const
{
	const auto lights = GetPointLights();

	int id = 0;

	while (true)
	{
		bool exists = false;

		for (const auto& light : lights)
		{
			if (light->GetID() == id)
			{
				exists = true;
				break;
			}
		}

		if (!exists)
		{
			return id;
		}

		++id;
	}
}

bool PointLightObjectManager::Save(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	nlohmann::json rootJson;
	rootJson["PointLights"] = nlohmann::json::array();

	for (const auto& light : GetPointLights())
	{
		if (light->GetOwner() != owner)
		{
			continue;
		}

		nlohmann::json lightJson;

		lightJson["ID"] = light->GetID();

		lightJson["Name"] = light->GetObjectName();

		// 座標(Stageからのローカル位置)
		const auto& pos = light->GetLocalPos();
		lightJson["Position"]["x"] = pos.x;
		lightJson["Position"]["y"] = pos.y;
		lightJson["Position"]["z"] = pos.z;

		// 色
		const auto& color = light->GetColor();
		lightJson["Color"]["r"] = color.x;
		lightJson["Color"]["g"] = color.y;
		lightJson["Color"]["b"] = color.z;

		// 範囲
		lightJson["Range"] = light->GetRange();

		rootJson["PointLights"].push_back(lightJson);
	}

	std::ofstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("PointLightDataの保存に失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("PointLightDataの保存に失敗しました\n");

		return false;
	}

	file << rootJson.dump(4);

	return true;
}

bool PointLightObjectManager::Load(const std::string& filePath, const std::shared_ptr<StageObject>& owner)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		OutputDebugStringA("PointLightDataを開けませんでした\n");
		KdDebugGUI::Instance().AddErrorLog("PointLightDataを開けませんでした\n");

		return false;
	}

	nlohmann::json rootJson;

	try
	{
		file >> rootJson;
	}
	catch (const nlohmann::json::exception& e)
	{
		OutputDebugStringA("PointLightDataの読み込みに失敗しました\n");
		KdDebugGUI::Instance().AddErrorLog("PointLightDataの読み込みに失敗しました\n");

		OutputDebugStringA(e.what());
		OutputDebugStringA("\n");
		KdDebugGUI::Instance().AddErrorLog("%s\n", e.what());

		return false;
	}

	if (!rootJson.contains("PointLights") ||
		!rootJson["PointLights"].is_array())
	{
		OutputDebugStringA("PointLights配列がありません\n");
		KdDebugGUI::Instance().AddErrorLog("PointLights配列がありません\n");

		return false;
	}

	// Jsonに保存されてる情報でPointLightを生成
	for (const auto& lightJson : rootJson["PointLights"])
	{
		auto light = CreatePointLight(owner);

		if (!light)
		{
			continue;
		}

		// 座標(Stageからのローカル位置)
		if (lightJson.contains("Position"))
		{
			const auto& pos = lightJson["Position"];

			light->SetLocalPos({
				pos.value("x", 0.0f),
				pos.value("y", 0.0f),
				pos.value("z", 0.0f)
				});
		}

		// 色
		if (lightJson.contains("Color"))
		{
			const auto& color = lightJson["Color"];

			light->SetColor({
				color.value("r", 3.0f),
				color.value("g", 3.0f),
				color.value("b", 3.0f)
				});
		}

		// 範囲
		light->SetRange(lightJson.value("Range", 3.0f));

		// 読み込んだローカル位置を、ワールド位置に反映
		light->UpdateWorldPos();
	}

	return true;
}
