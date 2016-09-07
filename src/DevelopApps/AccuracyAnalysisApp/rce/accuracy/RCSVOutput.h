#ifndef RCE_ACCURACY_RCSVOUTPUT_H
#define RCE_ACCURACY_RCSVOUTPUT_H

#include <vector>
#include <opencv2/core/core.hpp>

class QString;



namespace rce {
    namespace accuracy {

        QString vectorToCSV(const std::vector<double> &vec);
        QString vectorToCSV(const std::vector<cv::Point3d> &vec,
                            int idx = -1);


//        class RCSVOutput
//        {
//        public:
//            RCSVOutput();
//        };

    } // namespace accuracy
} // namespace rce

#endif // RCE_ACCURACY_RCSVOUTPUT_H
