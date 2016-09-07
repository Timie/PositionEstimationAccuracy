#include "RCameraPoseEstimation.h"

#include <Eigen/Core>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/eigen.hpp>

#include "theia/sfm/pose/perspective_three_point.h"
#include "theia/sfm/pose/dls_pnp.h"

#include "rce/geometry/RGeometry.h"

//#ifdef RCE_NO_INFO_OUTPUT
//#undef RCE_NO_INFO_OUTPUT
//#endif

#include "dfs/core/DDebug.h"
#include "rce/utility/ROpenCVtoQDebug.h"


#ifndef RCE_POSE_MAX_DISTANCE_TO_SCENE
#define RCE_POSE_MAX_DISTANCE_TO_SCENE (10000)
#endif





void
rce::geometry::RCameraPoseEstimation::
estimateCameraPose(const cv::Mat &cameraMatrix,
                   const cv::Mat &homographyFromWorldToImage,
                   const cv::Point3d &pointInFrontOfCamera,
                   std::vector<cv::Point3d> &positions,
                   std::vector<cv::Mat> &rotations,
                   std::vector<cv::Point3d> &headingVectors,
                   std::vector<cv::Point3d> &toSceneVectors,
                   bool leftHandedCoordinateSystem,
                   bool filterSolutions,
                   bool useAllMethods)
{
    cv::Mat H = getCleanHomography(cameraMatrix,
                                   homographyFromWorldToImage,
                                   leftHandedCoordinateSystem);


    // firstly, dissect homographyFromWorldToImage to transformation between world plane and projection plane
    cv::Mat cameraMatrixInv = cameraMatrix.inv();

    std::vector<cv::Point3d> candidatePositions;
    std::vector<cv::Mat> candidateRotations;

    estimatePoseNADIRApproach(H,
                              candidatePositions,
                              candidateRotations);

    if(useAllMethods)
    {
        // the simplest algorithm:
        estimatePoseBetweenSimple(H,
                                  candidatePositions,
                                  candidateRotations);

        // the dumbest algorithm
        estimatePoseBetweenDumb(H,
                                candidatePositions,
                                candidateRotations);

        // opencv algorithm
        estimatePoseBetweenCVHomography(H,
                                        candidatePositions,
                                        candidateRotations);

        // opencv algorithm 2
        estimatePoseBetweenImagesCVHomography(cameraMatrixInv * homographyFromWorldToImage,
                                              cameraMatrix,
                                              candidatePositions,
                                              candidateRotations);
    }


    // from theia library
    estimatePoseFromHomographyTheiaPNP(H,
                                       pointInFrontOfCamera,
                                       candidatePositions,
                                       candidateRotations);

    if(filterSolutions)
    {
        // get only meaningfull solutions from it
        getReasonableSolutions(candidatePositions,
                               candidateRotations,
                               pointInFrontOfCamera,
                               positions,
                               rotations,
                               headingVectors,
                               toSceneVectors,
                               -1);

    }
    else
    {
        positions = candidatePositions;
        rotations = candidateRotations;
    }

// for debug purposes
//    for(int i = 0;
//        i < positions.size();
//        ++i)
//    {
//        cv::Mat rotInv;
//        cv::Point3d posInv;
//        rce::geometry::invertTranslationAndRotation(positions[i],
//                                                    rotations[i],
//                                                    posInv,
//                                                    rotInv);
//        cv::Mat checkTransform(3,3, CV_64FC1);
//        rotInv.col(0).copyTo(checkTransform.col(0));
//        rotInv.col(1).copyTo(checkTransform.col(1));
//        checkTransform.at<double>(0,2) = posInv.x;
//        checkTransform.at<double>(1,2) = posInv.y;
//        checkTransform.at<double>(2,2) = posInv.z;

//        cv::Mat residualTransform = H * checkTransform.inv();
//        residualTransform = residualTransform / residualTransform.at<double>(2,2);

//        qDebug() << "Residual transform" << i << residualTransform;
//    }
}

