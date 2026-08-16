#include "WayPointNumber.h"

#include"../WayPoint.h"

void WayPointNumber::Init()
{

	for(int i=0;i<maxDigits;++i)
	{
		if (!m_spPoly[i])
		{
			m_spPoly[i] = std::make_shared<KdSquarePolygon>();
			m_spPoly[i]->SetMaterial("Asset/Textures/Numbers/Numbers.png");


			m_spPoly[i]->SetSplit(10,0);
		}
	}

	// 数字の位置をセット
	float numberPos=0;

	for (int i = 0; i < maxDigits; ++i)
	{
		m_localNumberPos[i] = Math::Vector3::Zero;

		m_localNumberPos[i].x = numberPos;
		numberPos += 1;
	}

}

void WayPointNumber::Update()
{

	auto target = m_wpTarget.lock();
	if (!target)
	{
		return;
	}





	// 行列作成
	for (int i = 0; i < maxDigits; ++i)
	{
		m_mWorld[i] = Math::Matrix::CreateTranslation(m_localNumberPos[i]) * target->GetMatrix();
	}

}

void WayPointNumber::DrawUnLit()
{

	for (int i = 0; i < maxDigits;i++)
	{
		if (m_numberDrawFlg[i])
		{
			KdShaderManager::Instance().m_StandardShader.DrawPolygon(
				*m_spPoly[i],m_mWorld[i]);
		}
	}

}
