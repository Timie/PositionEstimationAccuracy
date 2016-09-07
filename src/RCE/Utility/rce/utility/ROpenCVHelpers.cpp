#include "ROpenCVHelpers.h"


#include "rce/geometry/RGeometry.h"

#include <opencv2/imgproc/imgproc.hpp>


namespace rce {
    namespace utility {

        ROpenCVHelpers::ROpenCVHelpers()
        {
        }



        bool rectanglesAreOverlapping(const cv::Rect &r1,
                                      const cv::Rect &r2)
        {
            return r1.contains(rce::geometry::rectangleCentre(r2)) && r2.contains(rce::geometry::rectangleCentre(r1)); // centres of both rectangles are included each one in other
        }

        double getRectangleDifference(const cv::Rect &r1,
                                      const cv::Rect &r2)
        {

            cv::Rect intersection = r1 & r2;

            //      this part is 0, when the rectangles are perfectly overlapping
            return ((r1.area() - intersection.area()) + (r2.area() - intersection.area())) / static_cast<double>(r1.area() + r2.area());
        }


        void getListOfNonZeroPoints(const cv::Mat &input,
                                    std::vector<cv::Point2d> &output)
        {
            output.clear();
            output.reserve(input.size().width * input.size().height / 2);
            for(int x = 0; x < input.size().width; ++x)
            {
                for(int y = 0; y < input.size().height; ++y)
                {
                    if(input.at<uchar>(cv::Point(x,y)) != 0)
                    {
                        output.push_back(cv::Point2d(x,y));
                    }
                }
            }

            output.shrink_to_fit();
        }

        void
        drawPolygonOnCvMat(const QPolygonF &polygon,
                           cv::Mat &image,
                           const cv::Scalar &value,
                           const cv::Mat &transform)
        {
            if(polygon.size() > 2)
            {
                std::vector<cv::Point> shapePoints;
                shapePoints.reserve(polygon.size());

                if(transform.empty())
                {
                    for(int i = 0; i < polygon.size(); ++i)
                    {

                        shapePoints.push_back(cv::Point(qRound(polygon[i].x()),
                                                        qRound(polygon[i].y())));
                    }
                }
                else
                {
                    for(int i = 0; i < polygon.size(); ++i)
                    {
                        cv::Vec3d pos(polygon[i].x(),
                                      polygon[i].y(),
                                      1.0);

                        pos = cv::Mat(transform * cv::Mat(pos));

                        shapePoints.push_back(cv::Point(qRound(pos(0) / pos(2)),
                                                        qRound(pos(1) / pos(2))));
                    }
                }

                const cv::Point *pts = shapePoints.data();
                int npts = shapePoints.size();

                cv::fillPoly(image,
                             &pts,
                             &npts,
                             1,
                             value,
                             8,
                             0,
                             cv::Point());
            }
        }

        double
        absDiffAvg(const cv::Mat &mat1,
                const cv::Mat &mat2,
                const cv::Mat &mask)
        {
            double validCnts = 0;
            double absDiffSum = 0;


            int height = mat1.rows;
            int width = mat1.cols;


            if(mat1.type() != mat2.type())
            {
                throw "ROpenCVHelpers::absDiffAvg: Different matrix types...";
            }

            if(mat1.size() != mat2.size())
            {
                throw "ROpenCVHelpers::absDiffAvg: Different matrix sizes...";
            }

            if(mat1.type() == CV_8UC1)
            {


                for (int y = 0; y < height; ++y)
                {
                    const uchar* mat1ptr = mat1.ptr<uchar>(y);
                    const uchar* mat2ptr = mat2.ptr<uchar>(y);
                    const uchar* maskptr = mask.ptr<uchar>(y);

                    for (int x = 0; x < width; ++x)
                    {
                        if(maskptr[x] != 0)
                        {
                            validCnts++;
                            absDiffSum += std::abs(static_cast<int>(mat1ptr[x]) - static_cast<int>(mat2ptr[x]));
                        }
                    }
                }

            }
            else if(mat1.type() == CV_8UC3)
            {


                for (int y = 0; y < height; ++y)
                {
                    const cv::Point3_<uchar>* mat1ptr = mat1.ptr<cv::Point3_<uchar>>(y);
                    const cv::Point3_<uchar>* mat2ptr = mat2.ptr<cv::Point3_<uchar>>(y);
                    const uchar* maskptr = mask.ptr<uchar>(y);


                    for (int x = 0; x < width; ++x)
                    {
                        if(maskptr[x] != 0)
                        {
                            validCnts++;
                            absDiffSum += std::abs(static_cast<int>(mat1ptr[x].x) - static_cast<int>(mat2ptr[x].x));
                            absDiffSum += std::abs(static_cast<int>(mat1ptr[x].y) - static_cast<int>(mat2ptr[x].y));
                            absDiffSum += std::abs(static_cast<int>(mat1ptr[x].z) - static_cast<int>(mat2ptr[x].z));
                        }
                    }
                }

            }
            else if(mat1.type() == CV_32FC1)
            {




                for (int y = 0; y < height; ++y)
                {
                    const float* mat1ptr = mat1.ptr<float>(y);
                    const float* mat2ptr = mat2.ptr<float>(y);
                    const uchar* maskptr = mask.ptr<uchar>(y);

                    for (int x = 0; x < width; ++x)
                    {
                        if(maskptr[x] != 0)
                        {
                            validCnts++;
                            absDiffSum += std::abs(mat1ptr[x] - mat2ptr[x]);
                        }
                    }
                }

            }
            else if(mat1.type() == CV_32FC3)
            {



                for (int y = 0; y < height; ++y)
                {
                    const cv::Point3_<float>* mat1ptr = mat1.ptr<cv::Point3_<float>>(y);
                    const cv::Point3_<float>* mat2ptr = mat2.ptr<cv::Point3_<float>>(y);
                    const uchar* maskptr = mask.ptr<uchar>(y);

                    for (int x = 0; x < width; ++x)
                    {
                        if(maskptr[x] != 0)
                        {
                            validCnts++;
                            absDiffSum += std::abs(mat1ptr[x].x - mat2ptr[x].x);
                            absDiffSum += std::abs(mat1ptr[x].y - mat2ptr[x].y);
                            absDiffSum += std::abs(mat1ptr[x].z - mat2ptr[x].z);
                        }
                    }
                }

            }
            else
            {
                throw "ROpenCVHelpers::absDiffAvg: Not implemented for given matrix type.";
            }



            if(validCnts > 0)
            {
                return absDiffSum / validCnts;
            }
            else
            {
                return -1;
            }
        }

