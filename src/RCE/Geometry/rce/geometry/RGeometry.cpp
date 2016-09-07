#include "RGeometry.h"

#include <QtGlobal>
#include <QDebug>

//#include <iostream>
//#include <iomanip>

#include <Eigen/Geometry>

#include <opencv2/core/eigen.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/video/video.hpp>

#include "rce/core/RCppTweaks.h"

#define RCE_USE_OPENCV3_ALGORITHM (true)

#define RCE_USE_EIGEN_EULER_ANGLES (1)

namespace rce {
    namespace geometry {

        RGeometry::RGeometry()
        {
        }

        cv::Mat get4x4TransformMatrix(const cv::Mat &input)
        {
            if((input.cols == 4) && (input.rows == 4))
                return input;
            else
            {
                cv::Mat identity = cv::Mat::eye(cv::Size(4,4), input.type());

                cv::Rect cutRect(0,0,
                                 qMin(input.size().width, 4),
                                 qMin(input.size().height, 4));

                cv::Mat roi(identity, cutRect);
                input.copyTo(roi);

                return identity;
            }
        }

        cv::Mat get3x3TransformMatrix(const cv::Mat &input)
        {
            if(input.empty())
            {
                return  cv::Mat::eye(cv::Size(3,3), CV_64FC1);
            }
            else if((input.cols == 3) && (input.rows == 3))
                return input;
            else
            {
                cv::Mat identity = cv::Mat::eye(cv::Size(3,3), input.type());

                cv::Rect cutRect(0,0,
                                 qMin(input.size().width, 3),
                                 qMin(input.size().height, 3));

                cv::Mat roi(identity, cutRect);
                input.copyTo(roi);

                return identity;
            }
        }


        cv::Mat
        estimatePoseFromHomography(const cv::Mat &homography,
                                   const cv::Mat &invCameraMatrix)
        {
//            if(RCE_USE_OPENCV3_ALGORITHM)
//            {
//                // based upon implementation in https://github.com/Itseez/opencv/blob/7d4d28605087ec2d3878f9467aea313a2acdfd49/modules/calib3d/src/homography_decomp.cpp

//            }
//            else
//            {
                cv::Mat pose = cv::Mat::eye(3, 4, homography.type());
                cv::Mat h = invCameraMatrix * homography;


                // dumbest version based on http://dsp.stackexchange.com/questions/2736/step-by-step-camera-pose-estimation-for-visual-tracking-and-planar-markers
                // for another versions, see older revisions of this file in svn
                {
                    h.col(0).copyTo(pose.col(0));
                    h.col(1).copyTo(pose.col(1));
                    cv::Mat v3 = pose.col(0).cross(pose.col(1));
                    double norm1 = cv::norm(h.col(0));
                    double norm2 = cv::norm(h.col(1));
                    double tnorm = (norm1 + norm2) / 2.0;
                    //dDebug() << "Rotnorms"<< norm1 << norm2 << cv::norm(v3);
                    v3 = v3 / tnorm;
                    v3.copyTo(pose.col(2));

                    h.col(2).copyTo(pose.col(3));

                    return pose / tnorm;/*/ pose.at<double>(2,3)*/;

                }
//            }
        }

        double getAngleAverageDeg(double angle1,
                                  double angle2)
        {
            double angle1Norm = normalizeAngleDeg(angle1);
            double angle2Norm = normalizeAngleDeg(angle2);

            double angleAvg = normalizeAngleDeg((angle1Norm + angle2Norm) / 2.0);
            if(angleAbsDiff(angleAvg,
                            angle1Norm) > 90.0)
            {
                angleAvg = normalizeAngleDeg(angleAvg + 180.0);
            }

            return angleAvg;

        }

        double normalizeAngleDeg(double angle)
        {
            while(angle < 0.0)
            {
                angle += 360.0;
            }

            while(angle >= 360.0)
            {
                angle -= 360.0;
            }

            return angle;
        }

        double angleAbsDiff(double angle1,
                            double angle2)
        {
//            double angle1 = normalizeAngleDeg(angle1);
//            double angle2 = normalizeAngleDeg(angle2);
//            if(angle1 < angle2)
//            {
//                std::swap(angle1, angle2);
//            }

            double diff = normalizeAngleDeg(angle1 - angle2);

            if(diff > 180.0)
            {
                diff = 360 - diff;
            }

            return diff;

        }

        cv::Mat
        getHomographyFromPose(const cv::Mat &cameraPosition,
                              const cv::Mat &projectionMatrix)
        {
             cv::Mat homography3d = projectionMatrix * cameraPosition;
             cv::Mat homography(cv::Size(3,3), homography3d.type());
             homography3d.col(0).copyTo(homography.col(0));
             homography3d.col(1).copyTo(homography.col(1));
             homography3d.col(3).copyTo(homography.col(2));

             return homography;
        }

