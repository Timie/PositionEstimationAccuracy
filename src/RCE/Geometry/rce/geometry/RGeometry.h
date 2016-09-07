#ifndef RCE_GEOMETRY_RGEOMETRY_H
#define RCE_GEOMETRY_RGEOMETRY_H

#include <math.h>

#include <opencv2/core/core.hpp>
#include <QtGlobal>
#include <QRectF>
#include <QTransform>


class QPolygonF;



namespace rce {
    namespace geometry {


        /** Unit conversion constants: */
        const double INCHES_PER_FOOT = 12.0;
        const double CM_PER_INCH = 2.54;
        const double CM_PER_METER = 100.0;
        const double METERS_PER_FOOT = INCHES_PER_FOOT * CM_PER_INCH / CM_PER_METER; // = 0.3048
        const double MMETERS_PER_KM =	1000000.0;
        const double MMETERS_PER_MILE =	1609344.0;
        const double METERS_PER_MILE =	MMETERS_PER_MILE / 1000.0;
        const long   SECONDS_PER_MINUTE = 60;
        const long   MINUTES_PER_HOUR =	60;
        const long   SECONDS_PER_HOUR =	SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
        const double RADIANS_PER_DEGREE = M_PI/180.0;
        const double DEGREES_PER_RADIAN = 180.0/M_PI;

        /** Useful constants **/
        const double TWOPI = 2.0 *
                             M_PI;
        const double HALFPI = M_PI / 2.0;
        const double RCE_INV_SQRT_2 = 0.7071067811865475244008443621048490392848359376884740; // sqrt(2) / 2

        cv::Mat get4x4TransformMatrix(const cv::Mat &input);
        cv::Mat get3x3TransformMatrix(const cv::Mat &input);

        // USE RCameraPositionEstimation class instead!!!
        // H = M * D * RT, where RT is 1st, 2nd and 4th column of pose ([R|t])
        // distortion matrix D should consist only of shear and dilation in y axis... (currently not assumed)
        cv::Mat estimatePoseFromHomography(const cv::Mat &homography,
                                           const cv::Mat &invCameraMatrix);

        cv::Mat getHomographyFromPose(const cv::Mat &cameraPosition,
                                      const cv::Mat &projectionMatrix);

        template<typename ScalarType>
        cv::Point_<ScalarType> rectangleCentre(const cv::Rect_<ScalarType> &rect)
        {
            return cv::Point_<ScalarType>(rect.x + rect.width / static_cast<ScalarType>(2.0),
                                          rect.y + rect.height / static_cast<ScalarType>(2.0));
        }

        // specialized version
        template<int>
        cv::Point_<int> rectangleCentre(const cv::Rect_<int> &rect)
        {
            return cv::Point_<int>(qRound(rect.x + rect.width / 2.0),
                                          qRound(rect.y + rect.height / 2.0));
        }


        template <typename VecType, int DIMS>
        bool areVecsEqual(const cv::Vec<VecType, DIMS> &vec1,
                          const cv::Vec<VecType, DIMS> &vec2)
        {
            for(int i = 0;
                i < DIMS;
                ++i)
            {
                if(vec1(i) != vec2(i))
                {
                    return false;
                }
            }

            return true;
        }

        template<typename ScalarType>
        cv::Point2d rectangleCentreF(const cv::Rect_<ScalarType> &rect)
        {

            return cv::Point2d(rect.x + rect.width / 2.0,
                                rect.y + rect.height / 2.0);
        }

        static inline bool isRectangleInsideImage(const cv::Rect &area,
                                    const cv::Mat &image)
        {
            return ((area.x >= 0) && ((area.x + area.width) < image.cols) &&
                    (area.y >= 0) && ((area.y + area.height) < image.rows));
        }

        template<typename ScalarType1,
                 typename ScalarType2>
        static inline bool isPointInsideSize(const cv::Point_<ScalarType1> &pt,
                                             const cv::Size_<ScalarType2> &size)
        {
            return ((pt.x >= 0) &&
                    (pt.y >= 0) &&
                    (pt.x < size.width) &&
                    (pt.y < size.height));
        }

        static inline bool isPointValidInMask(const cv::Point &pt,
                                              const cv::Mat &mask)
        {
            return mask.empty() ||
                    ((pt.x >= 0) && (pt.y >= 0) &&
                    (pt.x < mask.cols) && (pt.y < mask.rows) &&
                    (mask.at<uchar>(pt) != 0));
        }

