
// Application
#include "Angles.h"
#include "CAxis.h"
#include "C3DScene.h"
#include "CPhysicalComponent.h"
#include "CAtmosphere.h"

//-------------------------------------------------------------------------------------------------

/*!
    \class CPhysicalComponent
    \brief The base class for all 3D entities that have computed physical properties, like velocity and collisions.
    \inmodule Quick3D
*/

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

/*!
    Instantiates a new CPhysicalComponent.
*/
CComponent* CPhysicalComponent::instanciator(C3DScene* pScene)
{
	return new CPhysicalComponent(pScene);
}

//-------------------------------------------------------------------------------------------------

/*!
    Constructs a CPhysicalComponent with its default parameters.
*/
CPhysicalComponent::CPhysicalComponent(C3DScene* pScene)
: CComponent(pScene)
, m_bPhysicsActive(true)
, m_bCollisionsActive(true)
, m_bOnGround(false)
, m_dDrag_norm(0.1)
, m_dAngularDrag_norm(0.1)
, m_dFriction_norm(0.1)
, m_dMass_kg(1.0)
, m_dStickToNOLL(0.0)
, m_eCollisionType(ctSphere)
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Destroys a CPhysicalComponent.
*/
CPhysicalComponent::~CPhysicalComponent()
{
}

//-------------------------------------------------------------------------------------------------

/*!
    Copies properties of \a target to this component.
*/
CPhysicalComponent& CPhysicalComponent::operator = (const CPhysicalComponent& target)
{
	CComponent::operator = (target);

	m_bPhysicsActive				= target.m_bPhysicsActive;
	m_bCollisionsActive				= target.m_bCollisionsActive;
	m_dDrag_norm					= target.m_dDrag_norm;
	m_dAngularDrag_norm				= target.m_dAngularDrag_norm;
	m_dMass_kg						= target.m_dMass_kg;
	m_vVelocity_ms					= target.m_vVelocity_ms;
	m_vAngularVelocity_rs			= target.m_vAngularVelocity_rs;
	m_vSummedForces_mss				= target.m_vSummedForces_mss;
	m_vSummedTorques_rss			= target.m_vSummedTorques_rss;
	m_eCollisionType				= target.m_eCollisionType;

	return *this;
}

//-------------------------------------------------------------------------------------------------