        QTransform
        cvMatToQTransform(const cv::Mat &cvTransformIn)
        {
            if(cvTransformIn.empty())
                return QTransform();
            else
            {

                double h[3][3];
                h[0][0] = 1;
                h[0][1] = 0;
                h[0][2] = 0;

                h[1][0] = 0;
                h[1][1] = 1;
                h[1][2] = 0;

                h[2][0] = 0;
                h[2][1] = 0;
                h[2][2] = 1;

                if(cvTransformIn.type() == CV_64FC1)
                { // doubles

                    cv::Mat cvTransform = cvTransformIn * (1.0 / cvTransformIn.at<double>(2,2));
                    for(int rowIdx = 0;
                        rowIdx < qMin(cvTransform.rows, 3);
                        ++rowIdx)
                    {
                        for(int colIdx = 0;
                            colIdx < qMin(cvTransform.cols, 3);
                            ++colIdx)
                        {
                            h[rowIdx][colIdx] = cvTransform.at<double>(rowIdx,
                                                                       colIdx);
                        }
                    }
                }
                else if(cvTransformIn.type() == CV_32FC1)
                { // floats

                    cv::Mat cvTransform = cvTransformIn * (1.0 / cvTransformIn.at<float>(2,2));
                    for(int rowIdx = 0;
                        rowIdx < qMin(cvTransform.rows, 3);
                        ++rowIdx)
                    {
                        for(int colIdx = 0;
                            colIdx < qMin(cvTransform.cols, 3);
                            ++colIdx)
                        {
                            h[rowIdx][colIdx] = cvTransform.at<float>(rowIdx,
                                                                       colIdx);
                        }
                    }
                }
                else
                {
                    throw "Unsupported transformation type";
                }

                QTransform transform(h[0][0],h[1][0],h[2][0],
                                     h[0][1],h[1][1],h[2][1],
                                     h[0][2],h[1][2],h[2][2]);

                return transform;
            }

        }

        cv::Rect
        getRectangleInsideImage(const cv::Size &imageSize,
                                const cv::Rect &rect)
        {
            cv::Rect result = rect;
            result.width = qMax(0,
                                qMin(result.width,
                                  imageSize.width - result.x));
            result.height = qMax(0,
                                 qMin(result.height,
                                      imageSize.height - result.y));
            result.x = qBound(0,result.x,imageSize.width - 1);
            result.y = qBound(0,result.y,imageSize.height - 1);

            return result;
        }

        QPointF
        transformPoint(const cv::Mat &transform,
                       const QPointF &pt)
        {
            cv::Vec3d cvPt(pt.x(),
                           pt.y(),
                           1.0);

            cvPt = cv::Mat(transform * cv::Mat(cvPt));

            return QPointF(cvPt(0) / cvPt(2),
                           cvPt(1) / cvPt(2));
        }

        cv::Mat
        rotationMatrixFromVectors(const cv::Vec2d &vecFrom,
                                  const cv::Vec2d &vecTo)
        {
//            if(vecFrom == vecTo)
//            {
//                return cv::Mat::eye(2,2,CV_64FC1);
//            }
//            else if(vecFrom == (vecTo * -1))
//            {
//                return cv::Matx<double,2,2>(-1,0,
//                                            0,-1); // just flipping around
//            }
//            // derived from http://math.stackexchange.com/a/476311/161840
//            cv::Vec2d a = vecFrom / cv::norm(vecFrom);
//            cv::Vec2d b = vecFrom / cv::norm(vecTo);
//            cv::Vec2d v = a(0) * b(1) - a(1) * b(0);
//            double s = cv::norm(v);
//            double c = a.dot(b);


            //based upon http://stackoverflow.com/a/2946536/1603969
            if(areVecsEqual(vecFrom, vecTo))
            {
                return cv::Mat::eye(2,2,CV_64FC1);
            }
            else if(areVecsEqual(vecFrom,
                                 (vecTo * -1)))
            {
                return cv::Mat(cv::Matx<double,2,2>(-1,0,
                                            0,-1)).clone(); // just flipping around
            }
            else
            {
                double angle = angleFromVectors(vecFrom,
                                                vecTo);
                double cosAngle = std::cos(angle);
                double sinAngle = std::sin(angle);

                return cv::Mat(cv::Matx<double,2,2>(cosAngle,-sinAngle,
                                                   sinAngle,cosAngle)).clone();
            }


        }

        double
        angleFromVectors(const cv::Vec2d &vecFrom,
                        const cv::Vec2d &vecTo)
        {
            if(vecFrom == vecTo)
            {
                return 0;

            }
            else if(vecFrom == (vecTo * -1))
            {
                return M_PI;
            }
            else
            {

                return std::atan2(vecTo(1),
                                  vecTo(0)) -
                        std::atan2(vecFrom(1),
                                   vecFrom(0));
            }
        }

        cv::Mat
        transformFromLineSegments(const cv::Vec2d &line1start,
                                       const cv::Vec2d &line1end,
                                       const cv::Vec2d &line2start,
                                       const cv::Vec2d &line2end)
        {
            Eigen::Affine2d transform = Eigen::Affine2d::Identity();

            // transform to origin
            transform.pretranslate(Eigen::Vector2d(-line1start(0),
                                                   -line1start(1)));

            // find rotation
            cv::Vec2d vec1 = line1end - line1start;
            cv::Vec2d vec2 = line2end - line2start;
            double angle = angleFromVectors(vec1,
                                            vec2);
            transform.prerotate(Eigen::Rotation2Dd(angle));

            // find scale
            double scale = cv::norm(vec2) / cv::norm(vec1);
            transform.prescale(scale);

            // now transform to start of the line 2
            transform.pretranslate(Eigen::Vector2d(line2start(0),
                                                   line2start(1)));

            cv::Mat transformCV;
            cv::eigen2cv(transform.matrix(),
                         transformCV);
//            std::cerr << "transformFromLineSegments" <<
//                      line1start <<
//                         line2start <<
//                         angle << scale << std::endl <<
//                         transformCV << std::endl;

            return transformCV;

        }

