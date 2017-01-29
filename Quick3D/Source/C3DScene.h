
#pragma once

#include "quick3d_global.h"

// Qt
#include <QGLWidget>
#include <QImage>
#include <QTime>

// Application
#include "C3DSceneStatistics.h"
#include "CVector3.h"
#include "CMatrix4.h"
#include "Interpolator.h"
#include "CDumpable.h"
#include "CGLExtension.h"
#include "CRessourcesManager.h"
#include "CBuildingGenerator.h"
#include "CTreeGenerator.h"
#include "CShaderCollection.h"
#include "CCamera.h"
#include "CLight.h"
#include "CFog.h"
#include "CRain.h"
#include "CRenderContext.h"
#include "CViewport.h"
#include "CMeshByMaterial.h"

//-------------------------------------------------------------------------------------------------

// #define DRAW_BY_MATERIAL

//-------------------------------------------------------------------------------------------------

class CView;
class CController;

//-------------------------------------------------------------------------------------------------

class QUICK3D_EXPORT C3DScene : public CDumpable
{
public:

    //-------------------------------------------------------------------------------------------------
    // Constructeurs et destructeur
    //-------------------------------------------------------------------------------------------------

    //! Constructeur
    C3DScene(bool bForDisplay = true);

    //! Destructeur
    virtual ~C3DScene();

    //-------------------------------------------------------------------------------------------------
    // Setters
    //-------------------------------------------------------------------------------------------------

    //!
    void setWorldOrigin(Math::CVector3 value);

    //!
    void setController(CController* pController);

    //!
    void setDefaultController(CController* pController);

    //!
    void setShaderQuality(double value);

    //!
    void setTime(double value);

    //!
    void setTimeOfDay(QTime value);

    //!
    void setFogLevel(double value);

    //!
    void setWindLevel(double value);

    //!
    void setRainVisible(bool bOnOff);

    //!
    void forceWideFOV(bool value);

    //!
    void forceSmallFOV(bool value);

    //!
    void forceIR(bool value);

    //!
    void setStreamView(bool value) { m_bStreamView = value; }

    //!
    void setDepthComputing(bool value) { m_bDepthComputing = value; }

    //!
    void setRenderingShadows(bool value) { m_bRenderingShadows = value; }

    //!
    void setEditMode(bool value) { m_bEditMode = value; }

    //!
    void setDebugMode(bool value) { m_DebugMode = value; }

    //!
    void setBoundsOnly(bool value) { m_bBoundsOnly = value; }

    //!
    void setNormalsOnly(bool value) { m_bNormalsOnly = value; }

    //-------------------------------------------------------------------------------------------------
    // Getters
    //-------------------------------------------------------------------------------------------------

    //!
    bool getForDisplay() const { return m_bForDisplay; }

    //!
    Math::CVector3 getWorldOrigin() const { return m_vWorldOrigin; }

    //!
    QMap<int, CViewport*>& getViewports() { return m_pViewports; }

    //!
    const QMap<int, CViewport*>& getViewports() const { return m_pViewports; }

    //!
    double getShaderQuality() { return m_dShaderQuality; }

    //!
    CGLExtension* getGLExtension() { return m_pGLExtension; }

    //!
    CRessourcesManager* getRessourcesManager() { return m_pRessourcesManager; }

    //!
    CBuildingGenerator* getBuildingGenerator() { return m_pBuildingGenerator; }

    //!
    CTreeGenerator* getTreeGenerator() { return m_pTreeGenerator; }

    //!
    CShaderCollection* getShaders() { return m_vShaders; }

    //!
    CController* getController() { return m_pController; }

    //!
    QVector<QSP<CLight> > getLights();

    //!
    QVector<QSP<CComponent> >& getComponents() { return m_vComponents; }

    //!
    const QVector<QSP<CComponent> >& getComponents() const { return m_vComponents; }

    //!
    QVector<QSP<CComponent> > getComponentsByTag(const QString& sTag);

    //!
    QVector<QSP<CLight> > getLightsByTag(const QString& sTag);

    //!
    CFog& getFog() { return m_tFog; }

    //!
    double getTime() const { return m_dTime; }

    //!
    double getWindLevel() const { return m_dWindLevel; }

    //!
    bool isRenderingShadows() const { return m_bRenderingShadows; }

    //!
    bool getStreamView() const { return m_bStreamView; }

