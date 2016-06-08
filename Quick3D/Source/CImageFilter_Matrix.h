
#pragma once

#include "quick3d_global.h"

// Qt
#include <QGenericMatrix>

// Application
#include "CImageFilter.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CImageFilter_Matrix : public CImageFilter
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur d'apr�s une matrice 3x3
    CImageFilter_Matrix(const QMatrix3x3& mMatrix);

    //! Destructeur
    virtual ~CImageFilter_Matrix();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //! Retourne le nom de classe de cet objet
    virtual QString getClassName() const { return "CImageFilter_Matrix"; }

    //!
    virtual void process(QImage& imgImage);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

protected:

    QMatrix3x3	m_mMatrix;
};