        cv::Point2d
        getNormalVector(const cv::Point2d &pt1,
                        const cv::Point2d &pt2)
        {
            return cv::Point2d(pt1.y - pt2.y,
                               pt2.x - pt1.x);
        }

        cv::Point2d
        getUnitNormalVector(const cv::Point2d &pt1,
                            const cv::Point2d &pt2)
        {
            if(pt1 == pt2)
            {
               return cv::Point2d(0,0);
            }
            else
            {
                cv::Point2d result = getNormalVector(pt1,
                                                     pt2);
                double norm = cv::norm(result);
                return cv::Point2d(result.x / norm,
                                   result.y / norm);
            }
        }

        cv::Point2d
        getNearestPointOnLineSegment(const cv::Point2d &pt1,
                                     const cv::Point2d &pt2,
                                     const cv::Point2d &testPoint)
        {
            // based on http://www.ogre3d.org/tikiwiki/Nearest+point+on+a+line
            const cv::Point2d result = getNearestPointOnLine(pt1,
                                                    pt2,
                                                    testPoint);
            const double lineLength = squaredNorm(pt1 -
                                               pt2);

            const double p1R = squaredNorm(pt1 -
                                        result);
            const double p2R = squaredNorm(pt2 -
                                        result);
                //R                R p1 R p2 R                R
            if ( p1R > lineLength )
            {
                if ( p2R > p1R )
                    return pt1; //pt 1 is closer to result
                else
                    return pt2;
            }
            else if ( p2R > lineLength )
                return pt1;

            return result;
        }

        cv::Point2d
        getNearestPointOnLine(const cv::Point2d &linePt1,
                              const cv::Point2d &linePt2,
                              const cv::Point2d &testPoint)
        {
            // based on http://www.ogre3d.org/tikiwiki/Nearest+point+on+a+line
            const cv::Point2d A = testPoint  - linePt1;
            const cv::Point2d u = (linePt2 - linePt1);

            return linePt1 + (A.dot(u) * u);
        }


        cv::Mat
        estimateProjectiveTransform(const std::vector<cv::Point2d> &src,
                                    const std::vector<cv::Point2d> &dst)
        {
            assert(src.size() == dst.size());

            if(src.size() == 0)
            {
                return cv::Mat::eye(3,3,
                                    CV_64FC1);
            }
            else if(src.size() == 1)
            {
                // just shift
                Eigen::Affine2d transform = Eigen::Affine2d::Identity();
                transform.pretranslate(Eigen::Vector2d(dst[0].x - src[0].x,
                                                       dst[0].y - src[0].y));

                cv::Mat transformCV;
                cv::eigen2cv(transform.matrix(),
                             transformCV);
    //            std::cerr << "transformFromLineSegments" <<
    //                      line1start <<
    //                         line2start <<
    //                         angle << scale << std::endl <<
    //                         transformCV << std::endl;

                return transformCV;
            }
            else if(src.size() == 2)
            {
                // transform of line segments
                return transformFromLineSegments(cv::Vec2d(src[0].x, src[0].y),
                        cv::Vec2d(src[1].x, src[1].y),
                        cv::Vec2d(dst[0].x, dst[0].y),
                        cv::Vec2d(dst[1].x, dst[1].y));
            }
            else if(src.size() == 3)
            {
                // estimate affine transform
                return get3x3TransformMatrix(cv::estimateRigidTransform(src, dst,
                                                  true));
            }
            else
            {
                // estimate perspective
                return cv::findHomography(src, dst);
            }
        }



        cv::Point2d
        getCentroid(const std::vector<cv::Point2d> &src)
        {
            cv::Point2d sumPoint(0,0);
            for(int i = 0;
                i < src.size();
                ++i)
            {
                sumPoint += src[i];
            }

            return sumPoint * (1.0 / src.size());
        }

        double
        linearInterpolate(double x0,
                          double x1,
                          double fx0,
                          double fx1,
                          double x)
        {
            assert(x0 != x1);

            return fx0 + ((fx1 - fx0)/(x1 - x0)) * (x - x0);
        }

        cv::Mat
        alignUnitVectorsByRotation(const cv::Vec3d &a,
                                   const cv::Vec3d &b)
        {
            // derived from Kuba Ober answer at http://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d/180436
            if(a == b)
            {
                return cv::Mat::eye(3,3,
                                    CV_64FC1);
            }
            else
            {
                double dotAB = a.dot(b);
                cv::Vec3d crossAB = a.cross(b);
                cv::Vec3d crossBA = -crossAB;//b.cross(a);
                double normCrossAb = cv::norm(crossAB,
                                              cv::NORM_L2);

                if(normCrossAb <= std::numeric_limits<double>::epsilon())
                {
                    return calculateVectorOpositeRotation(a);
                }
                else
                {

                    cv::Vec3d v = b - ((dotAB) * a);
                    double normV = cv::norm(v,
                                            cv::NORM_L2);
                    v = v * (1.0 / normV);

                    cv::Mat GG = cv::Mat::eye(3,3,
                                              CV_64FC1);

                    GG.at<double>(0,0) = dotAB;
                    GG.at<double>(0,1) = -normCrossAb;
                    GG.at<double>(1,0) = normCrossAb;
                    GG.at<double>(1,1) = dotAB;

                    cv::Mat FFi(3,3,
                                CV_64FC1);
                    FFi.at<double>(0,0) = a(0);
                    FFi.at<double>(1,0) = a(1);
                    FFi.at<double>(2,0) = a(2);

                    FFi.at<double>(0,1) = v(0);
                    FFi.at<double>(1,1) = v(1);
                    FFi.at<double>(2,1) = v(2);

                    FFi.at<double>(0,2) = crossBA(0);
                    FFi.at<double>(1,2) = crossBA(1);
                    FFi.at<double>(2,2) = crossBA(2);

                    return FFi * GG * FFi.inv();
                }
            }
        }

