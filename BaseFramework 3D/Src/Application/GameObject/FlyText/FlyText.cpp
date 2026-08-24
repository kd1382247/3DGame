#include "FlyText.h"

#include"../../System/GameObjectFinder/GameObjectFinder.h"

#include"../Camera/CameraBase.h"


void FlyText::Init(int damage, const Math::Vector3& spawnPos)
{
	if (!m_spPolygon)
	{
		m_spPolygon = std::make_shared<KdSquarePolygon>();

		m_spPolygon->SetMaterial("Asset/Textures/Numbers/Numbers.png");
		m_spPolygon->SetSplit(10, 1);
		m_spPolygon->SetScale(0.3f);

		m_text = std::to_string(damage);

		// カメラクラスを取得
		SetUpReference();

	}

	SetPos(spawnPos);
}

void FlyText::Update()
{

	Math::Vector3 pos = GetPos();

	pos.y += m_moveSpeed;

	SetPos(pos);

	m_alpha -= 0.033;

	m_lifeTime--;

	if (m_lifeTime <= 0)
	{
		Destroy();
	}

}

void FlyText::DrawEffect()
{

	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZDisable);

	const float digitSpace = 0.3f;

	// 数字全体を中央揃えするための開始位置
	const float startOffset =-((m_text.size() - 1) * digitSpace) * 0.5f;

	for (size_t i = 0; i < m_text.size(); ++i)
	{
		char c = m_text[i];

		if (!std::isdigit(c))
		{
			continue;
		}

		int number = c - '0';

		float xOffset =startOffset + digitSpace * i;

		DrawDigit(number, xOffset);
	}


	KdShaderManager::Instance().UndoDepthStencilState();
}

void FlyText::SetUpReference()
{
	if(!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}
}

void FlyText::DrawDigit(int digit, float xOffset)
{
	// ビルボード処理
	Math::Matrix billboardMat=Math::Matrix::Identity;

	if (m_wpCamera.lock())
	{
		billboardMat = m_wpCamera.lock()->GetRotationMatrix();
		billboardMat.Translation(Math::Vector3::Zero);
	}



	m_spPolygon->SetUVRect(digit);

	Math::Matrix offsetMat =Math::Matrix::CreateTranslation(xOffset,0.0f,0.0f);

	Math::Matrix drawMat =offsetMat * billboardMat * m_mWorld;

	Math::Color color = { 1.0f,1.0f,1.0f,m_alpha };

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spPolygon,drawMat,color);
}
