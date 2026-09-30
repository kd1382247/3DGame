#include "EnemyHPBar.h"

#include"../../Character/Enemy/EnemyBase.h"
#include"../../Character/Player/Player.h"

#include"../../Camera/CameraBase.h"
#include"../../../System/GameObjectFinder/GameObjectFinder.h"

void EnemyHPBar::Init()
{
	
	// 各ポリゴンの初期化
	InitPolygon(m_spFrame, "Frame.png", KdSquarePolygon::PivotType::Left_Bottom, { m_barWidth,m_barHeight });
	InitPolygon(m_spFrameBackground, "Frame_bg.png", KdSquarePolygon::PivotType::Left_Bottom, { m_barWidth,m_barHeight });
	InitPolygon(m_spBarRed, "Bar_red.png", KdSquarePolygon::PivotType::Left_Bottom);
	InitPolygon(m_spBarGreen, "Bar_green.png", KdSquarePolygon::PivotType::Left_Bottom);


	SetUpReference();
}

void EnemyHPBar::Update()
{
	auto spEnemy = std::dynamic_pointer_cast<EnemyBase>(m_wpTarget.lock());
	
	if (!spEnemy)
	{
		Destroy();
		return;
	}

	auto spPlayer = m_wpPlayer.lock();

	if (!spPlayer)
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
	else
	{
		m_damageRate = m_hpRate;
	}

	Math::Vector3 displayDistance = spEnemy->GetPos() - spPlayer->GetPos();


	// HPバーを表示するかを判定
	if (currentHP < maxHP&&
		displayDistance.Length()<10.0f)
	{
		m_isHPBarVisible = true;
	}
	else
	{
		m_isHPBarVisible = false;
	}
}

void EnemyHPBar::DrawEffect()
{
	auto spEnemy = m_wpTarget.lock();
	auto spCamera = m_wpCamera.lock();
	if (!spEnemy||!spCamera)
	{
		return;
	}

	if (!m_isHPBarVisible)
	{
		return;
	}

	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZDisable);

	DrawFrameBackground();
	DrawBarRed();
	DrawBarGreen();
	DrawFrame();

	KdShaderManager::Instance().UndoDepthStencilState();

}

void EnemyHPBar::SetUpReference()
{
	if (!m_wpCamera.lock())
	{
		m_wpCamera = GameObjectFinder::Instance().FindObject<CameraBase>();
	}

	if (!m_wpPlayer.lock())
	{
		m_wpPlayer = GameObjectFinder::Instance().FindObject<Player>();
	}
}


void EnemyHPBar::InitPolygon(std::shared_ptr<KdSquarePolygon>& polygon, const std::string& filePath, const KdSquarePolygon::PivotType type, const Math::Vector2& scale, const Math::Vector2& split)
{
	if (polygon)
	{
		return;
	}

	polygon = std::make_shared<KdSquarePolygon>();

	polygon->SetMaterial(MaterialPath + filePath);
	polygon->SetScale(scale);
	polygon->SetSplit(split.x,split.y);
	polygon->SetPivot(type);

}

void EnemyHPBar::DrawFrame()
{

	Math::Matrix drawMat = CreateBaseMatrix();

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spFrame, drawMat);
}

void EnemyHPBar::DrawFrameBackground()
{

	Math::Matrix drawMat = CreateBaseMatrix();

	KdShaderManager::Instance().m_StandardShader.DrawPolygon(*m_spFrameBackground,drawMat);
}

void EnemyHPBar::DrawBarRed()
{
	DrawBar(m_spBarRed, m_damageRate, DamageBarDepth);
}

void EnemyHPBar::DrawBarGreen()
{
	DrawBar(m_spBarGreen, m_hpRate, HPBarDepth);
}

void EnemyHPBar::DrawBar(const std::shared_ptr<KdSquarePolygon>&polygon, float rate, float depth)
{

	polygon->SetScale({ m_barWidth * rate,m_barHeight });
	polygon->SetUVRect({0.0f,0.0f},{rate,1.0f});
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

	Math::Matrix offsetMat = Math::Matrix::CreateTranslation(m_offsetPos);

	Math::Matrix targetMat = Math::Matrix::CreateTranslation(spEnemy->GetPos());

	return offsetMat * billboardMat * targetMat;
}
