
// Application
#include "CLogManager.h"
#include "CWorkerManager.h"
#include "CRessourcesManager.h"
#include "CTerrain.h"
#include "CWaterMaterial.h"
#include "CTiledMaterial.h"
#include "C3DScene.h"

using namespace Math;

//-------------------------------------------------------------------------------------------------

int CTerrain::m_iNumTerrains = 0;

Math::Interpolator<double> CTerrain::m_iAltitudes_Sand;
Math::Interpolator<double> CTerrain::m_iAltitudes_Dirt;
Math::Interpolator<double> CTerrain::m_iAltitudes_Grass;
Math::Interpolator<double> CTerrain::m_iAltitudes_Snow;

//-------------------------------------------------------------------------------------------------

CTerrain::CTerrain(
	C3DScene* pScene,
	CHeightField* pHeights,
	CGeoloc gOriginalGeoloc,
	CGeoloc gOriginalSize,
	CGeoloc gGeoloc,
	CGeoloc gSize,
	int iPoints,
	int iLevel,
	int iMaxLevel,
	bool bIsWater,
	bool bGenerateNow
	)
: CComponent(pScene)
, CHeightField(1.0)
, m_pHeights(pHeights)
, m_gOriginalGeoloc(gOriginalGeoloc)
, m_gOriginalSize(gOriginalSize)
, m_gSize(gSize)
, m_iNumPoints(iPoints)
, m_iLevel(iLevel)
, m_iMaxLevel(iMaxLevel)
, m_bAllHeightsOverSea(false)
, m_bIsWater(bIsWater)
, m_bOK(false)
{
	QString sGeolocName = QString("_%1_%2").arg(m_gOriginalGeoloc.Latitude).arg(m_gOriginalGeoloc.Longitude);

	setName(QString("Terrain%1").arg(sGeolocName));

	m_iNumTerrains++;

	if (m_iNumPoints % 2 == 0) m_iNumPoints++;

	if (m_iAltitudes_Sand.count() == 0)
	{
		m_iAltitudes_Sand.addValue( 0.0, 1.0);
		m_iAltitudes_Sand.addValue(10.0, 0.0);

		m_iAltitudes_Dirt.addValue(10.0, 0.0);
		m_iAltitudes_Dirt.addValue(50.0, 1.0);

		m_iAltitudes_Grass.addValue(50.0, 0.0);
		m_iAltitudes_Grass.addValue(100.0, 1.0);
		m_iAltitudes_Grass.addValue(500.0, 1.0);
		m_iAltitudes_Grass.addValue(600.0, 0.0);

		m_iAltitudes_Snow.addValue(1000.0, 0.0);
		m_iAltitudes_Snow.addValue(1200.0, 1.0);
	}

	setGeoloc(gGeoloc);
	computeWorldTransform();

	// Create terrain mesh
	m_pMesh = new CMesh(pScene, 100000000.0, true);
	m_pMesh->setName(QString("TerrainMesh%1").arg(sGeolocName));

	if (bIsWater == false)
	{
		/*
		// For tests
		m_pMesh->setMaterial(new CMaterial(pScene));
		CPerlin* pPerlin = CPerlin::getInstance();
		Vector3 vPosition = gCenter.toVector3();
		m_pMesh->getMaterial()->material()->getDiffuse() = Vector4(pPerlin->getNoise_0_1(vPosition * 1.1), pPerlin->getNoise_0_1(vPosition * 1.2), pPerlin->getNoise_0_1(vPosition * 1.3), 1.0);
		*/
	}
	else
	{
		// Delete default material and create a water material
		m_pMesh->setMaterial(QSharedPointer<CWaterMaterial>(new CWaterMaterial(pScene)));

		// Set material IR factor
		m_pMesh->getMaterials()[0]->setIRFactor(0.2);
	}

	if (bGenerateNow)
	{
		work();
	}
	else
	{
		// Add this to workers for mesh generation
		CWorkerManager::getInstance()->addWorker(this);
	}
}

//-------------------------------------------------------------------------------------------------