    //!
    bool getDepthComputing() const { return m_bDepthComputing; }

    //!
    const QImage& getFrameBuffer() const { return m_imgFrameBuffer; }

    //!
    bool getEditMode() { return m_bEditMode; }

    //!
    bool getDebugMode() { return m_DebugMode; }

    //!
    bool getFrustumCheck() { return m_bFrustumCheck; }

    //!
    bool getBoundsOnly() { return m_bBoundsOnly; }

    //!
    bool getNormalsOnly() { return m_bNormalsOnly; }

    //-------------------------------------------------------------------------------------------------
    // M�thodes de contr�le
    //-------------------------------------------------------------------------------------------------

    //!
    void clear();

    //! D�truit les composants (hors lumi�res)
    void clearComponents();

    //! D�truit les viewports
    void clearViewports();

    //! Initialise la sc�ne
    virtual void init(QVector<QSP<CComponent> > vComponents);

    //! Initialise les shaders
    virtual void initShaders();

    //! Pr�pare l'environnement
    virtual void setupEnvironment(CRenderContext* pContext, QGLShaderProgram* pProgram, bool bBackgroundItem);

    //! Pr�pare les lumi�res
    virtual void setupLights(CRenderContext* pContext);

    //!
    virtual void makeCurrentRenderingContext();

    //!
    void autoResolveHeightFields();

    //!
    void updateScene(double dDeltaTime);

    //!
    void paintComponents(CRenderContext* pContext);

    //!
    void paintShadowCastingComponents(CRenderContext* pContext);

    //!
    void addComponent(QSP<CComponent> pComponent);

    //!
    void deleteComponentsByTag(const QString& sTag);

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersect(Math::CRay3 aRay) const;

    //! Calcul d'intersection avec un rayon
    virtual Math::RayTracingResult intersectComponentHierarchy(QSP<CComponent> pComponent, Math::CRay3 aRay) const;

    //! Calcul d'intersection avec un rayon
    Math::RayTracingResult intersectRecurse(QSP<CComponent> pComponent, const Math::CRay3& aRay) const;

    //!
    void addSegment(Math::CVector3 vStart, Math::CVector3 vEnd);

    //! Dump du contenu dans un flux
    virtual void dump(QTextStream& stream, int iIdent);

    //-------------------------------------------------------------------------------------------------
    // M�thodes prot�g�es
    // Protected methods
    //-------------------------------------------------------------------------------------------------

protected:

    //!
    static void getLightsRecurse(QVector<QSP<CLight> >& vLights, QSP<CComponent> pComponent);

    //!
    static void getLightsByTagRecurse(QVector<QSP<CLight> >& vLights, const QString &sTag, QSP<CComponent> pComponent);

    //-------------------------------------------------------------------------------------------------
    // Propri�t�s
    //-------------------------------------------------------------------------------------------------

public:

    C3DSceneStatistics                      m_tStatistics;
    CRain*                                  m_pRain;

protected:

    QMap<int, CViewport*>                   m_pViewports;
    CGLExtension*                           m_pGLExtension;
    CRessourcesManager*                     m_pRessourcesManager;
    CBuildingGenerator*                     m_pBuildingGenerator;
    CTreeGenerator*                         m_pTreeGenerator;
    CShaderCollection*                      m_vShaders;
    CController*                            m_pController;
    CController*                            m_pDefaultController;
    Math::CVector3                          m_vWorldOrigin;
    QTime                                   m_tTimeOfDay;
    CFog                                    m_tFog;
    Math::Interpolator<Math::Vector4>       m_vSunColor;
    bool                                    m_bForDisplay;
    bool                                    m_bFrustumCheck;
    bool                                    m_bEditMode;
    bool                                    m_DebugMode;
    bool                                    m_bBoundsOnly;
    bool                                    m_bNormalsOnly;
    double                                  m_dShaderQuality;
    double                                  m_dWindLevel;
    bool                                    m_bRenderingShadows;
    bool                                    m_bforceWideFOV;
    bool                                    m_bforceSmallFOV;
    bool                                    m_bForceIR;
    bool                                    m_bStreamView;
    bool                                    m_bDepthComputing;
    double                                  m_dTime;
    double                                  m_dSunIntensity;
    QImage                                  m_imgFrameBuffer;

    // Shared data

    QVector<QSP<CComponent> >               m_vComponents;
    QSP<CMeshGeometry>                      m_pSegments;
};
