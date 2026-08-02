#include "Beholder.h"

#include"../../../../System/CollisionManager/CollisionManager.h"


void Beholder::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Beholder/Beholder.gltf");

		// オブジェクト名セット
		SetObjectName("Beholder");
	}

	SetPos({ 0,0,0 });

}

void Beholder::Update()
{


}

void Beholder::PostUpdate()
{
	EnemyBase::PostUpdate();
}