void
rce::geometry::RCameraPoseEstimation::
estimateCameraPoseRobust(const cv::Mat &cameraMatrix,
                         const cv::Mat &homographyFromWorldToImage,
                         const cv::Point3d &pointInFrontOfCamera,
                         std::vector<cv::Point3d> &positions,
                         std::vector<cv::Mat> &rotations,
                         std::vector<cv::Point3d> &headingVectors,
                         std::vector<cv::Point3d> &toSceneVectors,
                         bool leftHandedCoordinateSystem)
{
    cv::Mat H = getCleanHomography(cameraMatrix,
                                   homographyFromWorldToImage,
                                   leftHandedCoordinateSystem);


    std::vector<cv::Point3d> candidatePositions;
    std::vector<cv::Mat> candidateRotations;

    // from theia library
    estimatePoseFromHomographyTheiaPNP(H.clone(),
                                       pointInFrontOfCamera,
                                       candidatePositions,
                                       candidateRotations);
    // filter them out
    std::vector<cv::Point3d> filteredPositions;
    std::vector<cv::Mat> filteredRotations;
    std::vector<cv::Point3d> filteredHeadingVectors;
    std::vector<cv::Point3d> filteredToSceneVectors;
    getReasonableSolutions(candidatePositions,
                           candidateRotations,
                           pointInFrontOfCamera,
                           filteredPositions,
                           filteredRotations,
                           filteredHeadingVectors,
                           filteredToSceneVectors,
                           0.1);

    if(filteredPositions.size() > 0)
    {

        cv::Point2d nadir = rce::geometry::calculateNadirPoint(H);

        dDebug() << "NADIR" << qSetRealNumberPrecision(10) << nadir.x << nadir.y;

        double bestDist = std::numeric_limits<double>::max();
        int bestIdx = -1;

        for(int i = 0;
            i < filteredPositions.size();
            ++i)
        {
            double dist = cv::norm(nadir - cv::Point2d(filteredPositions[i].x,
                                                       filteredPositions[i].y));
            if(dist < bestDist)
            {
                bestDist = dist;
                bestIdx = i;
            }
        }

        dDebug() << "Best distance" << bestDist;

        positions.push_back(filteredPositions[bestIdx]);
        rotations.push_back(filteredRotations[bestIdx]);
        headingVectors.push_back(filteredHeadingVectors[bestIdx]);
        toSceneVectors.push_back(filteredToSceneVectors[bestIdx]);
    }
    else
    {
        estimateCameraPose(cameraMatrix,
                           homographyFromWorldToImage,
                           pointInFrontOfCamera,
                           positions,
                           rotations,
                           headingVectors,
                           toSceneVectors,
                           leftHandedCoordinateSystem,
                           true,
                           true);
    }

}

cv::Mat
rce::geometry::RCameraPoseEstimation::
getCleanHomography(const cv::Mat &cameraMatrix,
                   const cv::Mat &_fromWorldToImage,
                   bool flipY)
{
    cv::Mat fromWorldToImage;
    if(flipY)
    { // to make sure that rotation is not mirroring (and thus rotation determinant would be negative)
        // we assume that final image is flipped
        fromWorldToImage = _fromWorldToImage.clone();
        fromWorldToImage.row(1) = fromWorldToImage.row(1) * (-1.0);
    }
    else
    {
        fromWorldToImage = _fromWorldToImage;
    }

    // firstly, get clean homography
    return cameraMatrix.inv() * fromWorldToImage;
}

double estimateZ(const cv::Mat &H,
                 const cv::Vec3d &r3rest,
                 const cv::Point2d &nadirPoint,
                 double coeffW,
                 double coeffR3)
{
    double zEst1 = (H.at<double>(0,2) + coeffW * H.at<double>(0,0) * nadirPoint.x + coeffW * H.at<double>(0,1) * nadirPoint.y) /
                   (-1.0 * coeffW * coeffR3 * r3rest(0));

    double zEst2 = (H.at<double>(1,2) + coeffW * H.at<double>(1,0) * nadirPoint.x + coeffW * H.at<double>(1,1) * nadirPoint.y) /
                   (-1.0 * coeffW * coeffR3 * r3rest(1));

    double zEst = (zEst1 + zEst2) / 2.0;

    dDebug() << "estimated height" << zEst << zEst1 << zEst2 << coeffW << coeffR3;
    return zEst;
}


