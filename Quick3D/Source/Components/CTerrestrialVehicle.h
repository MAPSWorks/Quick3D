
#pragma once

// qt-plus
#include "CLogger.h"

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CVehicle.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTerrestrialVehicle : public CVehicle
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Returns a new instance of this class
    static CComponent* instantiator(C3DScene* pScene);

    //! Constructor using a scene
    CTerrestrialVehicle(C3DScene* pScene);

    //! Destructor
    virtual ~CTerrestrialVehicle();

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const Q_DECL_OVERRIDE { return ClassName_CTerrestrialVehicle; }

    //!
    virtual void loadParameters(const QString& sBaseFile, const CXMLNode& xComponent) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------
};
