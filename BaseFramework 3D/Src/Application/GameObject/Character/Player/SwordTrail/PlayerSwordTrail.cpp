#include "PlayerSwordTrail.h"

void PlayerSwordTrail::Init()
{
	m_spTrailPolygon = std::make_shared<KdTrailPolygon>();

	 m_spTrailPolygon->SetMaterial("Asset/Textures/SwordTrail/SwordTrail2.png");

	m_spTrailPolygon->SetPattern(KdTrailPolygon::Trail_Pattern::eVertices);

	// 1フレームに2点追加するので、残したいフレーム数の2倍を指定する（今は10フレーム分＝20点）
	m_spTrailPolygon->SetLength(40);
}

void PlayerSwordTrail::StartTrail()
{
	m_spTrailPolygon->ClearPoints();
	m_isTracking = true;
	m_historyCount = 0;
}

void PlayerSwordTrail::UpdateTrail(const std::shared_ptr<KdModelWork>& model,const Math::Matrix& worldMat, float animFrame)
{
	if (!m_isTracking)
	{

		if (m_spTrailPolygon->GetNumPoints() >= 2)
		{
			m_spTrailPolygon->DelPointBack();
			m_spTrailPolygon->DelPointBack();
		}

		return;
	}

	// 剣を振っている区間(m_trailStartFrame〜m_trailEndFrame)の外では、
	// トレイルを伸ばさずに縮めていく(振りかぶり中・振った後の戻りでは表示しない)
	bool inTrailRange = (animFrame >= m_trailStartFrame && animFrame <= m_trailEndFrame);

	if (!inTrailRange)
	{
		if (m_spTrailPolygon->GetNumPoints() >= 2)
		{
			m_spTrailPolygon->DelPointBack();
			m_spTrailPolygon->DelPointBack();
		}

		return;
	}

	std::shared_ptr<KdModelWork> spModel = model;
	if (!spModel) { return; }

	const KdModelData::Node* pDataNode = spModel->FindDataNode("weapon_r");
	const KdModelWork::Node* pWorkNode = spModel->FindNode("weapon_r");

	if (!pDataNode || !pWorkNode) { return; }

	Math::Matrix playerMat = worldMat;

	// 逆バインド行列 × 今の姿勢行列 × プレイヤーのワールド行列
	Math::Matrix skinMat = pDataNode->m_boneInverseWorldMatrix * pWorkNode->m_worldTransform * playerMat;

	Math::Vector3 tipWorld = Math::Vector3::Transform(m_tipLocalPos, skinMat);
	Math::Vector3 baseWorld = Math::Vector3::Transform(m_baseLocalPos, skinMat);

	// 履歴を1つ古い方へずらして、最新の実座標を追加
	m_tipHistory[0] = m_tipHistory[1];
	m_tipHistory[1] = m_tipHistory[2];
	m_tipHistory[2] = m_tipHistory[3];
	m_tipHistory[3] = tipWorld;

	m_baseHistory[0] = m_baseHistory[1];
	m_baseHistory[1] = m_baseHistory[2];
	m_baseHistory[2] = m_baseHistory[3];
	m_baseHistory[3] = baseWorld;

	if (m_historyCount < 4) { m_historyCount++; }

	if (m_historyCount < 4)
	{
		// 履歴が4点揃うまでは、そのまま追加するしかない
		AddPoint(tipWorld, baseWorld);
		return;
	}

	const int subStep = 4;
	for (int i = 1; i <= subStep; i++)
	{
		float t = (float)i / subStep;

		Math::Vector3 tipSub = Math::Vector3::CatmullRom(
			m_tipHistory[0], m_tipHistory[1], m_tipHistory[2], m_tipHistory[3], t);

		Math::Vector3 baseSub = Math::Vector3::CatmullRom(
			m_baseHistory[0], m_baseHistory[1], m_baseHistory[2], m_baseHistory[3], t);

		AddPoint(tipSub, baseSub);
	}
}

void PlayerSwordTrail::AddPoint(const Math::Vector3& tipPos, const Math::Vector3& basePos)
{
	// 毎回同じ順番(tip→base)で追加する
	m_spTrailPolygon->AddPoint(Math::Matrix::CreateTranslation(tipPos));
	m_spTrailPolygon->AddPoint(Math::Matrix::CreateTranslation(basePos));
}

void PlayerSwordTrail::EndTrail()
{
	m_isTracking = false;
}