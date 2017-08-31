
// Qt
#include <QDebug>

// Quick3D
#include "CComponentFactory.h"
#include "CCamera.h"

// Application
#include "CUnitTests.h"

//-------------------------------------------------------------------------------------------------

using namespace Math;

//-------------------------------------------------------------------------------------------------

CUnitTests::CUnitTests()
{
}

//-------------------------------------------------------------------------------------------------

CUnitTests::~CUnitTests()
{
    CComponentFactory::killInstance();
}

//-------------------------------------------------------------------------------------------------

void CUnitTests::run()
{
    qDebug() << "--------------------------------------------------";
    qDebug() << "Testing CVector2::pointToAngles()";

    QVector<CVector2> vNormalizedPoints;
    QVector<CVector2> vPoints;
    CVector2 viewportSize(1000, 1000);
    CMatrix4 transform;
    double dFOV = 90.0;

    vNormalizedPoints << CVector2(0.00, 0.5);
    vNormalizedPoints << CVector2(0.25, 0.5);
    vNormalizedPoints << CVector2(0.50, 0.5);
    vNormalizedPoints << CVector2(0.75, 0.5);
    vNormalizedPoints << CVector2(1.00, 0.5);

    vNormalizedPoints << CVector2(0.00, 0.0);
    vNormalizedPoints << CVector2(0.50, 0.0);
    vNormalizedPoints << CVector2(1.00, 0.0);

    foreach (CVector2 vPoint, vNormalizedPoints)
    {
        CVector2 vAngles2 = CVector2::pointToAngles(viewportSize, dFOV, vPoint);

        CVector3 vAngles3(vAngles2.Y, vAngles2.X, 0.0);

        CRay3 ray;
        ray.vOrigin = transform * CVector3();
        ray.vNormal = CMatrix4::makeRotation(vAngles3) * CVector3(0.0, 0.0, 1.0);
        ray.vNormal = (transform * ray.vNormal) - ray.vOrigin;

        qDebug() << "--------------------";
        qDebug() << "Point =" << vPoint.X << "," << vPoint.Y;
        qDebug() << "Angles = " << Angles::toDeg(vAngles2.X) << "," << Angles::toDeg(vAngles2.Y);
        qDebug() << "Ray =" << ray.vNormal.X << "," << ray.vNormal.Y << "," << ray.vNormal.Z;
        qDebug() << "Ray length =" << ray.vNormal.magnitude();
    }

    qDebug() << "--------------------------------------------------";
    qDebug() << "Testing CCamera::screenPointToWorldRay()";

    vPoints << CVector2(0, 0);
    vPoints << CVector2(250, 0);
    vPoints << CVector2(500, 0);
    vPoints << CVector2(750, 0);
    vPoints << CVector2(1000, 0);

    vPoints << CVector2(0, 500);
    vPoints << CVector2(250, 500);
    vPoints << CVector2(500, 500);
    vPoints << CVector2(750, 500);
    vPoints << CVector2(1000, 500);

    vPoints << CVector2(0, 750);
    vPoints << CVector2(250, 750);
    vPoints << CVector2(500, 750);
    vPoints << CVector2(750, 750);
    vPoints << CVector2(1000, 750);

    vPoints << CVector2(0, 1000);
    vPoints << CVector2(250, 1000);
    vPoints << CVector2(500, 1000);
    vPoints << CVector2(750, 1000);
    vPoints << CVector2(1000, 1000);

    C3DScene* pScene = new C3DScene();
    QSP<CCamera> pCamera = QSP<CCamera>(new CCamera(pScene));

    pScene->viewports()[0] = new CViewport(pScene);
    pScene->viewports()[0]->setSize(Math::CVector2(1024, 768));
    pScene->viewports()[0]->setCamera(pCamera);

    pCamera->setGeoloc(CGeoloc(0.0, 0.0, 0.0));
    pCamera->computeWorldTransform();

    double dVerticalFOV = CVector2::computeVerticalFOV(pScene->viewports()[0]->size(), pCamera->FOV());
    double dAspectRatio = pScene->viewports()[0]->size().X / pScene->viewports()[0]->size().Y;

//    CMatrix4 mTransform = CCamera::getInternalCameraMatrix(pCamera->worldPosition(), pCamera->worldRotation());
//    CMatrix4 mProject = CCamera::getInternalProjectionMatrix(dVerticalFOV, dAspectRatio, 1.0, 1000.0);

    foreach (CVector2 vPoint, vPoints)
    {
        CRay3 ray = pCamera->screenPointToWorldRay(pScene->viewports()[0], vPoint);

        qDebug() << "--------------------";
        qDebug() << "Point =" << vPoint.X << "," << vPoint.Y;
        qDebug() << "Ray pos =" << ray.vOrigin.X << "," << ray.vOrigin.Y << "," << ray.vOrigin.Z;
        qDebug() << "Ray norm =" << ray.vNormal.X << "," << ray.vNormal.Y << "," << ray.vNormal.Z;
        qDebug() << "Ray length =" << ray.vNormal.magnitude();

//        CVector3 vPoint3D = ray.vOrigin + (ray.vNormal * 10.0);
//        vPoint3D = mTransform * vPoint3D;
//        vPoint3D = mProject.project(vPoint3D);
//        vPoint3D.X = (vPoint3D.X + 0.5) * pScene->viewports()[0]->size().X;
//        vPoint3D.Y = (vPoint3D.Y + 0.5) * pScene->viewports()[0]->size().Y;

//        qDebug() << "Projected point =" << vPoint3D.X << "," << vPoint3D.Y << "," << vPoint3D.Z;

//        CAxis cameraAxis(ray.vNormal, CAxis(pCamera->worldRotation()).Up);
//        CAxis nollAxis(pCamera->geoloc().getNOLLAxis());
//        CAxis axis = cameraAxis.transferTo(nollAxis);
//        CVector3 angles = axis.eulerAngles();

        // ray.vNormal = (pCamera->worldTransformInverse() * (ray.vNormal - ray.vOrigin)).normalized();
        ray = pCamera->worldTransformInverse() * ray;
//        CVector3 angles;
//        angles.Y = ray.vNormal.eulerYAngle();
//        ray.vNormal = CMatrix4::makeRotation(CVector3(0.0, -angles.Y, 0.0)) * ray.vNormal;
//        angles.X = ray.vNormal.eulerXAngle();
        CVector3 angles = eulerAngles(ray.vNormal);

        qDebug() << "Angles =" << Angles::toDeg(angles.X) << "," << Angles::toDeg(angles.Y) << "," << Angles::toDeg(angles.Z);
    }

    delete pScene;
}