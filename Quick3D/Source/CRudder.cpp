
// Application
#include "CLogManager.h"
#include "CConsoleBoard.h"
#include "C3DScene.h"
#include "CRudder.h"
#include "CAircraft.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CComponent* CRudder::instanciator(C3DScene* pScene)
{
	return new CRudder(pScene);
}

//-------------------------------------------------------------------------------------------------

CRudder::CRudder(C3DScene* pScene)
: CWing(pScene)
{
	LOG_DEBUG("CRudder::CRudder()");
}

//-------------------------------------------------------------------------------------------------

CRudder::~CRudder()
{
	LOG_DEBUG("CRudder::~CRudder()");
}

//-------------------------------------------------------------------------------------------------

void CRudder::update(double dDeltaTime)
{
	CComponent* pRoot = getRoot();
	CAircraft* pAircraft = dynamic_cast<CAircraft*>(pRoot);

	if (pAircraft != NULL)
	{
		// Apply aileron lift

		CAxis aRotationAxis;

		aRotationAxis = aRotationAxis.rotate(CVector3(m_dWingAngle_rad, 0.0, 0.0));
		aRotationAxis = aRotationAxis.rotate(pAircraft->getOriginRotation());

		CAxis aVelocityAxis(euleurAngles(pAircraft->getVelocity_ms()));

		aVelocityAxis = aVelocityAxis.transferFrom(aRotationAxis);

		double dDotBodyAirflow = aVelocityAxis.euleurAngles().X / Math::Pi;
		double dDotBodyAirflowAileronCorrected = m_iBodyAirflowDotAileronLiftFactor.getValue(dDotBodyAirflow);

		double dAirDragFactor = CAtmosphere::getInstance()->getAirDragFactor(pAircraft->getGeoloc().Altitude);
		double dVelocity = pAircraft->getVelocity_ms().getMagnitude();
		double dTotalMass_kg = pAircraft->getTotalMass_kg();
		double dMassMultiplier = sqrt(dTotalMass_kg);
		double dAileronAreaOnMass = m_dAileronArea_m2 / dMassMultiplier;
		double dAileronLift = dVelocity * dAileronAreaOnMass;

		dAileronLift *= dTotalMass_kg;
		dAileronLift *= m_dAileronAngle_rad;
		dAileronLift *= 5.0;
		dAileronLift *= dAirDragFactor;
		dAileronLift *= dDotBodyAirflowAileronCorrected;

		CVector3 vAileronForce = CVector3(-1.0, 0.0, 0.0) * dAileronLift;
		CVector3 vAileronPosition = getOriginPosition() + m_vAileronPosition;

		pAircraft->addUncenteredLocalForce_kg(vAileronPosition, vAileronForce);

		LOG_VALUE(QString("%1 AIL POS / AIL LIFT KG").arg(m_sName),
			QString("%1 / %2")
			.arg(QString::number(m_dAileronAngle_rad, 'f', 2))
			.arg(QString::number(dAileronLift, 'f', 2))
			);
	}
}