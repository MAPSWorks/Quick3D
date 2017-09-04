
// qt-plus
#include "CLogger.h"
#include "CMemoryMonitor.h"

// Application
#include "CHeightField.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CHeightField
    \brief The base class for all height fields.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CHeightField. \a dRigidness is a default value for terrain rigidness.
*/
CHeightField::CHeightField(double dRigidness)
    : m_dRigidness(dRigidness)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CHeightField.
*/
CHeightField::~CHeightField()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a pRigidness, if not nullptr, is filled with the terrain rigidness at the specified location.
*/
double CHeightField::getHeightAt(const CGeoloc& gPosition, double* pRigidness)
{
    if (pRigidness != nullptr) *pRigidness = m_dRigidness;

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a pRigidness, if not nullptr, is filled with the terrain rigidness at the specified location.
    \a aAxis
*/
double CHeightField::getHeightAt(const CVector3& vPosition, const CAxis& aAxis, double* pRigidness)
{
    if (pRigidness != nullptr) *pRigidness = m_dRigidness;

    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the height at the specified \a gPosition. \br\br
    \a aAxis
    \a bForPhysics
*/
double CHeightField::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
    return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns \c true if the terrain is generated by functions (has no significant amount of data in memory). \br
    Returns false by default, can be overridden by subclasses if they are low memory consumers.
*/
bool CHeightField::isGenerated()
{
    return false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Flattens terrain at the specified \a gPosition, to the extents of \a dRadius.
*/
void CHeightField::flatten(const CGeoloc& gPosition, double dRadius)
{
}
