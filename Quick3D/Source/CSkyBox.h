
#pragma once

#include "quick3d_global.h"

// Application
#include "CQ3DConstants.h"
#include "CMesh.h"

class C3DScene;

class QUICK3D_EXPORT CSkyBox : public CMesh
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CSkyBox(C3DScene* pScene);

	//!
	virtual ~CSkyBox();

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de classe de cet objet
	virtual QString getClassName() const { return ClassName_CSkyBox; }

	//!
	virtual void update(double dDeltaTime);

	//!
	virtual void paint(CRenderContext* pContext);

	//!
	virtual void loadParameters(CXMLNode xComponent);

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);
};