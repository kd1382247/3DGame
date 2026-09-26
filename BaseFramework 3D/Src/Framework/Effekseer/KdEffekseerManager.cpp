#include"../../Application/System/TimeManager/TimeManager.h"


void KdEffekseerManager::Create(int w, int h)
{
	// エフェクトのレンダラーの作成
	m_efkRenderer = ::EffekseerRendererDX11::Renderer::Create(KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext(), 8000);

	// エフェクトのマネージャーの作成
	m_efkManager = ::Effekseer::Manager::Create(8000);

	// 左手座標系に変換
	m_efkManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

	// 描画用インスタンスから描画機能を設定
	m_efkManager->SetSpriteRenderer(m_efkRenderer->CreateSpriteRenderer());
	m_efkManager->SetRibbonRenderer(m_efkRenderer->CreateRibbonRenderer());
	m_efkManager->SetRingRenderer(m_efkRenderer->CreateRingRenderer());
	m_efkManager->SetTrackRenderer(m_efkRenderer->CreateTrackRenderer());
	m_efkManager->SetModelRenderer(m_efkRenderer->CreateModelRenderer());

	// GPUパーティクル用のデータ生成機能を登録
	m_efkManager->SetGpuParticleFactory(m_efkRenderer->CreateGpuParticleFactory());

	// GPUパーティクルの計算・描画を管理する機能を登録
	m_efkManager->SetGpuParticleSystem(m_efkRenderer->CreateGpuParticleSystem());


	// 描画用インスタンスからテクスチャの読み込み機能を設定
	m_efkManager->SetTextureLoader(m_efkRenderer->CreateTextureLoader());
	m_efkManager->SetModelLoader(m_efkRenderer->CreateModelLoader());
	m_efkManager->SetMaterialLoader(m_efkRenderer->CreateMaterialLoader());
	m_efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());



	// 投影行列を設定
	m_efkRenderer->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(
			90.0f / 180.0f * 3.14f, (float)w / (float)h, 1.0f, 500.0f));
}

void KdEffekseerManager::Update()
{
	
	if (m_efkManager == nullptr) { return; }

	UpdateEffekseerEffect();

	UpdateEkfCameraMatrix();

}

void KdEffekseerManager::Draw()
{

	if (m_efkManager == nullptr ||
		m_efkRenderer == nullptr) {
		return;
	}

	// GPUパーティクルの更新計算を指示
	m_efkManager->Compute();

	m_efkRenderer->BeginRendering();
	m_efkManager->Draw();
	m_efkRenderer->EndRendering();

}

std::weak_ptr<KdEffekseerObject> KdEffekseerManager::Play(
	const std::string& effName, const DirectX::SimpleMath::Vector3& pos, const float size, const float speed, bool isLoop, const int startFrame,const int endFrame, const Math::Vector3& rotate)
{
	PlayEfkInfo info;

	info.FileName	= effName;
	info.Pos		= pos;
	info.Size		= Math::Vector3(size);
	info.Rotate		= rotate;
	info.Speed		= speed;
	info.IsLoop		= isLoop;
	info.StartFrame = startFrame;
	info.EndFrame   = endFrame;
	return Play(info);
}

void KdEffekseerManager::StopAllEffect()
{
	if (m_efkManager == nullptr) { return; }

	auto efkFoundItr = m_nowEffectPlayList.begin();
	while (efkFoundItr != m_nowEffectPlayList.end())
	{
		KdEffekseerObject* effObj = efkFoundItr->get();
		if (effObj)
		{
			if (effObj->IsLoop()) effObj->SetLoop(false);
		}
		++efkFoundItr;
	}

	m_efkManager->StopAllEffects();
}

void KdEffekseerManager::StopEffect(const std::string& name)
{
	auto foundItr = m_effectMap.find(name);

	if (foundItr == m_effectMap.end()) { return; }

	if (foundItr->second->IsLoop())
	{
		foundItr->second->SetLoop(false);
		return;
	}

	m_efkManager->StopEffect(foundItr->second->GetHandle());
}

