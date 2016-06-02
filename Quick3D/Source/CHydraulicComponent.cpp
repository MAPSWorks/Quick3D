
// Application
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CHydraulicComponent.h"

//-------------------------------------------------------------------------------------------------

CComponent* CHydraulicComponent::instanciator(C3DScene* pScene)
{
	return new CHydraulicComponent(pScene);
}

//-------------------------------------------------------------------------------------------------

CHydraulicComponent::CHydraulicComponent(C3DScene* pScene)
: CComponent(pScene)
, m_dPressure(0.0)
{
	LOG_DEBUG("CHydraulicComponent::CHydraulicComponent()");
}

//-------------------------------------------------------------------------------------------------

CHydraulicComponent::~CHydraulicComponent()
{
	LOG_DEBUG("CHydraulicComponent::~CHydraulicComponent()");
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::loadParameters(CXMLNode xComponent)
{
	CComponent::loadParameters(xComponent);

	QVector<CXMLNode> vInputsNodes = xComponent.getNodesByTagName(ParamName_PowerInput);

	foreach (CXMLNode xInput, vInputsNodes)
	{
		if (xInput.m_vAttributes[ParamName_Name].isEmpty() == false)
		{
			m_vInputs.append(CComponentReference<CHydraulicComponent>(xInput.m_vAttributes[ParamName_Name]));
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::solveLinks(C3DScene* pScene)
{
	CComponent::solveLinks(pScene);

	for (int iIndex = 0; iIndex < m_vInputs.count(); iIndex++)
	{
		m_vInputs[iIndex].solve(pScene, this);
	}
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::update(double dDeltaTime)
{
	if (m_vInputs.count() > 0 && m_vInputs[0].component() != NULL)
	{
		double dPressure = m_vInputs[0].component()->pull(0.0, dDeltaTime);
		push(dPressure, dDeltaTime);
	}

	/*
	LOG_VALUE(QString("%1 (I/U/T)").arg(m_sName),
		QString("%1 %2 %3")
		.arg(QString::number(m_tLoad.m_dAmperage, 'f', 2))
		.arg(QString::number(m_tLoad.m_dVoltage, 'f', 0))
		.arg(QString::number(m_tLoad.m_eType))
		);
	*/
}

//-------------------------------------------------------------------------------------------------

void CHydraulicComponent::push(double dPressure, double dDeltaTime)
{
	m_dPressure += dPressure;
}

//-------------------------------------------------------------------------------------------------

double CHydraulicComponent::pull(double dPressure, double dDeltaTime)
{
	m_dPressure -= dPressure * dDeltaTime;

	return m_dPressure;
}