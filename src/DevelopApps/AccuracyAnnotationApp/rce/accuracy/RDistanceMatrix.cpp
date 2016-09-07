#include "RDistanceMatrix.h"


#include "rce/utility/RXmlConversion.h"
#include "rce/geometry/RGeometry.h"
#include "dfs/core/DDebug.h"
#include "rce/utility/ROpenCVtoQDebug.h"


rce::accuracy::RDistanceMatrix::
RDistanceMatrix()
{

}

rce::accuracy::RDistanceMatrix::
RDistanceMatrix(int numOfPlaces):
    distances_(cv::Mat::zeros(numOfPlaces,
                       numOfPlaces,
                       CV_64FC1)),
    setFlags_(cv::Mat::eye(numOfPlaces,
                             numOfPlaces,
                             CV_8UC1))
{

}

void
rce::accuracy::RDistanceMatrix::
readFromQDomElement(const QDomElement &element)
{
    QDomElement distMatElement = element.firstChildElement("dist_mat");
    rce::utility::RXmlConversion::readFromQDomElement(distMatElement,
                                                      distances_);

    QDomElement flagMatElement = element.firstChildElement("flag_mat");
    rce::utility::RXmlConversion::readFromQDomElement(flagMatElement,
                                                      setFlags_);
}

void
rce::accuracy::RDistanceMatrix::
writeToQDomElement(QDomElement &element) const
{
    QDomElement distMatElement = element.ownerDocument().createElement("dist_mat");
    rce::utility::RXmlConversion::writeToQDomElement(distMatElement,
                                                     distances_);
    element.appendChild(distMatElement);

    QDomElement flagMatElement = element.ownerDocument().createElement("flag_mat");
    rce::utility::RXmlConversion::writeToQDomElement(flagMatElement,
                                                     setFlags_);
    element.appendChild(flagMatElement);
}

void
rce::accuracy::RDistanceMatrix::
setDistance(int placeA, int placeB, double distance)
{
    distances_.at<double>(placeA, placeB) = distance;
    distances_.at<double>(placeB, placeA) = distance;
    setFlags_.at<uchar>(placeA, placeB) = 255;
    setFlags_.at<uchar>(placeB, placeA) = 255;
}

void
rce::accuracy::RDistanceMatrix::
resize(int numOfPlaces)
{
    if(numOfPlaces < getNumOfPlaces())
    {
        if(numOfPlaces > 0)
        {
            distances_ = distances_(cv::Rect(0,0,
                                             numOfPlaces,
                                             numOfPlaces));
            setFlags_ = setFlags_(cv::Rect(0,0,
                                           numOfPlaces,
                                           numOfPlaces));
        }
        else
        {
            distances_ = cv::Mat();
            setFlags_ = cv::Mat();
        }

    }
    else if(numOfPlaces > getNumOfPlaces())
    {
        cv::Mat tmp1 = distances_;
        distances_ = cv::Mat::zeros(numOfPlaces,numOfPlaces,CV_64FC1);


        cv::Mat tmp2 = setFlags_;
        setFlags_ = cv::Mat::eye(numOfPlaces,numOfPlaces,CV_8UC1);

        if(!tmp2.empty())
        {

            tmp1.copyTo(distances_(cv::Rect(0,0,tmp1.cols, tmp1.rows)));
            tmp2.copyTo(setFlags_(cv::Rect(0,0,tmp2.cols, tmp2.rows)));
        }
    }
}