        double getAngleAverageDeg(double angle1,
                                  double angle2);
        double normalizeAngle(double angle);
        double normalizeAngleDeg(double angle);
        double angleAbsDiff(double angle1,
                            double angle2);


        static inline double getRectangleDifference(const QRectF &r1,
                                                    const QRectF &r2)
        {
            QRectF intersection = r1 & r2;

            double intersectionArea = intersection.width() * intersection.height();

            if(intersectionArea < std::numeric_limits<double>::epsilon())
            {
                return 1.0;
            }
            else
            {

                double r1Area = r1.width() * r1.height();
                double r2Area = r2.width() * r2.height();

                return ((r1Area + r2Area - 2*intersectionArea) / (r1Area + r2Area));
            }
        }

        QTransform cvMatToQTransform(const cv::Mat &cvTransform);



        cv::Rect getRectangleInsideImage(const cv::Size &imageSize,
                                         const cv::Rect &rect);


        QPointF transformPoint(const cv::Mat &transform,
                               const QPointF &pt);

        cv::Mat rotationMatrixFromVectors(const cv::Vec2d &vecFrom,
                                          const cv::Vec2d &vecTo);

        double angleFromVectors(const cv::Vec2d &vecFrom,
                               const cv::Vec2d &vecTo);

        // maps line1 to line2
        cv::Mat transformFromLineSegments(const cv::Vec2d &line1start,
                                               const cv::Vec2d &line1end,
                                               const cv::Vec2d &line2start,
                                               const cv::Vec2d &line2end);

        // should be always counterclockwise (in counterclockwise frame)
        cv::Point2d getNormalVector(const cv::Point2d &pt1,
                                    const cv::Point2d &pt2);
        // should be always counterclockwise (in counterclockwise frame)
        // return zero vector when pt1 == pt2
        cv::Point2d getUnitNormalVector(const cv::Point2d &pt1,
                                    const cv::Point2d &pt2);

        cv::Point2d getNearestPointOnLineSegment(const cv::Point2d &linePt1,
                                                 const cv::Point2d &linePt2,
                                                 const cv::Point2d &pt);

        cv::Point2d getNearestPointOnLine(const cv::Point2d &linePt1,
                                                 const cv::Point2d &linePt2,
                                                 const cv::Point2d &pt);

        cv::Mat getResidualTransform(const cv::Mat &cameraMatrixInv, //3*3
                                     const cv::Mat &rotation, // rotation of camera 3*3
                                     const cv::Point3d &translation, // translation of camera
                                     const cv::Mat &homography,
                                     bool leftHandedCS);
        // returns 3*4 projection matrix
        cv::Mat composeProjectionMatrix(const cv::Mat &cameraMatrix, //3*3
                                        const cv::Mat &rotation, // rotation of camera 3*3
                                        const cv::Point3d &translation, // translation of camera
                                        const cv::Mat &residualTransform = cv::Mat(),
                                        bool leftHandedCS = false); // transform to be added between K and space transformation


        cv::Mat estimateProjectiveTransform(const std::vector<cv::Point2d> &src,
                                       const std::vector<cv::Point2d> &dst);

        // assumes line throught (x0,fx0) and (x1,fx1), and evaluates
        // x2 at this line.
        // does not work, when x0 == x1
        double linearInterpolate(double x0, double x1,
                                 double fx0, double fx1,
                                 double x);

        cv::Point2d getCentroid(const std::vector<cv::Point2d> &src);

        template <typename T>
        T squaredNorm(const cv::Point_<T> &pt)
        {
            return pt.x * pt.x + pt.y * pt.y;
        }



        /**
         * @brief alignUnitVectorsByRotation returns 3D rotation matrix, that map 3D unit vector
         * fromVec to 3D unit vector toVec
         */
        cv::Mat alignUnitVectorsByRotation(const cv::Vec3d &fromVec,
                                           const cv::Vec3d &toVec);

        /**
         * calculates euler angles from rotation matrix (in doubles). It assumes that the
         * rotation matrix was constructed by applying rotaitons in following order:
         *  around x axis
         *  around y axis
         *  around z axis
         *
         * Return vector of euler angles, where:
         *  cv::Vec3d.(0) is angle around x axis
         *  cv::Vec3d.(1) is angle around y axis
         *  cv::Vec3d.(2) is angle around z axis
         */
        cv::Vec3d calculateEulerAnglesFromRotation(const cv::Mat &rotationMatrix);

