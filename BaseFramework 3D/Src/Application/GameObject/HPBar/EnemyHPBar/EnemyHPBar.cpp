#include "EnemyHPBar.h"

#include"../../Character/Enemy/EnemyBase.h"

#include"../../Camera/CameraBase.h"
#include"../../../System/GameObjectFinder/GameObjectFinder.h"

void EnemyHPBar::Init()
{

	if (!m_spBackground)
	{
		m_spBackground = std::make_shared<KdSquarePolygon>();

		m_spBackground->SetMaterial("Asset/Textures/HP/Background.png");
		m_spBackground->SetPivot(KdSquarePolygon::PivotType::Left_Bottom);

		m_spBackground->SetSplit(1, 1);
		m_spBackground->SetUVRect(0);
		m_spBackground->SetScale({ m_barWidth,m_barHeight });
	}

	if (!m_spDamageBar)
	{
		m_spDamageBar = std::make_shared<KdSquarePolygon>();

		m_spDamageBar->SetMaterial("Asset/Textures/HP/DamageBar.png");
		m_spDamageBar->SetPivot(KdSquarePolygon::PivotType::Left_Bottom);

		m_spDamageBar->SetSplit(1, 1);
		m_spDamageBar->SetUVRect(0);
	}

	if (!m_spHPBar)
	{
		m_spHPBar = std::make_shared<KdSquarePolygon>();

		m_spHPBar->SetMaterial("Asset/Textures/HP/HPBar.png");
		m_spHPBar->SetPivot(KdSquarePolygon::PivotType::Left_Bottom);

		m_spHPBar->SetSplit(1, 1);
		m_spHPBar->SetUVRect(0);
	}

}

void EnemyHPBar::Update()
{
	auto spEnemy = m_wpTarget.lock();
	if (!spEnemy)
	{
		return;
	}

	float maxHP = spEnemy->GetMaxHP();

	if (maxHP <= 0)
	{
		return;
	}

	float currentHP = spEnemy->GetCurrentHP();


	m_hpRate = std::clamp(currentHP / maxHP,0.0f,1.0f);
	
	// ダメージバーを減少
	if (m_damageRate > m_hpRate)
	{
		m_damageRate -= 0.008f;

		if (m_damageRate < m_hpRate)
		{
			m_damageRate = m_hpRate;
		}

	}

	SetUpReference();

}

void EnemyHPBar::DrawEffect()
{
	auto spEnemy = m_wpTarget.lock();
	auto spCamera = m_wpCamera.lock();
	if (!spEnemy||!spCamera)
	{
		return;
	}

	KdShaderManager::Instance().ChangeDepthStencilState(
		KdDepthStencilState::ZDisable);

	DrawBackground();
	DrawDamageBar();
	DrawHPBar();

	KdShaderManager::Instance().UndoDepthStencilState();

}

void EnemyHPBar::SetUpReference()
{
	if (!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}
}

void EnemyHPBar::DrawBackground()
{

	Math::Matrix drawMat = CreateBaseMatrix();

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spBackground,drawMat);
}

void EnemyHPBar::DrawDamageBar()
{
	DrawBar(m_spDamageBar, m_damageRate, DamageBarDepth);
}

void EnemyHPBar::DrawHPBar()
{
	DrawBar(m_spHPBar, m_hpRate, HPBarDepth);
}

void EnemyHPBar::DrawBar(const std::shared_ptr<KdSquarePolygon>&polygon, float rate, float depth)
{

	polygon->SetScale({ m_barWidth * rate,m_barHeight });

	Math::Matrix depthMat = Math::Matrix::CreateTranslation({ 0.0f,0.0f,depth});
	Math::Matrix drawMat = depthMat * CreateBaseMatrix();

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*polygon, drawMat);
}

Math::Matrix EnemyHPBar::CreateBaseMatrix() const
{
	auto spCamera = m_wpCamera.lock();
	auto spEnemy = m_wpTarget.lock();
	if (!spEnemy || !spCamera)
	{
		return Math::Matrix::Identity;
	}

	Math::Matrix billboardMat = spCamera->GetRotationMatrix();
	billboardMat.Translation(Math::Vector3::Zero);

	Math::Matrix offsetMat = Math::Matrix::CreateTranslation(m_barOffset);

	Math::Matrix targetMat = Math::Matrix::CreateTranslation(spEnemy->GetPos());

	return offsetMat * billboardMat * targetMat;
}