void KdEffekseerManager::StopEffect(const Effekseer::Handle& handle)
{
	m_efkManager->StopEffect(handle);
}

void KdEffekseerManager::Release()
{
	Reset();

	m_efkManager.Reset();
	m_efkRenderer.Reset();
}

void KdEffekseerManager::Reset()
{
	StopAllEffect();

	m_effectMap.clear();
	m_nowEffectPlayList.clear();

	m_isPause = false;
}

void KdEffekseerManager::SetPos(const int handle, const Math::Vector3& pos)
{
	Effekseer::Vector3D efkPos = GetEfkVec3D(pos);

	m_efkManager->SetLocation(handle, efkPos);
}

void KdEffekseerManager::SetRotation(const int handle, const Math::Vector3& axis, const float angle)
{
	Effekseer::Vector3D efkAxis = GetEfkVec3D(axis);

	m_efkManager->SetRotation(handle, efkAxis, angle);
}

void KdEffekseerManager::SetWorldMatrix(const int handle, const Math::Matrix& mWorld)
{
	Effekseer::Matrix43 mEfk{};

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			mEfk.Value[i][j] = mWorld.m[i][j];
		}
	}

	m_efkManager->SetMatrix(handle, mEfk);
}

void KdEffekseerManager::SetScale(const int handle, const Math::Vector3& scale)
{
	float scaleX = scale.x;
	float scaleY = scale.y;
	float scaleZ = scale.z;

	m_efkManager->SetScale(handle, scaleX, scaleY, scaleZ);
}

void KdEffekseerManager::SetScale(const int handle, const float scale)
{
	m_efkManager->SetScale(handle, scale, scale, scale);
}

void KdEffekseerManager::SetSpeed(const int handle, const float speed)
{
	m_efkManager->SetSpeed(handle, speed);
}

void KdEffekseerManager::SetPause(const int handle, const bool isPause)
{
	m_efkManager->SetPaused(handle, isPause);
}

const bool KdEffekseerManager::IsPlaying(const int handle) const
{
	return (m_efkManager->GetInstanceCount(handle) != 0);
}

bool KdEffekseerManager::HasEffectFinished(const Effekseer::Handle& handle) const
{
	// 未再生、または既に無効化されたハンドルは「終了している」とみなす
	if (handle < 0) { return true; }

	return m_efkManager->GetInstanceCount(handle) == 0;
}

std::weak_ptr<KdEffekseerObject> KdEffekseerManager::Play(const PlayEfkInfo& info)
{
	// 渡された座標をEffekseerの座標に置き換え
	Effekseer::Vector3D efkPos = GetEfkVec3D(info.Pos);

	Effekseer::Handle handle = 0;

	std::shared_ptr<KdEffekseerObject> spEfkObject = std::make_shared<KdEffekseerObject>();

	auto efkFoundItr = m_effectMap.find(info.FileName);
	// 既に生成されたことがある
	if (efkFoundItr != m_effectMap.end())
	{
		handle = m_efkManager->Play(efkFoundItr->second->GetEffect(), efkPos,info.StartFrame);
		spEfkObject->SetEffect(efkFoundItr->second->WorkEffect());
	}
	// エフェクト新規生成
	else
	{
		std::string loadFileName = EffekseerPath + info.FileName;

		// エフェクト新規生成
		auto effect = Effekseer::Effect::Create(m_efkManager,
			(const EFK_CHAR*)sjis_to_wide(loadFileName).c_str());

		if (effect == nullptr)
		{
#ifdef _DEBUG
			assert(0 && "Effekseerのエフェクト作成失敗");
#endif
			return std::weak_ptr<KdEffekseerObject>();
		}

		handle = m_efkManager->Play(effect, efkPos,info.StartFrame);
		spEfkObject->SetEffect(effect);
		m_effectMap[info.FileName] = spEfkObject;
	}

	m_efkManager->SetScale(handle, info.Size.x, info.Size.y, info.Size.z);
	m_efkManager->SetSpeed(handle, info.Speed);
	Math::Vector3 rotate = ConvertToRadian(info.Rotate);
	m_efkManager->SetRotation(handle, rotate.x, rotate.y, rotate.z);
	spEfkObject->SetParentManager(m_efkManager);
	spEfkObject->SetHandle(handle);
	spEfkObject->SetPlayEfkInfo(info);
	m_nowEffectPlayList.emplace_back(spEfkObject);
	return spEfkObject;
}