        cv::Mat buildRotationMatrixFromEulerAngles(const cv::Vec3d &eulerAngles);

        /**
         * @brief calculateVectorOpositeRotation
         * @param vec must be non-zero vector
         * @return
         */
        cv::Mat calculateVectorOpositeRotation(const cv::Vec3d &vec);



        template <typename ScalarType1,
                  typename ScalarType2>
        double getDistanceFromImageEdge(const cv::Point_<ScalarType1> &pt,
                                        const cv::Size_<ScalarType2> &size)
        {
            return std::min(std::abs(pt.x),
                            std::min(std::abs(pt.x - size.width),
                                     std::min(std::abs(pt.y),
                                              std::abs(pt.y - size.height))));
        }




        // r1, r2 and r3 are columns of rotation matrix
        cv::Mat composeRotationMatrix(const cv::Vec3d &r1, const cv::Vec3d &r2, const cv::Vec3d &r3);


        void invertTranslationAndRotation(const cv::Point3d &t,
                                          const cv::Mat &r,
                                          cv::Point3d &tInv,
                                          cv::Mat &rInv);

        cv::Point3d transformPoint(const cv::Point3d &p,
                                   const cv::Point3d &t,
                                   const cv::Mat &rot);


        cv::Vec3d transformVector(const cv::Vec3d &v,
                                   const cv::Vec3d &t,
                                   const cv::Mat &rot);

        QPainterPath calculateIntersectionProjective(const QPainterPath &thisViewPath,
                                               const QPainterPath &otherViewPath,
                                               const QTransform &otherTransform);


        QPainterPath calculateIntersectionProjective(const QPainterPath &thisViewPath,
                                               const QPainterPath &otherViewPath,
                                               const cv::Mat &otherTransform);

        cv::Mat qTransform2CV(const QTransform &trans);

        // horizon line is: (ax + by + c = 0)
        bool getHorizonFromTransformation(const cv::Mat &transform,
                                          double &a,
                                          double &b,
                                          double &c);

        bool cutRectangleByLine(const QRectF &rectangle,
                                double a,
                                double b,
                                double c,
                                QPolygonF &part1,
                                QPolygonF &part2);
        double getLineY(double x,
                        double a,
                        double b,
                        double c);
        double getLineX(double y,
                        double a,
                        double b,
                        double c);

        double getDistanceFromLine(double a, double b, double c, // line ax + by +c = 0
                                   double x0, double y0); // point (x0,y0)

        QPointF getPolygonApproxCentre(const QPolygonF &polygon);

        // returns just first section of the polyline that is inside given polygon
        QPolygonF simplePolylineIntersection(const QPolygonF &polyline,
                                             const QPolygonF &polygon);

        QPolygonF simplePolygonIntersection(const QPolygonF &polygon,
                                            const QPainterPath &path);

        double calculatePolygonArea(const QPolygonF &polygon);



        // Nadir point is directly under the camera (ergo, X,Y coordinates of camera position)
        cv::Point2d calculateNadirPoint(const cv::Mat &fromWorldToNormImgTransform, // from map to normalised frame coordinates = ergo, K^-1H, or  [R^T_1 R^T_2 (-R^T t)]
                                        const cv::Mat &fromNormImgToWorldTransform = cv::Mat()); // just optional parameter. It will be computed if not provided.




        cv::Point2d transformPoint(const cv::Point2d &p,
                                   const cv::Point2d &t,
                                   const cv::Mat &rot);

        cv::Vec2d transformVector(const cv::Vec2d &v,
                                   const cv::Vec2d &t,
                                   const cv::Mat &rot);

        cv::Mat get2DRotationMatrix(double angle);

        cv::Point2d rotatePointAround(const cv::Point2d &pt,
                                      const cv::Mat &rotation,
                                      const cv::Point2d &pivot);

        cv::Mat createTranslationTransform(double x,
                                         double y);
        cv::Mat createRotationTransform(double angle,
                                        double xPivot,
                                        double yPivot);
        cv::Mat createRotationTransform(double angle);

        cv::Mat createScaleTransform(double xScale,
                                     double yScale);

        // only fist 4 points in poly are considered
        QPointF calculatePrespectiveRectangleCenter(const QPolygonF &poly);


        class RGeometry
        {
        public:
            RGeometry();
        };

    } // namespace geometry
} // namespace rce

#endif // RCE_GEOMETRY_RGEOMETRY_H