void
rce::geometry::RCameraPoseEstimation::
estimatePoseNADIRApproach(const cv::Mat &cameraMatrix,
                          const cv::Mat &fromWorldToImage,
                          std::vector<cv::Point3d> &positions,
                          std::vector<cv::Mat> &rotations,
                          bool flipY)
{
    // firstly, get clean homography
    cv::Mat H = getCleanHomography(cameraMatrix,
                                   fromWorldToImage,
                                   flipY);


    estimatePoseNADIRApproach(H,
                              positions,
                              rotations);

}

void
rce::geometry::RCameraPoseEstimation::
estimatePoseNADIRApproach(const cv::Mat &_H,
                          std::vector<cv::Point3d> &positions,
                          std::vector<cv::Mat> &rotations)
{
    cv::Mat H = _H.clone();
    // estimate magnitude of scaling and denormalise the H
    double wEst1 = cv::norm(H.col(0));
    double wEst2 = cv::norm(H.col(1));
    double wEst = (wEst1 + wEst2) / 2.0;
    H = H / wEst;
    // w = +- wEst

    // calculate the inverse
    cv::Mat invH = H.inv();

    // calculate NADIR point
    cv::Point2d nadirPoint = rce::geometry::calculateNadirPoint(H,
                                                                invH);


    dDebug() << "NADIR POSE" << qSetRealNumberPrecision(10) << nadirPoint.x << nadirPoint.y << wEst1 << wEst2 << wEst1/wEst2;

    // get two estimates of R
    cv::Mat r3est = H.col(0).cross(H.col(1)); // the sign of w does not have effect in here, as perpendicularity is kept even if we multiply by -1 both vectors
    // r3 = +- r3est


    // estimate Z for all 4 posibilities
    double zPP = estimateZ(H, cv::Vec3d(r3est), nadirPoint, 1,1); // inside this function we can check if both estimated Z are similar or not.
    double zMP = estimateZ(H, cv::Vec3d(r3est), nadirPoint, -1,1);
    double zPM = estimateZ(H, cv::Vec3d(r3est), nadirPoint, 1,-1);
    double zMM = estimateZ(H, cv::Vec3d(r3est), nadirPoint, -1,-1);

    std::vector<cv::Vec3d> coeffSolutions;

    coeffSolutions.push_back(cv::Vec3d(1,1, zPP));
    coeffSolutions.push_back(cv::Vec3d(-1,1, zMP));
    coeffSolutions.push_back(cv::Vec3d(1,-1, zPM));
    coeffSolutions.push_back(cv::Vec3d(-1,-1, zMM));

    for(int i = 0;
        i < coeffSolutions.size();
        ++i)
    {
        cv::Mat fixedH = H * coeffSolutions[i](0);
        // fixedH should now be: [R_r^T(1) R_r^T(2) (-R_r^T*t)]
        cv::Mat rotationT(3,3,CV_64FC1);
        fixedH.col(0).copyTo(rotationT.col(0));
        fixedH.col(1).copyTo(rotationT.col(1));
        cv::Mat(r3est * coeffSolutions[i](1)).copyTo(rotationT.col(2)); // QUESTION: should we multiply here by coeffSolutions[i](0)????
                rotationT = rotationT * coeffSolutions[i](1); // this was added here... because it works when coeffSolutions[i](1) is -1

//        positions.push_back(cv::Point3d(nadirPoint.x,
//                                        nadirPoint.y,
//                                        coeffSolutions[i](2)));

//        rotations.push_back(rotationT.t());

        // make it pure rotation and translation
        Eigen::Matrix3d eigMat;
        cv::cv2eigen(rotationT,
                     eigMat);
        Eigen::Projective3d projTransform = Eigen::Projective3d::Identity();
        projTransform.linear() = eigMat;
        eigMat = projTransform.rotation();
        cv::Mat fixedRotationT;
        cv::eigen2cv(eigMat, fixedRotationT);

        cv::Vec3d fixedPosition2 = cv::Mat(fixedRotationT.t() *  coeffSolutions[i](1) * (-1) * fixedH.col(2));



        //qDebug() << "For solution:" << i << nadirPoint.x << nadirPoint.y << coeffSolutions[i](2) << "the alternative solutions are"
//                                                                 << fixedPosition2(0)
//                                                                 << fixedPosition2(1)
//                                                                 << fixedPosition2(2);

        positions.push_back(cv::Point3d(nadirPoint.x,
                                        nadirPoint.y,
                                        fixedPosition2(2)));

        rotations.push_back(fixedRotationT.t());

        //qDebug() << "Determinants" << cv::determinant(rotationT.t()) << cv::determinant(fixedRotationT.t());
    }
}




