
#include "CWaterMaterial.h"
#include "C3DScene.h"
#include "CPerlin.h"

using namespace Math;

CWaterMaterial::CWaterMaterial(C3DScene* pScene)
: CMaterial(pScene)
{
	m_cDiffuse = Vector4(0.05, 0.20, 0.10, 1.0);
	m_cSpecular = Vector4(0.9, 0.9, 0.9, 1.0);
	m_dShininess = 50.0;
	m_dReflection = 0.8;
	m_dReflectionSteepness = 4.0;
	m_dIRFactor = 0.2;
	m_dRigidness = 0.75;
}

CWaterMaterial::~CWaterMaterial()
{
}

QGLShaderProgram* CWaterMaterial::activate(CRenderContext* pContext)
{
	QGLShaderProgram* pProgram = CMaterial::activate(pContext);

	if (pProgram != NULL)
	{
		double dAmplitude = m_pScene->getWindLevel() * 4.0;

		pProgram->setUniformValue("u_wave_enable", (GLint) 1);
		pProgram->setUniformValue("u_wave_amplitude", (GLfloat) (float) dAmplitude);
	}

	return pProgram;
}

double CWaterMaterial::getHeightAt(const Math::CGeoloc& gPosition, double* pRigidness)
{
	if (pRigidness != NULL)
	{
		*pRigidness = m_dRigidness;
	}

	return WaveHeight(gPosition);
}

double CWaterMaterial::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, double* pRigidness)
{
	if (pRigidness != NULL)
	{
		*pRigidness = m_dRigidness;
	}

	return WaveHeight(CGeoloc(vPosition));
}

double CWaterMaterial::getHeightAt(const Math::CVector3& vPosition, const Math::CAxis& aAxis, bool bForPhysics)
{
	return WaveHeight(CGeoloc(vPosition));
}

double CWaterMaterial::WaveHeight(CGeoloc gPosition)
{
	CPerlin* pPerlin = CPerlin::getInstance();
	double dAmplitude = m_pScene->getWindLevel() * 4.0;

	CVector3 input_position = gPosition.toVector3() - m_pScene->getWorldOrigin();

	CVector3 position_1 = input_position;
	CVector3 position_2 = input_position + CVector3(2.0, 2.0, 2.0);

	double value_1 = pPerlin->getTurbulence((position_1 * 0.005) + CVector3(m_dTime, m_dTime, m_dTime) * 0.1) * dAmplitude * 4.0;
	double value_2 = pPerlin->getTurbulence((position_2 * 0.005) - CVector3(m_dTime, m_dTime, m_dTime) * 0.1) * dAmplitude * 4.0;

	// double value_3 = pPerlin->getTurbulence((position_1 * 0.050) + Vector3(m_dTime, m_dTime, m_dTime) * 0.5) * dAmplitude * 0.8;
	// double value_4 = pPerlin->getTurbulence((position_2 * 0.050) - Vector3(m_dTime, m_dTime, m_dTime) * 0.5) * dAmplitude * 0.8;

	// double alpha_factor = Math::Angles::clipDouble(((a_altitude * -1.0) / 40.0), 0.0, 1.0);
    // double alpha_factor = 1.0;

	// return (value_1 + value_2 + value_3 + value_4) * alpha_factor;
	return value_1 + value_2;
}