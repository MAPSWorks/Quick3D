
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CForceController.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CTankController : public CForceController
{

public:

    //-------------------------------------------------------------------------------------------------
    // Constructors and destructor
    //-------------------------------------------------------------------------------------------------

    //! Retourne une nouvelle instance de cet objet
    static CComponent* instanciator(C3DScene* pScene);

    //! Constructeur d'apr�s une sc�ne
    CTankController(C3DScene* pScene);

    //! Destructor
    virtual ~CTankController();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Control methods
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Inherited methods
    //-------------------------------------------------------------------------------------------------

    //! Returns this object's class name
    virtual QString getClassName() const { return ClassName_CTankController; }

    //! Charge les param�tres de cet objet
    virtual void loadParameters(const QString& sBaseFile, CXMLNode xComponent) Q_DECL_OVERRIDE;

    //! Recherche les liens de cet objet
    virtual void solveLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Efface les liens de cet objet
    virtual void clearLinks(C3DScene* pScene) Q_DECL_OVERRIDE;

    //! Met l'objet � jour d'apr�s le temps �coul� depuis la derni�re image
    virtual void update(double dDeltaTime) Q_DECL_OVERRIDE;

    //!
    virtual void keyPressEvent(QKeyEvent* event);

    //!
    virtual void keyReleaseEvent(QKeyEvent *event);

    //-------------------------------------------------------------------------------------------------
    // Properties
    //-------------------------------------------------------------------------------------------------

protected:

    bool	m_bTurretUp;
    bool	m_bTurretDown;
    bool	m_bTurretRight;
    bool	m_bTurretLeft;

    CComponentReference<CComponent>		m_rTurretYawTarget;
    CComponentReference<CComponent>		m_rTurretPitchTarget;
};