void
rce::geometry::RCameraPoseEstimation::
estimatePoseBetweenSimple(const cv::Mat &homography,
                          std::vector<cv::Point3d> &positions,
                          std::vector<cv::Mat> &rotations)
{

    // based upon http://ags.cs.uni-kl.de/fileadmin/inf_ags/3dcv-ws11-12/3DCV_WS11-12_lec04.pdf
    // homography = lambda * [r1 r2 t]
    double norm1 = cv::norm(homography.col(0));
    double norm2 = cv::norm(homography.col(1));
    double lambda = (norm1 + norm2) / 2.0;

    cv::Vec3d r1(homography.at<double>(0,0) / norm1,
                 homography.at<double>(1,0) / norm1,
                 homography.at<double>(2,0) / norm1);
    cv::Vec3d r2(homography.at<double>(0,1) / norm2,
                 homography.at<double>(1,1) / norm2,
                 homography.at<double>(2,1) / norm2);
    cv::Point3d t(homography.at<double>(0,2) / lambda,
                  homography.at<double>(1,2) / lambda,
                  homography.at<double>(2,2) / lambda);

    cv::Vec3d r3a = r1.cross(r2);
    double norm3 = cv::norm(r3a);
    r3a *= (1.0 / norm3);

    cv::Vec3d r3b = r3a * (-1);

    cv::Mat rotMatA = rce::geometry::composeRotationMatrix(r1, r2, r3a);
    cv::Mat rotMatB = rce::geometry::composeRotationMatrix(r1, r2, r3b);

    cv::Mat rotMatAInv, rotMatBInv;
    cv::Point3d tAInv, tBInv;
    rce::geometry::invertTranslationAndRotation(t,
                                                rotMatA,
                                                tAInv,
                                                rotMatAInv);
    rce::geometry::invertTranslationAndRotation(t,
                                                rotMatB,
                                                tBInv,
                                                rotMatBInv);
    positions.push_back(tAInv);
    positions.push_back(tBInv);
    rotations.push_back(rotMatAInv);
    rotations.push_back(rotMatBInv);
}

void
rce::geometry::RCameraPoseEstimation::
estimatePoseBetweenDumb(const cv::Mat &h,
                        std::vector<cv::Point3d> &positions,
                        std::vector<cv::Mat> &rotations)
{

    // dumbest version based on http://dsp.stackexchange.com/questions/2736/step-by-step-camera-pose-estimation-for-visual-tracking-and-planar-markers

    cv::Mat rotation(3,3,
                     CV_64FC1);

    h.col(0).copyTo(rotation.col(0));
    h.col(1).copyTo(rotation.col(1));
    cv::Mat v3 = h.col(0).cross(h.col(1));
    double norm1 = cv::norm(h.col(0));
    double norm2 = cv::norm(h.col(1));
    double tnorm = (norm1 + norm2) / 2.0;
    //dDebug() << "Rotnorms"<< norm1 << norm2 << cv::norm(v3);
    v3 = v3 / tnorm;
    v3.copyTo(rotation.col(2));

    cv::Point3d position = static_cast<cv::Point3d>(cv::Mat(h.col(2) / tnorm));

    cv::Mat rotMatInv;
    cv::Point3d tInv;
    rce::geometry::invertTranslationAndRotation(position,
                                                rotation / tnorm,
                                                tInv,
                                                rotMatInv);

    positions.push_back(tInv);
    rotations.push_back(rotMatInv);

}