std::weak_ptr<KdEffekseerObject> KdEffekseerManager::Play(const std::shared_ptr<KdEffekseerObject>& spObject)
{
	return Play(spObject->GetPlayEfkInfo());
}

void KdEffekseerManager::UpdateEffekseerEffect()
{
	if (m_isPause) { return; }

	const float deltaTime =
		TimeManager::Instance().GetDeltaTime();

	const float deltaFrames = deltaTime * 60.0f;

	m_efkManager->Update(deltaFrames);

	std::vector<PlayEfkInfo> replayList;

	auto it = m_nowEffectPlayList.begin();

	while (it != m_nowEffectPlayList.end())
	{
		const auto& effect = *it;

		if (!effect)
		{
			it = m_nowEffectPlayList.erase(it);
			continue;
		}

		const auto handle = effect->GetHandle();

		const bool finished =
			m_efkManager->GetInstanceCount(handle) == 0;

		// 一時停止中は、区間の時計も止める
		if (!finished && !m_efkManager->GetPaused(handle))
		{
			effect->AdvanceFrames(
				deltaFrames * m_efkManager->GetSpeed(handle)
			);
		}

		const bool reachedEnd = effect->HasReachedEndFrame();

		if (finished || reachedEnd)
		{
			if (!finished)
			{
				// 今回の再生だけを停止する
				m_efkManager->StopEffect(handle);
			}

			if (effect->IsLoop())
			{
				replayList.push_back(effect->GetPlayEfkInfo());
			}

			it = m_nowEffectPlayList.erase(it);

			continue;
		}

		++it;
	}

	// リストの走査が終わってから再生し直す
	for (const auto& info : replayList)
	{
		Play(info);
	}
}
void KdEffekseerManager::UpdateEkfCameraMatrix()
{
	std::shared_ptr<KdCamera> spCamera = m_wpCamera.lock();
	if (!spCamera) return;

	Math::Matrix mView = spCamera->GetCameraMatrix().Invert();
	Math::Matrix mProj = spCamera->GetProjMatrix();
	Effekseer::Matrix44 mEfkView;
	Effekseer::Matrix44 mEfkProj;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			mEfkView.Values[i][j] = mView.m[i][j];
			mEfkProj.Values[i][j] = mProj.m[i][j];
		}
	}

	m_efkRenderer->SetProjectionMatrix(mEfkProj);
	m_efkRenderer->SetCameraMatrix(mEfkView);
}

bool KdEffekseerObject::IsPlaying()
{
	if (m_parentManager == nullptr) { return false; }

	// ハンドルが0( 未再生 or 再生終了 )でない場合はTrue, 
	// そうでなければFalse
	return m_parentManager->GetInstanceCount(m_handle) != 0;
}

void KdEffekseerObject::SetPos(const Math::Vector3& pos)
{
	m_info.Pos = pos;

	KdEffekseerManager::GetInstance().SetPos(m_handle, pos);
}

void KdEffekseerObject::SetScale(const Math::Vector3 scale)
{
	m_info.Size = scale;

	KdEffekseerManager::GetInstance().SetScale(m_handle, scale);
}

void KdEffekseerObject::SetScale(const float scale)
{
	m_info.Size = Math::Vector3(scale);

	KdEffekseerManager::GetInstance().SetScale(m_handle, scale);
}

void KdEffekseerObject::SetSpeed(const float speed)
{
	m_info.Speed = speed;

	KdEffekseerManager::GetInstance().SetSpeed(m_handle, speed);
}

void KdEffekseerObject::SetWorldMatrix(const Math::Matrix& mWorld)
{
	KdEffekseerManager::GetInstance().SetWorldMatrix(m_handle, mWorld);
}