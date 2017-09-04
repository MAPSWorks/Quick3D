
// Application
#include "C3DScene.h"
#include "CTerrestrialVehicle.h"

//-------------------------------------------------------------------------------------------------

CComponent* CTerrestrialVehicle::instantiator(C3DScene* pScene)
{
    return new CTerrestrialVehicle(pScene);
}

//-------------------------------------------------------------------------------------------------

CTerrestrialVehicle::CTerrestrialVehicle(C3DScene* pScene)
    : CVehicle(pScene)
{
    LOG_DEBUG("CTerrestrialVehicle::CTerrestrialVehicle()");
}

//-------------------------------------------------------------------------------------------------

CTerrestrialVehicle::~CTerrestrialVehicle()
{
    LOG_DEBUG("CTerrestrialVehicle::~CTerrestrialVehicle()");
}

//-------------------------------------------------------------------------------------------------

void CTerrestrialVehicle::loadParameters(const QString& sBaseFile, CXMLNode xComponent)
{
    CVehicle::loadParameters(sBaseFile, xComponent);
}
