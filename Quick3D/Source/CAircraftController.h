
#pragma once

#include "quick3d_global.h"

// Qt
#include <QKeyEvent>
#include <QMouseEvent>

// Application
#include "CQ3DConstants.h"
#include "CStandardController.h"
#include "CComponentReference.h"
#include "CWing.h"
#include "CElevator.h"
#include "CRudder.h"
#include "CEngine.h"

//-------------------------------------------------------------------------------------------------

class C3DScene;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CAircraftController : public CStandardController
{

public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	static CComponent* instanciator(C3DScene* pScene);

	//!
	CAircraftController(C3DScene* pScene);

	//!
	virtual ~CAircraftController();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//! Retourne le nom de classe de l'objet
	virtual QString getClassName() const { return ClassName_CAircraftController; }

	//! Charge les param�tres de cet objet
	virtual void loadParameters(CXMLNode xComponent);

	//! Recherche les liens de cet objet
	virtual void solveLinks(C3DScene* pScene);

	//!
	virtual void update(double dDeltaTime);

	//!
	virtual void keyPressEvent(QKeyEvent* event);

	//!
	virtual void keyReleaseEvent(QKeyEvent *event);

	//!
	virtual void q3dEvent(CQ3DEvent* event);

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	bool							m_bAileronLeft;
	bool							m_bAileronRight;
	bool							m_bNoseUp;
	bool							m_bNoseDown;
	bool							m_bRudderLeft;
	bool							m_bRudderRight;

	CComponentReference<CWing>		m_rLeftWingTarget;
	CComponentReference<CWing>		m_rRightWingTarget;
	CComponentReference<CElevator>	m_rElevatorTarget;
	CComponentReference<CRudder>	m_rRudderTarget;
	CComponentReference<CEngine>	m_rEngine1Target;
	CComponentReference<CEngine>	m_rEngine2Target;
};