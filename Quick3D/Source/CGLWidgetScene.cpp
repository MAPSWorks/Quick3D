
// Application
#include "CLogManager.h"
#include "CVector3.h"
#include "CGLWidgetScene.h"
#include "CRessourcesManager.h"
#include "CTreeGenerator.h"
#include "CBuildingGenerator.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

#define ATMOSPHERE_ALTITUDE 100000.0

//-------------------------------------------------------------------------------------------------

CGLWidgetScene::CGLWidgetScene(bool bForDisplay)
: C3DScene(bForDisplay)
{
	LOG_DEBUG("CGLWidgetScene::CGLWidgetScene()");

	makeCurrentRenderingContext();

	if (m_bForDisplay)
	{
		m_pGLExtension = new CGLExtension();
		m_pRessourcesManager = new CRessourcesManager(this);
		m_pBuildingGenerator = new CBuildingGenerator(this);
		m_pTreeGenerator = new CTreeGenerator(this);
		m_vShaders = new CShaderCollection();
	}
}

//-------------------------------------------------------------------------------------------------

CGLWidgetScene::~CGLWidgetScene()
{
	LOG_DEBUG("CGLWidgetScene::~CGLWidgetScene()");
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::paintGL()
{
	if (m_bForDisplay)
	{
		m_iNumMeshesDrawn = 0;
		m_iNumPolysDrawn = 0;
		m_iNumChunksDrawn = 0;

		makeCurrentRenderingContext();

		//-------------------------------------------------------------------------------------------------
		// Clear frame buffer

		if (m_bDepthComputing)
		{
			glClearColor(1.0, 1.0, 1.0, 1.0);
		}
		else
		{
			glClearColor(m_tFog.color().X, m_tFog.color().Y, m_tFog.color().Z, 1.0);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-------------------------------------------------------------------------------------------------
		// Compute world origin

		if (m_pViewports.count() > 0 && m_pViewports[0]->getCamera() != NULL)
		{
			m_WorldOrigin = m_pViewports[0]->getCamera()->getWorldPosition();

			m_WorldOrigin.X = m_WorldOrigin.X - fmod(m_WorldOrigin.X, 1000.0);
			m_WorldOrigin.Y = m_WorldOrigin.Y - fmod(m_WorldOrigin.Y, 1000.0);
			m_WorldOrigin.Z = m_WorldOrigin.Z - fmod(m_WorldOrigin.Z, 1000.0);
		}

		//-------------------------------------------------------------------------------------------------
		// Render cameras

		foreach (int iIndex, m_pViewports.keys())
		{
			if (m_pViewports[iIndex]->isEnabled() && m_pViewports[iIndex]->getCamera() != NULL)
			{
				m_pViewports[iIndex]->getCamera()->render(this, m_pViewports[iIndex], m_bforceWideFOV, m_bforceSmallFOV, m_bForceIR);
			}
		}

		//-------------------------------------------------------------------------------------------------
		// R�cup�ration du frame buffer

		bool bGrabFrame = false;

		foreach (int iIndex, m_pViewports.keys())
		{
			if (m_pViewports[iIndex]->getStreamView() || m_pViewports[iIndex]->getNeedFrameBuffer())
			{
				bGrabFrame = true;
				break;
			}
		}

		if (bGrabFrame)
		{
			m_imgFrameBuffer = grabFrameBuffer();
		}
	}
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::init(QVector<CComponent*> vComponents)
{
	LOG_DEBUG("CGLWidgetScene::init()");

	if (m_bForDisplay)
	{
		makeCurrentRenderingContext();

		initShaders();
	}

	C3DScene::init(vComponents);
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::initShaders()
{
	LOG_DEBUG("CGLWidgetScene::initShaders()");

	m_vShaders->clear();

	if (m_bForDisplay)
	{
		makeCurrentRenderingContext();

		QGLShaderProgram* pProgram;

		setlocale(LC_NUMERIC, "C");

		//-----------------------------------------------
		// Standard

		pProgram = new QGLShaderProgram();

		pProgram->addShaderFromSourceCode(QGLShader::Vertex, getRessourcesManager()->getShaderByFilePathName(":/Resources/Shaders/VS_Standard.c"));
		pProgram->addShaderFromSourceCode(QGLShader::Fragment, getRessourcesManager()->getShaderByFilePathName(":/Resources/Shaders/FS_Standard.c"));
		pProgram->link();

		m_vShaders->addShader(SP_Standard, pProgram);

		//-----------------------------------------------

		setlocale(LC_ALL, "");
	}

	C3DScene::initShaders();
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem)
{
	if (m_bForDisplay)
	{
		// Ici on transf�re tous les param�tres de rendu au shader actif

		pProgram->setUniformValue("u_resolution", QVector2D(width(), height()));
		pProgram->setUniformValue("u_time", (GLfloat) m_dTime);
		pProgram->setUniformValue("u_shaderQuality", (GLfloat) m_dShaderQuality);
		pProgram->setUniformValue("u_rendering_shadows", (GLint) m_bRenderingShadows);

		CVector3 vCamTruePos = pContext->camera()->getWorldPosition();
		CVector3 vCamPos = pContext->camera()->getWorldPosition() - m_WorldOrigin;
		CVector3 vCamRot = pContext->camera()->getWorldRotation();
		CVector3 vWorldUp = pContext->camera()->getWorldPosition().Normalize();

		QMatrix4x4 mCameraMatrix;
		mCameraMatrix.setToIdentity();
		mCameraMatrix.rotate(Math::Angles::toDeg(vCamRot.Y), QVector3D(0, 1, 0));
		mCameraMatrix.rotate(Math::Angles::toDeg(vCamRot.X), QVector3D(1, 0, 0));
		mCameraMatrix.rotate(Math::Angles::toDeg(vCamRot.Z), QVector3D(0, 0, 1));

		QVector3D vFront = (QVector3D(0.0, 0.0, 1.0) * mCameraMatrix) - (QVector3D(0.0, 0.0, 0.0) * mCameraMatrix);
		QVector3D vUp = (QVector3D(0.0, 1.0, 0.0) * mCameraMatrix) - (QVector3D(0.0, 0.0, 0.0) * mCameraMatrix);

		pProgram->setUniformValue("u_depth_computing", m_bDepthComputing ? 1 : 0);

		pProgram->setUniformValue("u_camera_true_position", QVector3D(vCamTruePos.X, vCamTruePos.Y, vCamTruePos.Z));
		pProgram->setUniformValue("u_camera_position", QVector3D(vCamPos.X, vCamPos.Y, vCamPos.Z));
		pProgram->setUniformValue("u_camera_direction", QVector3D(vFront.x(), vFront.y(), vFront.z()));
		pProgram->setUniformValue("u_camera_up", QVector3D(vUp.x(), vUp.y(), vUp.z()));
		pProgram->setUniformValue("u_world_origin", QVector3D(m_WorldOrigin.X, m_WorldOrigin.Y, m_WorldOrigin.Z));
		pProgram->setUniformValue("u_world_up", QVector3D(vWorldUp.X, vWorldUp.Y, vWorldUp.Z));
		pProgram->setUniformValue("u_camera_altitude", (GLfloat) pContext->camera()->getGeoloc().Altitude);
		pProgram->setUniformValue("u_atmosphere_altitude", (GLfloat) ATMOSPHERE_ALTITUDE);

		// Lights
		// pProgram->setUniformValue("u_global_ambient", QVector3D(0.2, 0.2, 0.3));
		pProgram->setUniformValue("u_global_ambient", QVector3D(0.05, 0.05, 0.15));

		if (m_dShaderQuality >= 0.90)
		{
			pProgram->setUniformValue("u_shadow_enable", (GLint) (m_vLights.count() > 0 && m_vLights[0]->castShadows()));
		}
		else
		{
			pProgram->setUniformValue("u_shadow_enable", 0);
		}

		QVector3D u_light_position [8];
		QVector3D u_light_direction [8];
		QVector3D u_light_color [8];
		float u_light_distance [8];
		float u_light_spot_angle [8];

		int iOpenGLLightIndex = 0;

		for (int iLightIndex = 0; iLightIndex < m_vLights.count() && iOpenGLLightIndex < 8; iLightIndex++)
		{
			Vector4 vColor = m_vLights[iLightIndex]->getMaterial()->getDiffuse();

			if (vColor.X != 0.0 || vColor.Y != 0.0 || vColor.Z != 0.0)
			{
				CVector3 vWorldPosition = m_vLights[iLightIndex]->getWorldPosition() - m_WorldOrigin;
				CVector3 vWorldDirection = m_vLights[iLightIndex]->getWorldDirection();

				if (iLightIndex == 0)
				{
					vWorldDirection = CVector3();
				}

				u_light_position[iOpenGLLightIndex]		= QVector3D(vWorldPosition.X, vWorldPosition.Y, vWorldPosition.Z);
				u_light_direction[iOpenGLLightIndex]	= QVector3D(vWorldDirection.X, vWorldDirection.Y, vWorldDirection.Z);
				u_light_color[iOpenGLLightIndex]		= QVector3D(vColor.X, vColor.Y, vColor.Z);
				u_light_distance[iOpenGLLightIndex]		= (float) m_vLights[iLightIndex]->getDistance();
				u_light_spot_angle[iOpenGLLightIndex]	= (float) Math::Angles::toRad(m_vLights[iLightIndex]->getFOV());

				iOpenGLLightIndex++;
			}
		}

		pProgram->setUniformValue("u_num_lights", (GLint) iOpenGLLightIndex);
		pProgram->setUniformValueArray("u_light_position", u_light_position, 8);
		pProgram->setUniformValueArray("u_light_direction", u_light_direction, 8);
		pProgram->setUniformValueArray("u_light_color", u_light_color, 8);
		pProgram->setUniformValueArray("u_light_distance", (GLfloat*) u_light_distance, 8, 1);
		pProgram->setUniformValueArray("u_light_spot_angle", (GLfloat*) u_light_spot_angle, 8, 1);

		if (m_vLights.count() > 0 && m_vLights[0]->castShadows())
		{
			m_vLights[0]->getMaterial()->activateShadow(pContext, pProgram);
		}

		pProgram->setUniformValue("u_fog_enable", (GLint) m_tFog.enabled() ? 1 : 0);
		pProgram->setUniformValue("u_fog_distance", (GLfloat) m_tFog.distance());
		pProgram->setUniformValue("u_fog_color", QVector3D(m_tFog.color().X, m_tFog.color().Y, m_tFog.color().Z));
	}

	C3DScene::setupEnvironment(pContext, pProgram, bBackgroundItem);
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::setupLights(CRenderContext* pContext)
{
	m_tFog.color() = CVector3(0.2, 0.6, 1.0);

	double dNormalizedTime = fabs(((double) m_tTimeOfDay.secsTo(QTime(0, 0, 0)) /  86400.0));
	double dSunAngle = (dNormalizedTime * -360.0) + 180.0;

	CGeoloc gSunPosition(0.0, dSunAngle, 100000000.0);
	CVector3 vSunPosition = gSunPosition.toVector3();

	m_dSunIntensity = vSunPosition.Normalize().DotProduct(pContext->camera()->getWorldPosition().Normalize());
	m_dSunIntensity = (m_dSunIntensity + 1.0) * 0.5;

	if (pContext->camera()->getGeoloc().Altitude < 0.0)
	{
		double dSeaFactor = 1.0 - (fabs(pContext->camera()->getGeoloc().Altitude) / 1000.0);
		if (dSeaFactor < 0.1) dSeaFactor = 0.1;
		m_dSunIntensity *= dSeaFactor;
	}

	double dAtmosphereFactor = (ATMOSPHERE_ALTITUDE - pContext->camera()->getGeoloc().Altitude) / ATMOSPHERE_ALTITUDE;
	if (dAtmosphereFactor < 0.0) dAtmosphereFactor = 0.0;
	if (dAtmosphereFactor > 1.0) dAtmosphereFactor = 1.0;

	// Compute fog distance
	m_tFog.distance() = (1.0 - m_tFog.level()) * pContext->camera()->getMaxDistance();

	// Underwater fog
	if (pContext->camera()->getGeoloc().Altitude < 0.0)
	{
		m_tFog.distance() = 1000.0;
	}

	m_dSunIntensity = Math::Angles::_max(m_dSunIntensity, 1.0 - dAtmosphereFactor);

	if (m_vLights.count() > 0)
	{
		m_vLights[0]->setOriginPosition(vSunPosition);

		if (pContext->camera()->getGeoloc().Altitude >= 0.0)
		{
			m_vLights[0]->getMaterial()->getDiffuse() = m_vSunColor.getValue(m_dSunIntensity);
		}
		else
		{
			m_vLights[0]->getMaterial()->getDiffuse() = Vector4(0.00, 0.25, 0.50, 1.00);
		}
	}

	m_tFog.color() = m_tFog.color() * m_dSunIntensity;

	C3DScene::setupLights(pContext);
}

//-------------------------------------------------------------------------------------------------

void CGLWidgetScene::makeCurrentRenderingContext()
{
	makeCurrent();
}