CTerrain::~CTerrain()
{
	m_iNumTerrains--;

	// Remove this from workers
	CWorkerManager::getInstance()->removeWorker(this);

	delete m_pMesh;

	foreach (CMesh* pSeam, m_vSeams)
	{
		delete pSeam;
	}
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CTerrain::getBounds() const
{
	if (m_pMesh)
	{
		return m_pMesh->getBounds();
	}

	return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

CBoundingBox CTerrain::getWorldBounds() const
{
	if (m_pMesh)
	{
		return m_pMesh->getWorldBounds();
	}

	return CBoundingBox();
}

//-------------------------------------------------------------------------------------------------

void CTerrain::setMaterial(QSharedPointer<CMaterial> pMaterial)
{
	m_pMesh->setMaterial(pMaterial);
}

//-------------------------------------------------------------------------------------------------

int CTerrain::getPointIndexForXZ (int iX, int iZ) const
{
	return (iZ * m_iNumPoints) + iX;
}

//-------------------------------------------------------------------------------------------------

int CTerrain::getFaceIndexForVertices(int v1, int v2, int v3, int v4) const
{
	if (m_pMesh != NULL)
	{
		for (int iIndex = 0; iIndex < m_pMesh->getFaces().count(); iIndex++)
		{
			if (m_pMesh->getFaces()[iIndex].getIndices().count() == 4)
			{
				if (
					m_pMesh->getFaces()[iIndex].getIndices()[0] == v1 &&
					m_pMesh->getFaces()[iIndex].getIndices()[1] == v2 &&
					m_pMesh->getFaces()[iIndex].getIndices()[2] == v3 &&
					m_pMesh->getFaces()[iIndex].getIndices()[3] == v4
					)
				{
					return iIndex;
				}
			}
		}
	}

	return -1;
}

//-------------------------------------------------------------------------------------------------

void CTerrain::paint(CRenderContext* pContext)
{
	m_pMesh->paint(pContext);
}

//-------------------------------------------------------------------------------------------------

double CTerrain::getHeightAt(const Math::CGeoloc& gPosition, double* pRigidness)
{
	// Using terrain parameters
	// CPerlin* pPerlin = CPerlin::getInstance();
	// return m_pFunction->process(pPerlin, vPosition);

	if (pRigidness)
	{
		*pRigidness = 1.0;
	}

	if (m_bOK == false)
	{
		return Q3D_INFINITY;
	}

	if (m_bIsWater)
	{
		if (
			m_pMesh != NULL && m_pMesh->getMaterials().count() > 0 &&
			dynamic_cast<CHeightField*>(m_pMesh->getMaterials()[0].data()) != NULL
			)
		{
			CHeightField* pField = dynamic_cast<CHeightField*>(m_pMesh->getMaterials()[0].data());
			return pField->getHeightAt(gPosition, pRigidness);
		}

		return 0.0;
	}

	double dLatitudeDiff = Math::Angles::angleDifferenceDegree(gPosition.Latitude, getGeoloc().Latitude);
	double dLongitudeDiff = Math::Angles::angleDifferenceDegree(gPosition.Longitude, getGeoloc().Longitude);

	CGeoloc gLocalGeoloc(dLatitudeDiff, dLongitudeDiff, 0.0);
	CVector3 vCenter = getGeoloc().toVector3();
	CVector3 vLocal = gPosition.toVector3() - vCenter;
	CVector3 vUp = vCenter.Normalize();

	int iX = (int) (((gLocalGeoloc.Longitude / (m_gSize.Longitude)) + 0.5) * (m_iNumPoints - 1));
	int iZ = (int) (((gLocalGeoloc.Latitude / (m_gSize.Latitude)) + 0.5) * (m_iNumPoints - 1));

	for (int iIterZ = iZ - 2; iIterZ <= iZ + 2; iIterZ++)
	{
		for (int iIterX = iX - 2; iIterX <= iX + 2; iIterX++)
		{
			int iIndex1 = getPointIndexForXZ(iIterX + 0, iIterZ + 0);
			int iIndex2 = getPointIndexForXZ(iIterX + 1, iIterZ + 0);
			int iIndex3 = getPointIndexForXZ(iIterX + 1, iIterZ + 1);
			int iIndex4 = getPointIndexForXZ(iIterX + 0, iIterZ + 1);

			if (
				/*
				// Skirt
				iIndex1 >= 1 && iIndex2 >= 1 && iIndex3 >= 1 && iIndex4 >= 1 &&
				iIndex1 < m_pMesh->getVertices().count() - 1 &&
				iIndex2 < m_pMesh->getVertices().count() - 1 &&
				iIndex3 < m_pMesh->getVertices().count() - 1 &&
				iIndex4 < m_pMesh->getVertices().count() - 1
				*/
				// No skirt
				iIndex1 >= 0 && iIndex2 >= 0 && iIndex3 >= 0 && iIndex4 >= 0 &&
				iIndex1 < m_pMesh->getVertices().count() &&
				iIndex2 < m_pMesh->getVertices().count() &&
				iIndex3 < m_pMesh->getVertices().count() &&
				iIndex4 < m_pMesh->getVertices().count()
				)
			{
				CVector3 v1(m_pMesh->getVertices()[iIndex1].position());
				CVector3 v2(m_pMesh->getVertices()[iIndex2].position());
				CVector3 v3(m_pMesh->getVertices()[iIndex3].position());
				CVector3 v4(m_pMesh->getVertices()[iIndex4].position());

				CRay3 ray(vLocal + vUp * 50000.0, vUp * -1.0);

				RayTracingResult dResult(0.0);

				int iFaceIndex = getFaceIndexForVertices(iIndex1, iIndex2, iIndex3, iIndex4);

				if (iFaceIndex != -1)
				{
					dResult = CFace::intersectTriangle(
						ray,
						v1, v2, v3
						);

					if (dResult.m_dDistance < Q3D_INFINITY)
					{
						return CGeoloc(vCenter + ray.vOrigin + ray.vNormal * dResult.m_dDistance).Altitude;
					}

					dResult = CFace::intersectTriangle(
						ray,
						v1, v3, v4
						);

					if (dResult.m_dDistance < Q3D_INFINITY)
					{
						return CGeoloc(vCenter + ray.vOrigin + ray.vNormal * dResult.m_dDistance).Altitude;
					}
				}
			}
		}
	}

	return Q3D_INFINITY;
}

//-------------------------------------------------------------------------------------------------

void CTerrain::work()
{
	LOG_DEBUG(QString("CTerrain::work() : START : %1").arg(m_sName));

	m_bAllHeightsOverSea = true;
	int vVertexCount = 0;

	CPerlin* pPerlin = CPerlin::getInstance();

	// Patch initial
	if (m_bIsWater == false && m_iLevel < m_iMaxLevel / 2)
	{
		// Skirt
		// m_pMesh->createQuadPatch(m_iNumPoints, 1);
		// m_iNumPoints += 2;
		m_pMesh->createQuadPatch(m_iNumPoints, 0);
	}
	else
	{
		m_pMesh->createQuadPatch(m_iNumPoints, 0);
	}

	CVector3 vFinalCenter = getGeoloc().toVector3();

	// R�cup�ration du mat�riau du mesh
	CMaterial* pMaterial = NULL;

	if (m_pMesh->getMaterials().count() > 0)
	{
		pMaterial = m_pMesh->getMaterials()[0].data();
	}

	CTiledMaterial* pTiledMaterial = dynamic_cast<CTiledMaterial*>(pMaterial);

	// Move vertices
	for (int iIndex = 0; iIndex < m_pMesh->getVertices().count(); iIndex++)
	{
		CGeoloc gPosition;
		CVector3 vPosition = m_pMesh->getVertices()[iIndex].position();

		gPosition.Latitude = m_gOriginalGeoloc.Latitude + vPosition.Z * m_gOriginalSize.Latitude;
		gPosition.Longitude = m_gOriginalGeoloc.Longitude + vPosition.X * m_gOriginalSize.Longitude;

		if (pMaterial != NULL)
		{
			gPosition = pMaterial->transformGeoloc(gPosition);
		}

		/*
		// Skirt
		if (m_bIsWater == false && m_iLevel < m_iMaxLevel / 2)
		{
			if (
				gPosition.Latitude < m_gGeoloc.Latitude - m_gSize.Latitude * 0.5 ||
				gPosition.Latitude > m_gGeoloc.Latitude + m_gSize.Latitude * 0.5 ||
				gPosition.Longitude < m_gGeoloc.Longitude - m_gSize.Longitude * 0.5 ||
				gPosition.Longitude > m_gGeoloc.Longitude + m_gSize.Longitude * 0.5
				)
			{
				gPosition.Altitude = -m_gSize.Longitude * 100.0;
			}
		}
		*/

		CAxis aNOLLAxis = gPosition.getNOLLAxis();

		m_pMesh->getVertices()[iIndex].position() = gPosition.toVector3();
		m_pMesh->getVertices()[iIndex].normal() = m_pMesh->getVertices()[iIndex].position().Normalize();
		m_pMesh->getVertices()[iIndex].tangent() = aNOLLAxis.Front;
		m_pMesh->getVertices()[iIndex].gravity() = m_pMesh->getVertices()[iIndex].normal() * -1.0;

		if (pMaterial != NULL && pTiledMaterial == NULL)
		{
			m_pMesh->getVertices()[iIndex].texCoord() = pMaterial->getTexCoords(gPosition, m_iLevel);
		}

		if (m_bStopRequested)
		{
			return;
		}
	}

	// Loop over vertices
	for (int iIndex = 0; iIndex < m_pMesh->getVertices().count(); iIndex++)
	{
		double dTerrainAltitude = 0.0;

		if (m_pHeights != NULL)
		{
			// Ce test est important : avec les terrains non g�n�r�s, on veut �viter de charger trop de donn�es MNT en RAM
			// Donc on ne r�cup�re une altitude que pour les niveaux proches de la mer (x < niveau max / 2)
			if (m_pHeights->isGenerated() || m_iLevel < m_iMaxLevel / 2)
			{
				dTerrainAltitude = m_pHeights->getHeightAt(
					m_pMesh->getVertices()[iIndex].position(),
					CAxis(m_pMesh->getVertices()[iIndex].tangent(), m_pMesh->getVertices()[iIndex].normal()),
					false
					);
			}
			else
			{
				dTerrainAltitude = 0.0;
			}

			if (fabs(dTerrainAltitude - Q3D_INFINITY) < 0.01)
			{
				dTerrainAltitude = 0.0;
			}

			if (dTerrainAltitude < -20000.0)
			{
				dTerrainAltitude = -20000.0;
			}

			if (dTerrainAltitude < 0.0)
			{
				m_bAllHeightsOverSea = false;
			}

			m_pMesh->getVertices()[iIndex].altitude() = dTerrainAltitude;
		}

		// If this is a water mesh, altitude is always 0
		if (m_bIsWater == true)
		{
			dTerrainAltitude = 0.0;
		}

		// Translate vertex to final position
		m_pMesh->getVertices()[iIndex].position() =
			(m_pMesh->getVertices()[iIndex].position() - vFinalCenter) +
			(m_pMesh->getVertices()[iIndex].gravity() * -1.0) * dTerrainAltitude;

		vVertexCount++;

		if (m_bStopRequested)
		{
			return;
		}
	}

	m_pMesh->computeNormals();

	vVertexCount = 0;

	// Loop over vertices
	if (pMaterial != NULL && pTiledMaterial == NULL)
	{
		LOG_DEBUG(QString("CTerrain::work() : Setting up terrain textures for non-tiled material"));

		for (int iIndex = 0; iIndex < m_pMesh->getVertices().count(); iIndex++)
		{
			if (m_bIsWater == false)
			{
				double dSlope = fabs(m_pMesh->getVertices()[iIndex].normal().DotProduct(
					m_pMesh->getVertices()[iIndex].gravity() * -1.0
					));

				dSlope = pow(dSlope, 4.0);

				double dStoneMix = pPerlin->getNoise_0_1((vFinalCenter + m_pMesh->getVertices()[iIndex].position()) * 0.2);
				double dDirtMix = pPerlin->getNoise_0_1((vFinalCenter + m_pMesh->getVertices()[iIndex].position()) * 0.3);
				double dGrassMix = pPerlin->getNoise_0_1((vFinalCenter + m_pMesh->getVertices()[iIndex].position()) * 0.1);

				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(0, 1.0);

				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(0, dStoneMix);
				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(1, 1.0 - dStoneMix);

				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(2, m_iAltitudes_Dirt.getValue(m_pMesh->getVertices()[iIndex].altitude()) * dSlope * dDirtMix);
				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(3, m_iAltitudes_Dirt.getValue(m_pMesh->getVertices()[iIndex].altitude()) * dSlope * (1.0 - dDirtMix));

				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(4, m_iAltitudes_Sand.getValue(m_pMesh->getVertices()[iIndex].altitude()) * dSlope);
				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(5, m_iAltitudes_Snow.getValue(m_pMesh->getVertices()[iIndex].altitude()) * dSlope);

				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(6, m_iAltitudes_Grass.getValue(m_pMesh->getVertices()[iIndex].altitude()) * dSlope * dGrassMix);
				m_pMesh->getVertices()[iIndex].setDiffuseTextureWeight(7, m_iAltitudes_Grass.getValue(m_pMesh->getVertices()[iIndex].altitude()) * dSlope * (1.0 - dGrassMix));
			}

			vVertexCount++;

			if (m_bStopRequested)
			{
				return;
			}
		}

		// Remove underwater if terrain is large
		if (m_bIsWater == false && m_iLevel > m_iMaxLevel / 2)
		{
			for (int iFaceIndex = 0; iFaceIndex < m_pMesh->getFaces().count(); iFaceIndex++)
			{
				bool bIsOversea = false;

				for (int iVertexIndex = 0; iVertexIndex < m_pMesh->getFaces()[iFaceIndex].getIndices().count(); iVertexIndex++)
				{
					if (m_pMesh->getVertices()[m_pMesh->getFaces()[iFaceIndex].getIndices()[iVertexIndex]].altitude() > 0.0)
					{
						bIsOversea = true;
					}
				}

				if (bIsOversea == false)
				{
					m_pMesh->getFaces().remove(iFaceIndex);
					iFaceIndex--;
				}

				if (m_bStopRequested)
				{
					return;
				}
			}
		}
	}

	// Update mesh
	m_pMesh->updateGeometry(false);
	m_pMesh->setInheritTransform(false);
	m_pMesh->setOriginPosition(getOriginPosition());
	m_pMesh->computeWorldTransform();

	// Terrain is ready
	m_bOK = true;

	/*
	LOG_DEBUG(QString("CTerrain::work() : Terrain built, vertices = %1, faces = %2")
		.arg(m_pMesh->getVertices().count())
		.arg(m_pMesh->getFaces().count())
		);
		*/

	if (m_pMesh->getFaces().count() == 0)
	{
		LOG_WARNING(QString("CTerrain::work() : No faces in %1").arg(m_sName));
	}

	LOG_DEBUG(QString("CTerrain::work() : FINISHED : %1").arg(m_sName));
}

//-------------------------------------------------------------------------------------------------

void CTerrain::flatten(const CGeoloc& gPosition, double dRadius)
{
	/*
	Vector3 vLocal = vPosition - m_vCenter;

	for (int iIndex = 0; iIndex < m_vVertices.count(); iIndex++)
	{
		double dDistance = (vLocal - m_vVertices[iIndex].position()).getMagnitude();

		if (dDistance < dRadius)
		{
			double dFactor = 1.0 - (dDistance / dRadius);

			Vector3 vNormal = vPosition.Normalize();

			m_vVertices[iIndex].position().Y += (vPosition.Y - m_vVertices[iIndex].position().Y) * dFactor;
		}
	}

	// Update mesh
	updateGeometry(true);
	*/
}

//-------------------------------------------------------------------------------------------------

RayTracingResult CTerrain::intersect(Math::CRay3 ray) const
{
	if (m_pMesh != NULL)
	{
		return m_pMesh->intersect(ray);
	}

	return RayTracingResult(Q3D_INFINITY);
}

//-------------------------------------------------------------------------------------------------

void CTerrain::dump(QTextStream& stream, int iIdent)
{
	dumpIdent(stream, iIdent, QString("[CTerrain]"));
	dumpIdent(stream, iIdent, QString("Original geoloc : %1").arg(m_gOriginalGeoloc.toString()));
	dumpIdent(stream, iIdent, QString("Original size : %1").arg(m_gOriginalSize.toString()));
	dumpIdent(stream, iIdent, QString("Size : %1").arg(m_gSize.toString()));
	dumpIdent(stream, iIdent, QString("Num. points : %1").arg(m_iNumPoints));
	dumpIdent(stream, iIdent, QString("Level : %1").arg(m_iLevel));
	dumpIdent(stream, iIdent, QString("Max level : %1").arg(m_iMaxLevel));
	dumpIdent(stream, iIdent, QString("Mesh :"));

	dumpOpenBlock(stream, iIdent); iIdent++;
	if (m_pMesh) m_pMesh->dump(stream, iIdent);
	iIdent--; dumpCloseBlock(stream, iIdent);

	CComponent::dump(stream, iIdent);
}