        cv::Vec3d calculateEulerAnglesFromRotation(const cv::Mat &rotationMatrix)
        {

#ifdef RCE_USE_EIGEN_EULER_ANGLES
            Eigen::Matrix3d mat;
            cv::cv2eigen(rotationMatrix,
                         mat);
            Eigen::Vector3d ea = mat.eulerAngles(2,1,0);
            return cv::Vec3d(ea(2),
                             ea(1),
                             ea(0));
#else
            // based upon http://www.staff.city.ac.uk/~sbbh653/publications/euler.pdf
            if((rotationMatrix.at<double>(2,0) != 1) &&
               (rotationMatrix.at<double>(2,0) != -1))
            {
                double theta1 = std::asin(rotationMatrix.at<double>(2,0));
                double theta2 = M_PI - theta1;
                double cosTheta1 = cos(theta1);
                double cosTheta2 = cos(theta2);
                double psi1 = atan2(rotationMatrix.at<double>(2,1) / cosTheta1,
                                    rotationMatrix.at<double>(2,2) / cosTheta1);
                double psi2 = atan2(rotationMatrix.at<double>(2,1) / cosTheta2,
                                    rotationMatrix.at<double>(2,2) / cosTheta2);
                double phi1 = atan2(rotationMatrix.at<double>(1,0) / cosTheta1,
                                    rotationMatrix.at<double>(0,0) / cosTheta1);
                double phi2 = atan2(rotationMatrix.at<double>(1,0) / cosTheta2,
                                    rotationMatrix.at<double>(0,0) / cosTheta2);

                double sol1Rank = std::abs(psi1) +
                                  std::abs(phi1) +
                                  std::abs(theta1);
                double sol2Rank = std::abs(psi2) +
                                  std::abs(phi2) +
                                  std::abs(theta2);
                if(sol2Rank > sol1Rank)
                {
                    return cv::Vec3d(psi1,
                                     theta1,
                                     phi1);
                }
                else
                {
                    return cv::Vec3d(psi2,
                                     theta2,
                                     phi2);
                }
            }
            else
            {
                double phi = 0;

                if(rotationMatrix.at<double>(2,0) != -1)
                {
                    double theta = M_PI / 2.0;
                    double psi = phi + atan2(rotationMatrix.at<double>(0,1),
                                             rotationMatrix.at<double>(0,2));
                    return cv::Vec3d(psi,
                                     theta,
                                     phi);
                }
                else
                {
                    double theta = -M_PI / 2.0;
                    double psi = -phi + atan2(-rotationMatrix.at<double>(0,1),
                                             -rotationMatrix.at<double>(0,2));
                    return cv::Vec3d(psi,
                                     theta,
                                     phi);

                }
            }
#endif
        }

        cv::Mat
        buildRotationMatrixFromEulerAngles(const cv::Vec3d &eulerAngles)
        {
            double psi = eulerAngles(0);
            double theta = eulerAngles(1);
            double phi = eulerAngles(2);

            Eigen::Matrix3d m;
            m = Eigen::AngleAxisd(phi,
                                  Eigen::Vector3d::UnitZ()) *
                Eigen::AngleAxisd(theta,
                                                  Eigen::Vector3d::UnitY()) *
                Eigen::AngleAxisd(psi,
                                                  Eigen::Vector3d::UnitX());

            cv::Mat result;
            cv::eigen2cv(m, result);
            return result;


        }

        cv::Mat
        calculateVectorOpositeRotation(const cv::Vec3d &vec)
        {
            cv::Vec3d orthogonalVector;
            if(vec(0) != 0)
            {
                orthogonalVector(0) = -vec(1);
                orthogonalVector(1) = vec(0);
                orthogonalVector(2) = 0;
            }
            else
            {

                orthogonalVector(0) = 0;
                orthogonalVector(1) = -vec(2);
                orthogonalVector(2) = vec(1);
            }

            // now rotate stuff around this orthogonal vector
            Eigen::Matrix3d m;
            m = Eigen::AngleAxisd(M_PI,
                                  Eigen::Vector3d(orthogonalVector(0),
                                                  orthogonalVector(1),
                                                  orthogonalVector(2))).matrix();

            cv::Mat result;
            cv::eigen2cv(m, result);
            return result;
        }

        cv::Mat
        composeRotationMatrix(const cv::Vec3d &r1,
                              const cv::Vec3d &r2,
                              const cv::Vec3d &r3)
        {
            Eigen::Matrix3d r;
            r(0,0) = r1(0);
            r(1,0) = r1(1);
            r(2,0) = r1(2);
            r(0,1) = r2(0);
            r(1,1) = r2(1);
            r(2,1) = r2(2);
            r(0,2) = r3(0);
            r(1,2) = r3(1);
            r(2,2) = r3(2);

            Eigen::Projective3d t = Eigen::Projective3d::Identity();
            t.linear() = r;
            r = t.rotation(); // calculates rotation using SVD

            cv::Mat rCv;
            cv::eigen2cv(r, rCv);
            return rCv;
        }

