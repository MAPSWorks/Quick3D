
#pragma once

// Application
#include "quick3d_global.h"
#include "CQ3DConstants.h"
#include "CElectricalComponent.h"

//-------------------------------------------------------------------------------------------------
// Forward declarations

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CElectricalConsumer : public CElectricalComponent
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //!
    static CComponent* instantiator(C3DScene* pScene);

    //!
    CElectricalConsumer(C3DScene* pScene);

    //!
    virtual ~CElectricalConsumer();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //!
    virtual QString getClassName() const { return ClassName_CElectricalConsumer; }

    //!
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    CElectricalLoad         m_tCurrent;
    bool                    m_bPowered;
};