void
rce::geometry::RCameraPoseEstimation::
estimatePoseBetweenCVHomography(const cv::Mat &homography,
                                std::vector<cv::Point3d> &positions,
                                std::vector<cv::Mat> &rotations)
{
    estimatePoseBetweenImagesCVHomography(homography,
                                          cv::Mat::eye(3,3,
                                                       CV_64FC1),
                                          positions,
                                          rotations);
}

void
rce::geometry::RCameraPoseEstimation::
estimatePoseBetweenImagesCVHomography(const cv::Mat &homography,
                                      const cv::Mat &cameraMatrix,
                                      std::vector<cv::Point3d> &positions,
                                      std::vector<cv::Mat> &rotations)
{
    std::vector<cv::Mat> rotationsCV, translationsCV, normals;
    cv::decomposeHomographyMat(homography,
                               cameraMatrix,
                               rotationsCV,
                               translationsCV,
                               normals);

    for(int i = 0;
        i < rotationsCV.size();
        ++i)
    {
        //qDebug() << "estimatePoseBetweenCVHomography" << rotationsCV[i].type() << translationsCV[i].type() << translationsCV[i].cols << translationsCV[i].rows  << translationsCV[i].channels();
        positions.push_back(cv::Point3d(translationsCV[i]));
        rotations.push_back(rotationsCV[i]);

        cv::Point3d tInv;
        cv::Mat rotInv;
        rce::geometry::invertTranslationAndRotation(cv::Point3d(translationsCV[i]),
                                                    rotationsCV[i],
                                                    tInv,
                                                    rotInv);
        rotations.push_back(rotInv);
        positions.push_back(tInv);

    }
}

void
rce::geometry::RCameraPoseEstimation::
estimatePoseFromHomographyTheiaP3P(const cv::Mat &homography,
                                   const cv::Point3d &sceneCenter,
                                   std::vector<cv::Point3d> &positions,
                                   std::vector<cv::Mat> &rotations,
                                   double featurePointScale)
{
    cv::Vec3d scenePt1(sceneCenter.x - 0.5 * featurePointScale,
                       sceneCenter.y + 1 * featurePointScale,
                       1);
    cv::Vec3d scenePt2(sceneCenter.x - 0.5 * featurePointScale,
                       sceneCenter.y - 1 * featurePointScale,
                       1);
    cv::Vec3d scenePt3(sceneCenter.x + 1 * featurePointScale,
                       sceneCenter.y,
                       1);
    cv::Vec3d camPt1 = cv::Mat(homography * cv::Mat(scenePt1));
    cv::Vec3d camPt2 = cv::Mat(homography * cv::Mat(scenePt2));
    cv::Vec3d camPt3 = cv::Mat(homography * cv::Mat(scenePt3));

    Eigen::Vector2d feature_point[3];
    feature_point[0](0) = camPt1(0) / camPt1(2);
    feature_point[0](1) = camPt1(1) / camPt1(2);
    feature_point[1](0) = camPt2(0) / camPt2(2);
    feature_point[1](1) = camPt2(1) / camPt2(2);
    feature_point[2](0) = camPt3(0) / camPt3(2);
    feature_point[2](1) = camPt3(1) / camPt3(2);

    Eigen::Vector3d world_point[3];
    world_point[0](0) = scenePt1(0);
    world_point[0](1) = scenePt1(1);
    world_point[0](2) = 0;
    world_point[1](0) = scenePt2(0);
    world_point[1](1) = scenePt2(1);
    world_point[1](2) = 0;
    world_point[2](0) = scenePt3(0);
    world_point[2](1) = scenePt3(1);
    world_point[2](2) = 0;

    std::vector<Eigen::Matrix3d> solution_rotations;
    std::vector<Eigen::Vector3d> solution_translations;

    theia::PoseFromThreePoints(feature_point,
                               world_point,
                               &solution_rotations,
                               &solution_translations);

    for(int i = 0;
        i < solution_rotations.size();
        ++i)
    {
        cv::Mat rotation;
        cv::eigen2cv(solution_rotations[i],
                     rotation);
        cv::Point3d position(solution_translations[i](0),
                             solution_translations[i](1),
                             solution_translations[i](2));
//        rotations.push_back(rotation);
//        positions.push_back(position);

        cv::Point3d tInv;
        cv::Mat rotInv;
        rce::geometry::invertTranslationAndRotation(position,
                                                    rotation,
                                                    tInv,
                                                    rotInv);
        rotations.push_back(rotInv);
        positions.push_back(tInv);
    }
}

