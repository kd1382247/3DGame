#include "StageBase.h"

void StageBase::Init()
{
	
}

void StageBase::Update()
{

}

void StageBase::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel,m_mWorld);
}
