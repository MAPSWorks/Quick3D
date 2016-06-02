
#pragma once

#include "quick3d_global.h"

// Fondations
#include "CVector3.h"
#include "CMatrix4.h"
#include "CGeoloc.h"

class QUICK3D_EXPORT CBoundingBox
{
public:

	//-------------------------------------------------------------------------------------------------
	// Constructeurs et destructeur
	//-------------------------------------------------------------------------------------------------

	//!
	CBoundingBox();

	//!
	CBoundingBox(const CBoundingBox& target);

	//!
	CBoundingBox(Math::CVector3 vMinimum, Math::CVector3 vMaximum);

	//!
	virtual ~CBoundingBox();

	//!
	CBoundingBox& operator = (const CBoundingBox& target);

	//-------------------------------------------------------------------------------------------------
	// Getters
	//-------------------------------------------------------------------------------------------------

	//!
	Math::CVector3 minimum() const { return m_vMinimum; }

	//!
	Math::CVector3 maximum() const { return m_vMaximum; }

	//!
	Math::CVector3& minimum() { return m_vMinimum; }

	//!
	Math::CVector3& maximum() { return m_vMaximum; }

	//!
	Math::CVector3 center() const { return (m_vMinimum + m_vMaximum) * 0.5; }

	//!
	Math::CVector3 halfSizes() const { return (m_vMaximum - m_vMinimum) * 0.5; }

	//!
	double radius() const { return (m_vMaximum - m_vMinimum).getMagnitude() * 0.5; }

	//!
	void expand(Math::CVector3 vAmount);

	//-------------------------------------------------------------------------------------------------
	// M�thodes h�rit�es
	//-------------------------------------------------------------------------------------------------

	//! Calcul d'intersection avec un rayon
	virtual Math::RayTracingResult intersect(Math::CRay3 ray) const;

	//-------------------------------------------------------------------------------------------------
	// M�thodes de contr�le
	//-------------------------------------------------------------------------------------------------

	//!
	void prepare();

	//!
	bool contains(Math::CVector3 vPosition) const;

	//!
	bool contains(Math::CGeoloc gPosition) const;

	//!
	bool containsSpherical(Math::CGeoloc gPosition) const;

	//!
	bool contains(Math::CVector3 vPosition, double dRadius) const;

	//!
	bool contains(Math::CGeoloc gPosition, double dRadius) const;

	//!
	bool intersects(const CBoundingBox& target) const;

	//!
	void scale(double dFactor);

	//!
	CBoundingBox translated(Math::CVector3 vAmount) const;

	//!
	CBoundingBox rotated(Math::CVector3 vAmount) const;

	//!
	CBoundingBox transformed(const Math::CMatrix4& mTransform) const;

	//!
	CBoundingBox operator & (const CBoundingBox& target) const;

	//-------------------------------------------------------------------------------------------------
	// Propri�t�s
	//-------------------------------------------------------------------------------------------------

protected:

	Math::CVector3	m_vMinimum;
	Math::CVector3	m_vMaximum;
};