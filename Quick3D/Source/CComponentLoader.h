
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CSingleton.h"
#include "CXMLNode.h"

// Quick3D
#include "CComponent.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CComponentLoader : public CSingleton<CComponentLoader>
{
    friend class CSingleton<CComponentLoader>;

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

protected:

    //! Constructeur par d�faut
    CComponentLoader();

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

public:

    //! Charge une sc�ne
    QVector<QSP<CComponent> > load(const QString& sBaseFile, C3DScene* pScene);

    //! Charge un composant
    CComponent* loadComponent(const QString& sBaseFile, C3DScene* pScene);

    //! Charge un composant
    CComponent* loadComponent(const QString& sBaseFile, C3DScene* pScene, CXMLNode xComponent, CComponent* pParent);
};