void
rce::geometry::RCameraPoseEstimation::
estimatePoseFromHomographyTheiaPNP(const cv::Mat &homography,
                                   const cv::Point3d &sceneCenter,
                                   std::vector<cv::Point3d> &positions,
                                   std::vector<cv::Mat> &rotations,
                                   double featurePointScale)
{
    cv::Vec3d scenePt1(sceneCenter.x - 1 * featurePointScale,
                       sceneCenter.y + 1 * featurePointScale,
                       1);
    cv::Vec3d scenePt2(sceneCenter.x - 1 * featurePointScale,
                       sceneCenter.y - 1 * featurePointScale,
                       1);
    cv::Vec3d scenePt3(sceneCenter.x + 1 * featurePointScale,
                       sceneCenter.y + 1 * featurePointScale,
                       1);
    cv::Vec3d scenePt4(sceneCenter.x + 1 * featurePointScale,
                       sceneCenter.y - 1 * featurePointScale,
                       1);
    cv::Vec3d camPt1 = cv::Mat(homography * cv::Mat(scenePt1));
    cv::Vec3d camPt2 = cv::Mat(homography * cv::Mat(scenePt2));
    cv::Vec3d camPt3 = cv::Mat(homography * cv::Mat(scenePt3));
    cv::Vec3d camPt4 = cv::Mat(homography * cv::Mat(scenePt4));

    std::vector<Eigen::Vector2d> feature_points(4);
    feature_points[0](0) = camPt1(0) / camPt1(2);
    feature_points[0](1) = camPt1(1) / camPt1(2);
    feature_points[1](0) = camPt2(0) / camPt2(2);
    feature_points[1](1) = camPt2(1) / camPt2(2);
    feature_points[2](0) = camPt3(0) / camPt3(2);
    feature_points[2](1) = camPt3(1) / camPt3(2);
    feature_points[3](0) = camPt4(0) / camPt4(2);
    feature_points[3](1) = camPt4(1) / camPt4(2);

    std::vector<Eigen::Vector3d> world_points(4);
    world_points[0](0) = scenePt1(0);
    world_points[0](1) = scenePt1(1);
    world_points[0](2) = 0;
    world_points[1](0) = scenePt2(0);
    world_points[1](1) = scenePt2(1);
    world_points[1](2) = 0;
    world_points[2](0) = scenePt3(0);
    world_points[2](1) = scenePt3(1);
    world_points[2](2) = 0;
    world_points[3](0) = scenePt4(0);
    world_points[3](1) = scenePt4(1);
    world_points[3](2) = 0;

    std::vector<Eigen::Quaterniond> solution_rotations;
    std::vector<Eigen::Vector3d> solution_translations;

    theia::DlsPnp(feature_points,
                  world_points,
                  &solution_rotations,
                  &solution_translations);

    dDebug() << "Estimated positions:" << solution_rotations.size();
    for(int i = 0;
        i < solution_rotations.size();
        ++i)
    {
        cv::Mat rotation;
        cv::eigen2cv(solution_rotations[i].matrix(),
                     rotation);
        cv::Point3d position(solution_translations[i](0),
                             solution_translations[i](1),
                             solution_translations[i](2));
//        rotations.push_back(rotation);
//        positions.push_back(position);

        cv::Point3d tInv;
        cv::Mat rotInv;
        rce::geometry::invertTranslationAndRotation(position,
                                                    rotation,
                                                    tInv,
                                                    rotInv);
        rotations.push_back(rotInv);
        positions.push_back(tInv);
    }
}