std::vector<cv::Point2d>
rce::accuracy::RDistanceMatrix::
generatePositions(const cv::Point2d &place1Pos,
                  double headingToPlace2)
{
    if(allDistancesAreKnown())
    {
        // based upon: http://math.stackexchange.com/questions/156161/finding-the-coordinates-of-points-from-distance-matrix
        cv::Mat m = cv::Mat(distances_.size(),
                            CV_64FC1);
        for(int i = 0;
            i < m.rows;
            ++i)
        {
            for(int j = 0;
                j < m.cols;
                ++j)
            {
                double d1J = distances_.at<double>(0,j);
                double dI1 = distances_.at<double>(i,0);
                double dIJ = distances_.at<double>(i,j);
                m.at<double>(i,j) = (d1J * d1J + dI1 * dI1 + dIJ * dIJ) / 2.0;

            }
        }
        cv::Mat u, s, vt;
        cv::SVDecomp(m, s,u,vt);
        cv::Mat sRoot;
        cv::sqrt(s, sRoot);
        cv::Mat x = u * sRoot;

        dDebug() << "xMatrix" << x << u << s << vt;

        std::vector<cv::Point2d> positions = x;

        if(positions.size() > 0)
        {
            //  move and rotate
            cv::Point2d zeroPoint = positions[0];
            cv::Point2d shiftToDest = place1Pos - zeroPoint;

            cv::Point2d secondPoint = positions[1];

            double currentHeading = std::atan2(secondPoint.y - zeroPoint.y,
                                               secondPoint.x - zeroPoint.x);
            double headingFix = headingToPlace2 - currentHeading;
            cv::Mat rotation = rce::geometry::get2DRotationMatrix(headingFix);

            for(int i = 0;
                i < positions.size();
                ++i)
            {
                positions[i] = rce::geometry::rotatePointAround(positions[i],
                                                                rotation,
                                                                zeroPoint) + shiftToDest;
            }

            return positions;
        }
        else
        {
            return std::vector<cv::Point2d>();
        }

    }
    else
    {
        return std::vector<cv::Point2d>();
    }
}

std::vector<cv::Point2d>
rce::accuracy::RDistanceMatrix::
generatePositions3Points(const cv::Point2d &place1Pos,
                  double headingToPlace2,
                  bool flippedPlace3)
{
    if(getNumOfPlaces() == 0)
    {
        return std::vector<cv::Point2d>();
    }
    std::vector<cv::Point2d> solutionVector(getNumOfPlaces());
    std::vector<uchar> calculatedFlags(getNumOfPlaces(),0);

    solutionVector[0] = place1Pos;
    calculatedFlags[0] = 255;

    {
        int secondPointIdx = -1;
        cv::Point2d secondPoint;
        // now calculate the next point
        for(int i = 1; i < solutionVector.size(); ++i)
        {
            if(setFlags_.at<uchar>(i, 0) != 0)
            {
                double distance = distances_.at<double>(i,0);
                double xDiff = std::cos(headingToPlace2) * distance;
                double yDiff = std::sin(headingToPlace2) * distance;

                secondPoint = cv::Point2d(place1Pos.x + xDiff,
                                                                place1Pos.y + yDiff);

                secondPointIdx = i;
                solutionVector[i] = secondPoint;
                calculatedFlags[i] = 255;
                break;
            }
        }
        if(secondPointIdx < 0)
        {
            return std::vector<cv::Point2d> ();
        }

        int thirdPointIdx =  -1;
        cv::Point2d thrirdPoint;
        // now find the third point
        for(int i = 1; i < solutionVector.size(); ++i)
        {
            if((i != secondPointIdx) &&
               (setFlags_.at<uchar>(i, 0) != 0) &&
               (setFlags_.at<uchar>(i, secondPointIdx) != 0))
            {
                double dist12 = distances_.at<double>(0,secondPointIdx);
                double dist13 = distances_.at<double>(0, i);
                double dist23 = distances_.at<double>(i, secondPointIdx);

                // we know distances, time to calculate the angle
                double a1 = (dist12 * dist12 +
                        dist13 * dist13 -
                        dist23 * dist23);
                double a2 = (2*dist13*dist12);
                double angle = std::acos(a1 / a2);
                if(flippedPlace3)
                {
                    angle = angle + headingToPlace2;
                }
                else
                {
                    angle = headingToPlace2 - angle;
                }

                double xDiff = std::cos(angle) * dist13;
                double yDiff = std::sin(angle) * dist13;


                thrirdPoint = cv::Point2d(place1Pos.x + xDiff,
                                          place1Pos.y + yDiff);

                thirdPointIdx = i;
                solutionVector[i] = thrirdPoint;
                calculatedFlags[i] = 255;
                break;

            }
        }


        if(thirdPointIdx < 0)
        {
            return std::vector<cv::Point2d> ();
        }
    }


    while(!isSolutionFound(calculatedFlags))
    {
        bool solutionChanged = false;
        // change first unknown point that is tied to 3 other known points for which their position is known.
        for(int i = 1;
            i < solutionVector.size();
            ++i)
        {
            if(calculatedFlags[i] == 0)
            {
                // we are at the point i, now chceck, whether it is connected to some three other points that are calculated
                int ptIdx1 = -1;
                int ptIdx2 = -1;
                int ptIdx3 = -1;
                for(int j = 0;
                    j < solutionVector.size();
                    ++j)
                {
                    if((j != i) &&
                       (setFlags_.at<uchar>(i,j) != 0))
                    {
                        if(ptIdx1 < 0)
                        {
                            ptIdx1 = j;
                        }
                        else if(ptIdx2 < 0)
                        {
                            ptIdx2 = j;
                        }
                        else
                        {
                            ptIdx3 = j;
                            break;
                        }
                    }
                }

                if((ptIdx1 >= 0) && (ptIdx2 >= 0) && (ptIdx3 >= 0))
                {
                    // yup, it is connected - we can calculate its position
                    double distI1 = distances_.at<double>(i,ptIdx1);
                    double distI2 = distances_.at<double>(i,ptIdx2);
                    double distI3 = distances_.at<double>(i,ptIdx3);

                    cv::Point2d vector12 = solutionVector[ptIdx2] - solutionVector[ptIdx1];
                    double dist12 = cv::norm(vector12);


                    double a1 = (distI1 * distI1 -
                                 distI2 * distI2 +
                                 dist12 * dist12);
                    double a2 = (2*distI1*dist12);
                    double angle213 = std::acos(a1 / a2);
                    double heading12 = std::atan2(vector12.y,
                                                  vector12.x);

                    double angleA = heading12 + angle213;
                    double angleB = heading12 - angle213;


                    double xDiffA = std::cos(angleA) * distI1;
                    double yDiffA = std::sin(angleA) * distI1;
                    cv::Point2d solutionA = solutionVector[ptIdx1] + cv::Point2d(xDiffA,
                                                                                 yDiffA);


                    double xDiffB = std::cos(angleB) * distI1;
                    double yDiffB = std::sin(angleB) * distI1;
                    cv::Point2d solutionB = solutionVector[ptIdx1] + cv::Point2d(xDiffB,
                                                                                 yDiffB);

                    double distanceA = std::abs(cv::norm(solutionA - solutionVector[ptIdx3]) - distI3);
                    double distanceB = std::abs(cv::norm(solutionB - solutionVector[ptIdx3]) - distI3);
                    if(distanceA <= distanceB)
                    {
                        solutionVector[i] = solutionA;
                    }
                    else
                    {
                        solutionVector[i] = solutionB;
                    }

                    calculatedFlags[i] = 255;
                    solutionChanged = true;
                }
            }
        }

        if(!solutionChanged)
        {
            if(isSolutionFound(calculatedFlags))
            {
                // FIXME: just to make things in centimeters
                for(int i = 0;
                    i < solutionVector.size();
                    ++i)
                {
                    solutionVector[i] = solutionVector[i] * 1000;
                }

                return solutionVector;
            }
            else
            {
                return std::vector<cv::Point2d> ();
            }
        }
    }

    // FIXME: just to make things in centimeters
    for(int i = 0;
        i < solutionVector.size();
        ++i)
    {
        solutionVector[i] = solutionVector[i] * 1000;
    }

    return solutionVector;
}