        void
        invertTranslationAndRotation(const cv::Point3d &t,
                                     const cv::Mat &r,
                                     cv::Point3d &tInv,
                                     cv::Mat &rInv)
        {
            // according to: http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche53.html
            rInv = r.t();
            cv::Vec3d tVec(t.x,
                           t.y,
                           t.z);
            cv::Vec3d tInvVec = cv::Mat(-rInv * cv::Mat(tVec));
            tInv.x = tInvVec(0);
            tInv.y = tInvVec(1);
            tInv.z = tInvVec(2);
        }

        cv::Point3d
        transformPoint(const cv::Point3d &p,
                       const cv::Point3d &t,
                       const cv::Mat &rot)
        {
            cv::Vec3d resVec = transformVector(cv::Vec3d(p.x,
                                                         p.y,
                                                         p.z),
                                               cv::Vec3d(t.x,
                                                         t.y,
                                                         t.z),
                                               rot);
            return cv::Point3d(resVec(0),
                               resVec(1),
                               resVec(2));
        }

        cv::Vec3d
        transformVector(const cv::Vec3d &v,
                        const cv::Vec3d &t,
                        const cv::Mat &rot)
        {
            cv::Vec3d res = cv::Mat(rot * cv::Mat(v));
            return res + t;
        }

        cv::Point2d
        transformPoint(const cv::Point2d &p,
                       const cv::Point2d &t,
                       const cv::Mat &rot)
        {
            cv::Vec2d resVec = transformVector(cv::Vec2d(p.x,
                                                         p.y),
                                               cv::Vec2d(t.x,
                                                         t.y),
                                               rot);
            return cv::Point2d(resVec(0),
                               resVec(1));
        }

        cv::Vec2d
        transformVector(const cv::Vec2d &v,
                        const cv::Vec2d &t,
                        const cv::Mat &rot)
        {
            cv::Vec2d res = cv::Mat(rot * cv::Mat(v));
            return res + t;
        }

        cv::Mat
        get2DRotationMatrix(double angle)
        {
            cv::Mat_<double> rot(2,2);
            double angleCos = std::cos(angle);
            double angleSin = std::sin(angle);
            rot(0,0) = angleCos;
            rot(0,1) = -angleSin;
            rot(1,0) = angleSin;
            rot(1,1) = angleCos;

            return rot;
        }

        cv::Point2d
        rotatePointAround(const cv::Point2d &pt,
                          const cv::Mat &rotation,
                          const cv::Point2d &pivot)
        {
            return transformPoint(pt - pivot,
                                  pivot,
                                  rotation);
        }

        double normalizeAngle(double angle)
        {
            while(angle < 0.0)
            {
                angle += 2 * M_PI;
            }

            while(angle >= (2 * M_PI))
            {
                angle -= 2 * M_PI;
            }

            return angle;
        }

        QPainterPath
        calculateIntersectionProjective(const QPainterPath &thisViewPath,
                                        const QPainterPath &otherViewPath,
                                        const QTransform &otherTransform)
        {
            if(otherTransform.isIdentity())
            {
                return thisViewPath.united(otherViewPath);
            }
            else
            {
                cv::Mat transform = qTransform2CV(otherTransform);
                return calculateIntersectionProjective(thisViewPath,
                                                otherViewPath,
                                                transform);
            }
        }


        QPainterPath
        calculateIntersectionProjective(const QPainterPath &thisViewPath,
                                        const QPainterPath &otherViewPath,
                                        const cv::Mat &otherTransform)
        {
            if(otherTransform.empty())
            {
                return thisViewPath.united(otherViewPath);
            }

            QList<QPolygonF> polygons = otherViewPath.toFillPolygons();
            // here should come something like frustum clipping
            /*
             * Do it like this:
             * We know that horizon is at place where w in homogeneous coordinates is 0,
             * therefore, the line of horizon is given as h31*x + h32*y + h33 = 0.
             *
             * Now we may assume that some point (i.e. at the bottom of the view) is ground.
             * So we try to transform it, and see the sign of W. All valid points have the same
             * sign. (are on the correct side of the horizon)
             *
             * */
            QPainterPath result;
            for(int i = 0;
                i < polygons.size();
                ++i)
            {
                for(int j = 0;
                    j < polygons[i].size();
                    ++j)
                {
                    cv::Vec3d v(polygons[i][j].x(),
                                polygons[i][j].y(),
                                1);
                    v = cv::Mat(otherTransform * cv::Mat(otherTransform));
                    polygons[i][j].setX(v(0) / v(2));
                    polygons[i][j].setY(v(1) / v(2));
                }
                result.addPolygon(polygons[i]);
            }

            return result;
        }

        cv::Mat qTransform2CV(const QTransform &trans)
        {
            cv::Mat result(3,3,
                           CV_64FC1);
            result.at<double>(0,0) = trans.m11();
            result.at<double>(0,1) = trans.m21();
            result.at<double>(0,2) = trans.m31();
            result.at<double>(1,0) = trans.m12();
            result.at<double>(1,1) = trans.m22();
            result.at<double>(1,2) = trans.m32();
            result.at<double>(2,0) = trans.m13();
            result.at<double>(2,1) = trans.m23();
            result.at<double>(2,2) = trans.m33();

            return result;
        }