void
rce::geometry::RCameraPoseEstimation::
getReasonableSolutions(const std::vector<cv::Point3d> &allPositions,
                       const std::vector<cv::Mat> &allRotations,
                       const cv::Point3d &pointInFrontOfCamera,
                       std::vector<cv::Point3d> &positions,
                       std::vector<cv::Mat> &rotations,
                       std::vector<cv::Point3d> &headingVectors,
                       std::vector<cv::Point3d> &toSceneVectors,
                       double determinantLimit)
{
    for(int i = 0;
        i < allPositions.size();
        ++i)
    {
        //qDebug() << "Checking solution" << qSetRealNumberPrecision(10) << i << allPositions[i].x << allPositions[i].y << allPositions[i].z;
        // position must be above ground
        if(allPositions[i].z < 0)
        {
            dInfo() << "RCameraPoseEstimation::getReasonableSolutions: solution" << i << "does not fullfill height constraint." << allPositions[i].x << allPositions[i].y << allPositions[i].z;

            continue;
        }

        cv::Mat rotInv;
        cv::Point3d posInv;
        rce::geometry::invertTranslationAndRotation(allPositions[i],
                                                    allRotations[i],
                                                    posInv,
                                                    rotInv);

        // check whether the point in front of the camera is in front of the camera
        double camZ = rotInv.at<double>(2,0) * pointInFrontOfCamera.x +
                      rotInv.at<double>(2,1) * pointInFrontOfCamera.y +
                      rotInv.at<double>(2,2) * pointInFrontOfCamera.z +
                      posInv.z * 1.0;

        if(camZ <= 0)
        {
            dInfo() << "RCameraPoseEstimation::getReasonableSolutions: solution" << i << "does not fullfill heading constraint." << camZ;

            continue;
        }

        // calculate heading vector
        cv::Point3d originTransformed = rce::geometry::transformPoint(cv::Point3d(0,0,0),
                                                                      allPositions[i],
                                                                      allRotations[i]);
        cv::Point3d viewVectorTransformed = rce::geometry::transformPoint(cv::Point3d(0,0,1),
                                                                          allPositions[i],
                                                                          allRotations[i]);
        cv::Point3d headingVector = viewVectorTransformed - originTransformed;
        headingVector = headingVector / cv::norm(headingVector);

        // calculate to scene vector
        cv::Point3d toSceneVector = pointInFrontOfCamera - allPositions[i];
        double distToScene = cv::norm(toSceneVector);
        // dInfo() << i << "Distance to scene" << distToScene;
        if(distToScene > RCE_POSE_MAX_DISTANCE_TO_SCENE)
        {
            dInfo() << "RCameraPoseEstimation::getReasonableSolutions: solution" << i << "does not fullfill distance constraint." << distToScene;

            continue;
        }

        toSceneVector = toSceneVector / distToScene;

        if(determinantLimit >= 0)
        {
            double determinant = cv::determinant(allRotations[i]);
            if(std::abs(determinant - 1.0) > determinantLimit)
            {
                dInfo() << "RCameraPoseEstimation::getReasonableSolutions: solution" << i << "does not fullfill determinant constraint(" << determinant << ").";

                continue;
            }

            //qDebug() << "Transformation passes:" << allPositions[i].x << allPositions[i].y << allPositions[i].z << camZ << distToScene << determinant;

        }
        else
        {

            //qDebug() << "Transformation passes:" << allPositions[i].x << allPositions[i].y << allPositions[i].z << camZ << distToScene;
        }

        //qDebug() << "Rotation" << allRotations[i];


        positions.push_back(allPositions[i]);


        rotations.push_back(allRotations[i]);
        headingVectors.push_back(headingVector);
        toSceneVectors.push_back(toSceneVector);

    }
}