bool
rce::accuracy::RDistanceMatrix::
allDistancesAreKnown() const
{
    for(int x = 0;
        x < setFlags_.cols;
        ++x)
    {
        for(int y = 0;
            y < setFlags_.rows;
            ++y)
        {
            if(setFlags_.at<uchar>(y,x) == 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool
rce::accuracy::RDistanceMatrix::
isDistanceKnown(int place1, int place2) const
{
    return (setFlags_.at<uchar>(place1,place2) != 0);
}

double
rce::accuracy::RDistanceMatrix::
getDistance(int place1,
            int place2) const
{
    return distances_.at<double>(place1,
                                 place2);
}

void
rce::accuracy::RDistanceMatrix::
clearValue(int place1,
           int place2)
{
    distances_.at<double>(place1,place2) = 0;
    distances_.at<double>(place2,place1) = 0;
    setFlags_.at<uchar>(place1,place2)  = 0;
    setFlags_.at<uchar>(place2,place1)  = 0;

}

bool
rce::accuracy::RDistanceMatrix::
isSolutionFound(const std::vector<uchar> &calculatedFlags)
{
    for(int i = 0;
        i < calculatedFlags.size();
        ++i)
    {
        if(calculatedFlags[i] == 0)
        {
            return false;
        }
    }

    return true;
}
