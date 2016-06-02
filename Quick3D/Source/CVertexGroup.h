
#pragma once

// Qt
#include <QMap>

// Fondations
#include "CVector3.h"
#include "CXMLNode.h"

// Application
#include "CNamed.h"

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT CVertexGroup : public CNamed
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CVertexGroup();

	//!
	virtual ~CVertexGroup();

	//-------------------------------------------------------------------------------------------------
	// Setters
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	const QMap<int, double>& weights() const { return m_vWeights; }

	//!
	QMap<int, double>& weights() { return m_vWeights; }

	//!
	double vertexWeight(int iIndex) const;

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// M�thodes prot�g�s
	//-------------------------------------------------------------------------------------------------

	//!
	void clear() { m_vWeights.clear(); }

protected:

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	QMap<int, double>		m_vWeights;
};