        bool
        getHorizonFromTransformation(const cv::Mat &transform,
                                     double &a,
                                     double &b,
                                     double &c)
        {
            if(transform.empty())
            {
                return false;
            }
            else
            {
                a = transform.at<double>(2,0);
                b = transform.at<double>(2,1);
                c = transform.at<double>(2,2);
                if((a == 0) && (b == 0))
                { // there is no horizon line (so it is an affine transform)
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }

        bool
        cutRectangleByLine(const QRectF &rectangle,
                           double a,
                           double b,
                           double c,
                           QPolygonF &part1,
                           QPolygonF &part2)
        {
            part1.clear();
            part2.clear();
            if(a == 0)
            {
                // the line as at coordinate y = -b/c
                double lineY = -b/c;
                if((rectangle.top() > lineY) &&
                   (rectangle.bottom() < lineY))
                {

                    part1.push_back(rectangle.bottomLeft());
                    part1.push_back(rectangle.bottomRight());
                    part1.push_back(QPointF(rectangle.right(),
                                            lineY));
                    part1.push_back(QPointF(rectangle.left(),
                                            lineY));

                    part2.push_back(part1[3]);
                    part2.push_back(part1[2]);
                    part2.push_back(rectangle.topRight());
                    part2.push_back(rectangle.topLeft());


                    return true;

                }
                else
                {
                    return false;
                }
            }
            else // a is nonzero, lets use it!
            {
                double topCrossX = getLineX(rectangle.top(),
                                          a,b,c);
                double bottomCrossX = getLineX(rectangle.bottom(),
                                              a,b,c);
                if((topCrossX <= rectangle.left()) && (bottomCrossX <= rectangle.left()))
                {  // line is to the left
                    return false;
                }
                if((topCrossX >= rectangle.right()) && (bottomCrossX >= rectangle.right()))
                { // line is to the right
                    return false;
                }


                // there must be two crossings
                bool appendToPart1 = true;
                part1.append(rectangle.bottomLeft()); // start at bottom left
                // check crossing bottom line
                if((bottomCrossX > rectangle.left()) && (bottomCrossX <= rectangle.right()))
                {
                    // there is crossing!
                    part1.push_back(QPointF(bottomCrossX,
                                            rectangle.bottom()));
                    appendToPart1 = false;
                    part2.push_back(QPointF(bottomCrossX,
                                            rectangle.bottom()));
                }

                if(appendToPart1)
                {
                    part1.push_back(rectangle.bottomRight());
                }
                else
                {
                    part2.push_back(rectangle.bottomRight());
                }
                // check crossing right line
                if((((bottomCrossX - rectangle.right()) * (topCrossX - rectangle.right())) <= 0) && // this will be smaller than/equal 0, if line is crossed
                   (bottomCrossX != rectangle.right())) // just check to not identify the same crossing twice.
                {
                    part1.push_back(QPointF(rectangle.right(),
                                    getLineY(rectangle.right(),
                                             a,b,c)));
                    part2.push_back(part1.last());
                    appendToPart1 = !appendToPart1;
                }

                if(appendToPart1)
                {
                    part1.push_back(rectangle.topRight());
                }
                else
                {
                    part2.push_back(rectangle.topRight());
                }
                // check crossing top
                if((topCrossX >= rectangle.left()) && (topCrossX < rectangle.right()))
                {
                    part1.push_back(QPointF(topCrossX,
                                            rectangle.top()));
                    appendToPart1 = !appendToPart1;
                    part2.push_back(QPointF(topCrossX,
                                            rectangle.top()));

                }

                if(appendToPart1)
                {
                    part1.push_back(rectangle.topLeft());
                }
                else
                {
                    part2.push_back(rectangle.topLeft());
                }

                // check crossing left line
                if((((bottomCrossX - rectangle.left()) * (topCrossX - rectangle.left())) <= 0) && // this will be smaller than/equal 0, if line is crossed
                   (topCrossX != rectangle.left())) // just check to not identify the same crossing twice.
                {
                    part1.push_back(QPointF(rectangle.left(),
                                    getLineY(rectangle.left(),
                                             a,b,c)));
                    part2.push_back(part1.last());
                    appendToPart1 = !appendToPart1;
                }

                return true;

            }
        }

        double
        getLineY(double x,
                 double a,
                 double b,
                 double c)
        {
            // ax + yb +c = 0
            // y = -(ax + c)/b
            return -(a*x + c) / b;
        }

        double
        getLineX(double y,
                 double a,
                 double b,
                 double c)
        {
            // ax + yb +c = 0
            // x = -(yb + c)/a
            return -(y*b + c) / a;
        }

        QPointF
        getPolygonApproxCentre(const QPolygonF &polygon)
        {
            if(polygon.size() == 0)
            {
                return QPointF();
            }
            else
            {
                QPointF result(0,0);
                foreach(QPointF pt, polygon)
                {
                    result += pt;
                }

                return result * (1.0 / polygon.size());
            }
        }

        QPolygonF
        simplePolylineIntersection(const QPolygonF &polyline,
                                   const QPolygonF &polygon)
        {
            if(polyline.isEmpty())
            {
                return QPolygonF();
            }

            QPolygonF result;
            bool isInside = polygon.containsPoint(polyline.first(),
                                                  Qt::OddEvenFill);
            if(isInside)
            {
                result.push_back(polyline.first());
            }
            QPointF intersectionPoint;
            for(int i = 1;
                i < polyline.size();
                ++i)
            {
                QLineF lineSeg(polyline[i-1],
                        polyline[i]);
                QVector<QPointF> crossings;
                for(int j = 1;
                    j < polygon.size();
                    ++j)
                {
                    QLineF polSeg(polygon[j-1],
                            polygon[j]);
                    switch(lineSeg.intersect(polSeg,
                                      &intersectionPoint))
                    {
                        case QLineF::NoIntersection:
                            break;
                        case QLineF::BoundedIntersection:
                            if(intersectionPoint != lineSeg.p1())
                            {
                                crossings.push_back(intersectionPoint);
                            }
                            break;
                        case QLineF::UnboundedIntersection:
                            break;

                    }
                }

                for(int k = 0;
                    k < crossings.size();
                    ++k)
                {
                    result.push_back(crossings[k]);
                    if(isInside)
                    {
                        return result;
                    }
                }


                if(isInside)
                {
                    result.push_back(polyline[i]);
                }
            }

            return result;
        }

        QPolygonF
        simplePolygonIntersection(const QPolygonF &polygon,
                                  const QPainterPath &path)
        {
            QPainterPath pOther;
            pOther.addPolygon(polygon);
            return pOther.intersected(path).toFillPolygon();
        }

        cv::Point2d
        calculateNadirPoint(const cv::Mat &fromWorldToNormImgTransform,
                            const cv::Mat &_fromNormImgToWorldTransform)
        {
            // see https://bitbucket.org/hermandav/datafromsky/wiki/Nadir%20point%20calculation for explanation
            // prepare params
            cv::Mat fromNormImgToWorldTransform;
            if(_fromNormImgToWorldTransform.empty())
            {
                fromNormImgToWorldTransform = fromWorldToNormImgTransform.inv();
            }
            else
            {
                fromNormImgToWorldTransform = _fromNormImgToWorldTransform;
            }
            // firstly, check, whether there is a horizon in image and world space
            // horizon line is: (ax + by + c = 0)
            double iA, iB, iC, wA,wB,wC;
            if((!getHorizonFromTransformation(fromNormImgToWorldTransform,
                                             iA,iB,iC)) ||
               (!getHorizonFromTransformation(fromWorldToNormImgTransform,
                                              wA, wB, wC)))
            {
                // there is no horizon, therefore, it is affine transform
                // which means, camera is looking straigth down
                cv::Vec3d pp(0,0,1);
                cv::Vec3d result = cv::Mat(fromNormImgToWorldTransform * cv::Mat(pp));
                return cv::Point2d(result(0) / result(2),
                                   result(1) / result(2));
            }

            // so, there is a horizon
            // check, whether PP is at horizon (ergo, 0 *a + 0 * b + c ?==? 0)
            if(iC == 0)
            {
                // PP is at horizon
                // so nadir is at intersection of horizon in world space and line that
                // is projection of line that is perpendicular to image horizon and passes through PP

                // get perpendicular line that goes through PP (0,0).
                double perpA = -iB;
                double perpB = iA;
                double perpC = 0; // To cross 0,0, it must be that c = 0;

                // transform this line to world (see Multiple View Geometry, equation (2.6))
                cv::Vec3d perpLine(perpA, perpB, perpC);
                cv::Vec3d worldPerpLine = cv::Mat(fromWorldToNormImgTransform.t() * cv::Mat(perpLine));

                // find intersection (cross product)
                cv::Vec3d worldHorizon(wA, wB, wC);
                cv::Vec3d result = worldHorizon.cross(worldPerpLine);

                // and that is nadir point
                return cv::Point2d(result(0) / result(2),
                                   result(1) / result(2));

            }
            else
            {
                // PP is not at horizon
                // normal vector
                cv::Point2d horizNormal(iA,
                                        iB);
                if(iC < 0)
                {
                    // flipping normal, if the line is on the other side of (0,0)
                    horizNormal = horizNormal * (-1);
                }

                // normalises the length of the vector
                horizNormal = horizNormal * (1.0 / cv::norm(horizNormal));

                // distance of PP from horizon
                double distPP2H = getDistanceFromLine(iA,iB,iC,
                                                      0,0);

                double distPP2NADIR = 1 / distPP2H;
                cv::Point2d nadirImage = horizNormal * distPP2NADIR;

                // now transform the image to world coordinates
                cv::Vec3d nadirImageVec(nadirImage.x,
                                        nadirImage.y,
                                        1);
                cv::Vec3d result = cv::Mat(fromNormImgToWorldTransform * cv::Mat(nadirImageVec));

                // and that is nadir point
                return cv::Point2d(result(0) / result(2),
                                   result(1) / result(2));
            }

        }

        double
        getDistanceFromLine(double a, double b, double c,
                            double x0, double y0)
        {
            // https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
            return (std::abs(a * x0 + b * y0 + c)/ (std::sqrt(a*a + b*b)));
        }

        cv::Mat
        composeProjectionMatrix(const cv::Mat &cameraMatrix,
                                const cv::Mat &rotation,
                                const cv::Point3d &translation,
                                const cv::Mat &residualTransform,
                                bool leftHandedCS)
        {
            cv::Mat rotInv;
            cv::Point3d translInv;
            invertTranslationAndRotation(translation, rotation,
                                         translInv, rotInv);

            cv::Mat Rt(3,4, CV_64FC1);
            rotInv.copyTo(Rt.colRange(0,3));
            Rt.at<double>(0,3) = translInv.x;
            Rt.at<double>(1,3) = translInv.y;
            Rt.at<double>(2,3) = translInv.z;

            if(!residualTransform.empty())
            {
                Rt = residualTransform * Rt;
            }

            cv::Mat P = cameraMatrix * Rt;


            if(leftHandedCS)
            {
                // we assume that final image is flipped
                P.row(1) = P.row(1) * (-1);
            }

            return P;
        }

        cv::Mat
        getResidualTransform(const cv::Mat &cameraMatrixInv,
                             const cv::Mat &rotation,
                             const cv::Point3d &translation,
                             const cv::Mat &homography,
                             bool leftHandedCS)
        {
            cv::Mat rotInv;
            cv::Point3d translInv;
            invertTranslationAndRotation(translation, rotation,
                                         translInv, rotInv);

            cv::Mat cleanH(3,3, CV_64FC1);
            rotInv.col(0).copyTo(cleanH.col(0));
            rotInv.col(1).copyTo(cleanH.col(1));
            cleanH.at<double>(0,2) = translInv.x;
            cleanH.at<double>(1,2) = translInv.y;
            cleanH.at<double>(2,2) = translInv.z;


            cv::Mat dirtyH = homography.clone();
            if(leftHandedCS)
            {
                // we assume that final image is flipped
                dirtyH.row(1) = dirtyH.row(1) * (-1);
            }
            dirtyH = cameraMatrixInv * dirtyH;

            return cleanH.inv() * dirtyH;

        }

        double calculatePolygonArea(const QPolygonF &polygon)
        {
            if(polygon.size() < 3)
            {
                 return 0;
            }
            // based on http://alienryderflex.com/polygon_area/
            double area = 0;
            int i, j=polygon.size()-1  ;

            for (i=0; i<polygon.size(); i++)
            {
                area+=(polygon[j].x()+polygon[i].x())
                      *(polygon[j].y()-polygon[i].y());
                j=i;
            }

            return std::abs(area*0.5);
        }

        cv::Mat createTranslationTransform(double x, double y)
        {
            cv::Mat result = cv::Mat::eye(3,3, CV_64FC1);
            result.at<double>(0,2) = x;
            result.at<double>(1,2) = y;

            return result;

        }

        cv::Mat createRotationTransform(double angle, double xPivot, double yPivot)
        {
            if((xPivot == 0) && (yPivot == 0))
            {
                return createRotationTransform(angle);
            }
            else
            {
                return createTranslationTransform(xPivot, yPivot) *
                        createRotationTransform(angle) *
                        createTranslationTransform(-xPivot, -yPivot);
            }
        }

        cv::Mat createRotationTransform(double angle)
        {
            Eigen::Isometry2d rotationFix = Eigen::Isometry2d::Identity();
            rotationFix.rotate(angle);
            cv::Mat result;
            cv::eigen2cv(rotationFix.matrix(),
                         result);

            return result;

        }

        cv::Mat
        createScaleTransform(double xScale,
                             double yScale)
        {
            cv::Mat result = cv::Mat::eye(3,3, CV_64FC1);
            result.at<double>(0,0) = xScale;
            result.at<double>(1,1) = yScale;

            return result;
        }

        QPointF
        calculatePrespectiveRectangleCenter(const QPolygonF &poly)
        {
            QLineF diag0(poly[0],
                    poly[2]);
            QLineF diag1(poly[1],
                    poly[3]);

            QPointF result;
            diag0.intersect(diag1, &result);
            return result;
        }



//        void
//        calculateGridPoints(const cv::Point2d &pt1,
//                            const cv::Point2d &pt2,
//                            std::vector<cv::Point2d> &gridPoints)
//        {
//            gridPoints.clear();
//            if(pt1 == pt2)
//            {
//                return;
//            }
//            else
//            {
//                double diffX = pt2.x - pt1.x;
//                double diffY = pt2.y - pt1.y;
//                double absDiffX = abs(diffX);
//                double absDiffY = abs(diffY);

//                if(absDiffX > absDiffY)
//                {
//                    double deltaY = diffY / diffX;
//                    double firstXStep;
//                    double deltaX;
//                    if(diffX > 0)
//                    {
//                        deltaX = 1;
//                        firstXStep = std::ceil(pt1.x) - pt1.x;
//                    }
//                    else
//                    {
//                        deltaX = -1;
//                        firstXStep = std::floor(pt1.x) - pt1.x;
//                    }

//                    if(abs(firstXStep) < 0.5)
//                    {
//                        firstXStep += deltaX;
//                    }

//                    double xF = firstXStep;
//                    double yF = firstXStep * deltaY;
//                    double xEnd = qRound(pt2.x) - 0.5 * deltaX;
//                    for(double x = qRound(pt1.x + deltaX);
//                        x < xEnd;
//                        x += deltaX)
//                    {


//                        xF += deltaX;
//                        yF += deltaY;
//                    }


//                }
//            }
//        }

    } // namespace geometry
} // namespace rce