        int
        countNonZeroElems(const cv::Mat &mask)
        {
            int count = 0;
            if(mask.type() == CV_8UC1)
            {
                for(int row = 0; row < mask.rows; ++row)
                {
                    const uchar* p = mask.ptr<uchar>(row);
                    for(int col = 0; col < mask.cols; ++col)
                    {
                        if(p[col] != 0)
                        {
                            ++count;
                        }

                    }
                }
            }
            else
            {
                throw "ROpenCVHelpers::countNonZeroElems: Not implemented for this matrix type.";
            }

            return count;

        }

        bool
        isMatEqual(const cv::Mat &mat1,
                   const cv::Mat &mat2)
        {
            // taken from: http://stackoverflow.com/questions/9905093/how-to-check-whether-two-matrixes-are-identical-in-opencv

            // treat two empty mat as identical as well
            if (mat1.empty() && mat2.empty()) {
                return true;
            }
            // if dimensionality of two mat is not identical, these two mat is not identical
            if (mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims) {
                return false;
            }
            cv::Mat diff;
            cv::compare(mat1, mat2, diff, cv::CMP_NE);
            int nz = cv::countNonZero(diff);
            return nz==0;
        }


        bool isPointInsideImage(const cv::Point2i &pt,
                                const cv::Mat &image)
        {
            return ((pt.x > 0) &&
                    (pt.x < image.cols) &&
                    (pt.y > 0) &&
                    (pt.y < image.rows));
        }
        void
        copyImageOverOther(const cv::Mat &srcImage,
                           const cv::Point2i &offset,
                           cv::Mat &dstImage)
        {
            if((offset.x >= dstImage.cols) ||
               (offset.y >= dstImage.rows))
            {
                return;
            }
            else
            {
                cv::Rect dstRoi = (cv::Rect(offset,
                                        srcImage.size()) &
                                cv::Rect(cv::Point2i(0,0),
                                         dstImage.size()));
                cv::Rect srcRoi(dstRoi.tl() - offset,
                                dstRoi.size());
                cv::Mat srcSelect(srcImage, srcRoi);
                srcSelect.copyTo(cv::Mat(dstImage, dstRoi));
            }
        }

        bool
        isMatEqualEmptyIdentity(const cv::Mat &mat1,
                          const cv::Mat &mat2)
        {
            return ((mat1.empty() && mat2.empty()) ||
                    (mat1.empty() && isMatEqual(mat2, cv::Mat::eye(mat2.size(), mat2.type()))) ||
                    (mat2.empty() && isMatEqual(mat1, cv::Mat::eye(mat1.size(), mat1.type()))) ||
                    isMatEqual(mat1, mat2));
        }

        bool
        prepareOverlappingParts(const cv::Mat &img1,
                                const cv::Point2i &img1Offset,
                                const cv::Mat &img2,
                                cv::Mat &img1Overlap,
                                cv::Mat &img2Overlap,
                                cv::Point2i &img1OverOffset)
        {
            if((img1Offset.x >= img2.cols) ||
               (img1Offset.y >= img2.rows))
            {
                return false;
            }

            cv::Rect roi2 = (cv::Rect(img1Offset,
                                        img1.size()) &
                               cv::Rect(cv::Point2i(0,0),
                                        img2.size()));
            if((roi2.width <= 0) ||
               (roi2.height <= 0))
            {
                return false;
            }


            cv::Rect roi1(roi2.tl() - img1Offset,
                            roi2.size());

            img1Overlap = img1(roi1);
            img2Overlap = img2(roi2);

            img1OverOffset = roi2.tl();

            return true;
        }

    } // namespace utility
} // namespace rce