/*!
    Loads the properties of this component from a CXMLNode.
*/
void CPhysicalComponent::loadParameters(CXMLNode xComponent)
{
	CComponent::loadParameters(xComponent);

	// Propri�t�s physiques

	CXMLNode xPhysicsNode = xComponent.getNodeByTagName(ParamName_Physics);
	CXMLNode xCenterOfMassNode = xComponent.getNodeByTagName(ParamName_CenterOfMass);

	if (xPhysicsNode.isEmpty() == false)
	{
		if (xPhysicsNode.m_vAttributes["MassKG"].isEmpty() == false)
		{
			m_dMass_kg = xPhysicsNode.m_vAttributes["MassKG"].toDouble();
		}

		if (xPhysicsNode.m_vAttributes["Drag"].isEmpty() == false)
		{
			m_dDrag_norm = xPhysicsNode.m_vAttributes["Drag"].toDouble();
		}

		if (xPhysicsNode.m_vAttributes["AngularDrag"].isEmpty() == false)
		{
			m_dAngularDrag_norm = xPhysicsNode.m_vAttributes["AngularDrag"].toDouble();
		}

		if (xPhysicsNode.m_vAttributes["Friction"].isEmpty() == false)
		{
			m_dFriction_norm = xPhysicsNode.m_vAttributes["Friction"].toDouble();
		}

		if (xPhysicsNode.m_vAttributes["StickToNOLL"].isEmpty() == false)
		{
			m_dStickToNOLL = xPhysicsNode.m_vAttributes["StickToNOLL"].toDouble();
		}

		if (xPhysicsNode.m_vAttributes[ParamName_PhysicsActive].isEmpty() == false)
		{
			m_bPhysicsActive = (bool) xPhysicsNode.m_vAttributes[ParamName_PhysicsActive].toInt();
		}

		if (xPhysicsNode.m_vAttributes[ParamName_CollisionsActive].isEmpty() == false)
		{
			m_bCollisionsActive = (bool) xPhysicsNode.m_vAttributes[ParamName_CollisionsActive].toInt();
		}
	}

	if (xCenterOfMassNode.isEmpty() == false)
	{
		m_vCenterOfMass = CVector3(
			xCenterOfMassNode.m_vAttributes[ParamName_x].toDouble(),
			xCenterOfMassNode.m_vAttributes[ParamName_y].toDouble(),
			xCenterOfMassNode.m_vAttributes[ParamName_z].toDouble()
		);
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Removes this object from physical computations.
*/
void CPhysicalComponent::sleep()
{
	m_bPhysicsActive = false;
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds this object to physical computations.
*/
void CPhysicalComponent::wakeUp()
{
	m_bPhysicsActive = true;
}

//-------------------------------------------------------------------------------------------------

/*!
    If \a bEnabled is \c true, collisions for the component will be computed.
*/
void CPhysicalComponent::setCollisions(bool bEnabled)
{
	m_bCollisionsActive = bEnabled;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the total mass (including children) of the component.
*/
double CPhysicalComponent::getTotalMass_kg() const
{
	double dTotalMass_kg = m_dMass_kg;

	foreach (CComponent* pChild, m_vChildren)
	{
		CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(pChild);

		if (pPhysical != NULL)
		{
			dTotalMass_kg += pPhysical->getTotalMass_kg();
		}
	}

	return dTotalMass_kg;
}

//-------------------------------------------------------------------------------------------------

/*!
    Returns the euler angles of the velocity vector.
*/
CVector3 CPhysicalComponent::getVelocityVectorAngles() const
{
	return euleurAngles(m_vVelocity_ms);
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a local force to the component, relative to a position. It can also generate torque. \br\br
    \a vForce_kg is the force to apply. \br
    \a vPosition is the local position at which the force is applied.
*/
void CPhysicalComponent::addUncenteredLocalForce_kg(CVector3 vPosition, CVector3 vForce_kg)
{
	if (m_bPhysicsActive == true)
	{
		CVector3 vSavedForce_kg = vForce_kg;

		double dDistanceAttenuation = (1.0 / (1.0 + vPosition.getMagnitude())) * 2.3;
		vForce_kg *= dDistanceAttenuation;
		addLocalForce_kg(vForce_kg);

		double dDistanceMultiplier = vPosition.getMagnitude() * 0.001;

		CVector3 vForceNormalized = vSavedForce_kg.Normalize();

		CAxis aPositionAxis(euleurAngles(vPosition));

		double dXForce = aPositionAxis.Up.DotProduct(vForceNormalized);
		double dYForce = aPositionAxis.Right.DotProduct(vForceNormalized);

		CVector3 vForceOnAxis(dXForce, dYForce, 0.0);

		CAxis aForceAxis;

		aForceAxis = aForceAxis.transferTo(aPositionAxis);
		aForceAxis = aForceAxis.rotate(vForceOnAxis);
		aForceAxis = aForceAxis.transferFrom(aPositionAxis);

		addLocalTorque_kg(aForceAxis.euleurAngles() * (vSavedForce_kg.getMagnitude() * dDistanceMultiplier));
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a local force to the component. \br\br
    \a vForce_kg is the force to apply.
*/
void CPhysicalComponent::addLocalForce_kg(CVector3 vForce_kg)
{
	if (m_bPhysicsActive == true)
	{
		CVector3 vRotation = getOriginRotation();

		vForce_kg = CMatrix4().MakeRotation(CVector3(0.0, 0.0, vRotation.Z)) * vForce_kg;
		vForce_kg = CMatrix4().MakeRotation(CVector3(vRotation.X, 0.0, 0.0)) * vForce_kg;
		vForce_kg = CMatrix4().MakeRotation(CVector3(0.0, vRotation.Y, 0.0)) * vForce_kg;

		m_vSummedForces_mss = m_vSummedForces_mss + (vForce_kg / getTotalMass_kg()) * 5.0;
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a force to the component, expressed in the geocentric frame. \br\br
    \a vForce_kg is the force to apply.
*/
void CPhysicalComponent::addForce_kg(CVector3 vForce_kg)
{
	if (m_bPhysicsActive == true)
	{
		m_vSummedForces_mss = m_vSummedForces_mss + (vForce_kg / getTotalMass_kg()) * 5.0;
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a local torque to the component. \br\br
    \a vForce_kg is the torque to apply.
*/
void CPhysicalComponent::addLocalTorque_kg(CVector3 vForce_kg)
{
	if (m_bPhysicsActive == true)
	{
		m_vSummedTorques_rss = m_vSummedTorques_rss + (vForce_kg / getTotalMass_kg()) * 5.0;
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds a torque to the component, expressed in the geocentric frame. \br\br
    \a vForce_kg is the torque to apply.
*/
void CPhysicalComponent::addTorque_kg(CVector3 vForce_kg)
{
	if (m_bPhysicsActive == true)
	{
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Updates the component using \a dDeltaTimeS, which is the elapsed seconds since the last frame. \br\br
    If the component is not asleep, all physics are computed here, except collisions.
*/
void CPhysicalComponent::update(double dDeltaTimeS)
{
	CComponent::update(dDeltaTimeS);

	if (isRootObject())
	{
		if (m_pFields.count() > 0)
		{
			if (dDeltaTimeS > 0.0)
			{
				CVector3 vNewPosition = getOriginPosition();
				CVector3 vNewRotation = getOriginRotation();

				if (m_bPhysicsActive == true)
				{
					double dTotalMass_kg = getTotalMass_kg();
                    // double dAirForceFactor = CAtmosphere::getInstance()->getAirForceFactor(getGeoloc().Altitude);
					double dAirDragFactor = CAtmosphere::getInstance()->getAirDragFactor(getGeoloc().Altitude);

					// Ajout force de gravit�

					CVector3 vGravityForce = CVector3(0.0, -dTotalMass_kg * 2.0, 0.0);

					vGravityForce = CMatrix4().MakeInverseRotation(vNewRotation * -1.0) * vGravityForce;

					addUncenteredLocalForce_kg(m_vCenterOfMass, vGravityForce);

					// Ajout train�e

					double dVelocitySquared_ms = m_vVelocity_ms.getMagnitude();

					dVelocitySquared_ms = dVelocitySquared_ms * dVelocitySquared_ms;

					double dDrag = dVelocitySquared_ms * m_dDrag_norm * dAirDragFactor * dTotalMass_kg;

					CVector3 vDragForce = m_vVelocity_ms.Normalize() * -dDrag;

					addForce_kg(vDragForce);

					// Mise � jour v�locit� axiale selon accumulateurs de forces

					m_vVelocity_ms = m_vVelocity_ms + (m_vSummedForces_mss * dDeltaTimeS);

					// Mise � jour v�locit� angulaire selon accumulateurs de couple

					m_vAngularVelocity_rs = m_vAngularVelocity_rs + (m_vSummedTorques_rss * dDeltaTimeS);

					// Application train�e angulaire

					m_vAngularVelocity_rs = m_vAngularVelocity_rs - (m_vAngularVelocity_rs * ((m_dAngularDrag_norm * dDeltaTimeS) * dAirDragFactor));

					if (m_bOnGround)
					{
						m_vVelocity_ms = m_vVelocity_ms - (m_vVelocity_ms * (m_dFriction_norm * dDeltaTimeS));
						m_vAngularVelocity_rs = m_vAngularVelocity_rs - (m_vAngularVelocity_rs * (m_dFriction_norm * dDeltaTimeS));
					}

					// R�cup�ration r�f�rence NOLL (North-Oriented Local Level)

					CAxis aLocalAxis(getGeoloc().getNOLLAxis());

					// Rotation du corps selon son axe local

					CAxis aRotationAxis(vNewRotation);
					CAxis aVelocityAxis(vNewRotation);
					aVelocityAxis = aVelocityAxis.transferFrom(aRotationAxis);
					aVelocityAxis = aVelocityAxis.rotate(m_vAngularVelocity_rs * dDeltaTimeS);
					aVelocityAxis = aVelocityAxis.transferTo(aRotationAxis);
					vNewRotation = aVelocityAxis.euleurAngles();

					// Translation du corps

					vNewPosition += aLocalAxis.Right * m_vVelocity_ms.X * dDeltaTimeS;
					vNewPosition += aLocalAxis.Up * m_vVelocity_ms.Y * dDeltaTimeS;
					vNewPosition += aLocalAxis.Front * m_vVelocity_ms.Z * dDeltaTimeS;
				}

				// Gestion altitude

				CGeoloc gNewGeoloc(vNewPosition);

				double dBoundsYOffset = getBounds().minimum().Y;
				double dLowestAltitude = gNewGeoloc.Altitude + dBoundsYOffset;

				m_bOnGround = false;

				foreach (CHeightField* pField, m_pFields)
				{
					double dHeight = pField->getHeightAt(gNewGeoloc);

					// Est-ce qu'il y a du sol sous l'objet?
					if (!(fabs(dHeight - Q3D_INFINITY) < 0.01))
					{
						// Est-ce que l'objet est proche du sol?
						if (fabs(dLowestAltitude - dHeight) < 0.2)
						{
							m_bOnGround = true;
						}

						// Est-ce que l'objet passe sous le sol?
						if (dLowestAltitude < dHeight)
						{
							gNewGeoloc.Altitude = dHeight - dBoundsYOffset;

							// Remise � z�ro de la vitesse verticale
							m_vVelocity_ms.Y = 0.0;

							m_bOnGround = true;
						}
						else
						{
							// Ici on est sous l'eau, on fait remonter l'objet l�g�rement
							if (gNewGeoloc.Altitude < 0.0)
							{
								double dUpRate = 0.0 - gNewGeoloc.Altitude;
								if (dUpRate > 3.0) dUpRate = 3.0;
								gNewGeoloc.Altitude += dUpRate * dDeltaTimeS;
							}
						}
					}
				}

				// La position de l'objet est mise � jour si sa vitesse est sup�rieure � 1cm/s
				// Ou s'il a travers� le sol
				if (m_vVelocity_ms.getMagnitude() > 0.01 || m_bOnGround)
				{
					setGeoloc(gNewGeoloc);
				}

				setOriginRotation(vNewRotation);
			}
		}
	}
	else
	{
		if (m_dStickToNOLL > 0.0)
		{
			if (m_pParent != NULL)
			{
				CMatrix4 mParentPrevious = m_pParent->getPreviousWorldTransform();

				if (mParentPrevious.isIdentity() == false)
				{
					CAxis anAxis(getOriginRotation());

					anAxis = anAxis * m_pParent->getPreviousWorldTransform();
					anAxis = anAxis * m_pParent->getWorldTransformInverse();

					setOriginRotation(anAxis.euleurAngles());

					// Vector3 vNewAngles = anAxis.euleurAngles();
					// setOriginRotation(interpolate(vNewAngles, m_vOriginRotation, m_dStickToNOLL * dDeltaTime));
				}
			}
		}
	}

	// Remise � z�ro des accumulateurs de forces

	m_vSummedForces_mss = CVector3();
	m_vSummedTorques_rss = CVector3();
}

//-------------------------------------------------------------------------------------------------

/*!
    Does post update work on the component using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CPhysicalComponent::postUpdate(double dDeltaTimeS)
{
	CComponent::postUpdate(dDeltaTimeS);

	setPreviousWorldTransform(getWorldTransform());
}

//-------------------------------------------------------------------------------------------------

/*!
    Computes collisions for the components in \a vComponents using \a dDeltaTimeS, which is the elapsed seconds since the last frame.
*/
void CPhysicalComponent::computeCollisions(QVector<QSharedPointer<CComponent> >& vComponents, double dDeltaTimeS)
{
	foreach (QSharedPointer<CComponent> pComponent, vComponents)
	{
		CPhysicalComponent* pPhysical = dynamic_cast<CPhysicalComponent*>(pComponent.data());

		// Pour l'instant les terrains sont ignor�s car trait�s dans update
		if (pPhysical != NULL && pPhysical->getClassName() != ClassName_CAutoTerrain)
		{
			if (pPhysical->isRootObject() && pPhysical->getCollisionsActive() == true)
			{
				// On ne calcule de collision que si l'objet a une vitesse non nulle
				if (pPhysical->getVelocity_ms().getMagnitude() > 0.0)
				{
					computeCollisionsForComponent(pPhysical, vComponents, dDeltaTimeS);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Computes collisions for \a pComponent using \a dDeltaTimeS, which is the elapsed seconds since the last frame. \br\br
    \a vOtherComponents is the list of components which it can interact with.
*/
void CPhysicalComponent::computeCollisionsForComponent(CPhysicalComponent* pComponent, QVector<QSharedPointer<CComponent> >& vOtherComponents, double dDeltaTimeS)
{
	// On parcourt chacun des autres composants
	foreach (QSharedPointer<CComponent> pOtherComponent, vOtherComponents)
	{
		CPhysicalComponent* pOtherPhysical = dynamic_cast<CPhysicalComponent*>(pOtherComponent.data());

		if (pOtherPhysical != NULL)
		{
			if (pComponent != pOtherPhysical && pOtherPhysical->isRootObject() && pOtherPhysical->getCollisionsActive() == true)
			{
				// Pour l'instant les terrains sont ignor�s car trait�s dans update
				if (pOtherPhysical->getClassName() != ClassName_CAutoTerrain)
				{
					// Calcul de distance avec l'autre objet
					CVector3 vPosition = pComponent->getGeoloc().toVector3(pOtherPhysical->getGeoloc());
					double dRadiusSum = pComponent->getWorldBounds().radius() + pOtherPhysical->getWorldBounds().radius();

					// Est-ce que les deux sph�res se recoupent?
					if (vPosition.getMagnitude() < dRadiusSum)
					{
						// Si oui, on fait rebondir l'objet
						double dForces = pComponent->m_vVelocity_ms.getMagnitude();
						pComponent->m_vVelocity_ms += vPosition.Normalize() * dForces * 0.5;
					}
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Adds the the height field specified by \a value to the list of height fields that this component must take into account.
*/
void CPhysicalComponent::addField(CHeightField* value)
{
	if (m_pFields.contains(value) == false)
	{
		m_pFields.append(value);
	}
}

//-------------------------------------------------------------------------------------------------

/*!
    Dumps this component to \a stream using the indentation value in \a iIdent.
*/
void CPhysicalComponent::dump(QTextStream& stream, int iIdent)
{
	dumpIdent(stream, iIdent, QString("[CPhysicalComponent]"));
	dumpIdent(stream, iIdent, QString("Physics active : %1").arg(m_bPhysicsActive));
	dumpIdent(stream, iIdent, QString("Stick to NOLL : %1").arg(m_dStickToNOLL));
	dumpIdent(stream, iIdent, QString("Drag : %1").arg(m_dDrag_norm));
	dumpIdent(stream, iIdent, QString("Angular drag : %1").arg(m_dAngularDrag_norm));
	dumpIdent(stream, iIdent, QString("Mass : %1").arg(m_dMass_kg));
	dumpIdent(stream, iIdent, QString("Velocity : %1").arg(m_vVelocity_ms.toString()));
	dumpIdent(stream, iIdent, QString("Angular velocity : %1").arg(m_vAngularVelocity_rs.toString()));

	CComponent::dump(stream, iIdent);
}