#include "RCSVOutput.h"

#include <QString>

QString
rce::accuracy::
vectorToCSV(const std::vector<double> &vec)
{
    QString results;
    for(size_t i = 0;
        i < vec.size();
        ++i)
    {
        results.push_back(QString::number(vec[i],'f', 13)+";");
    }
    return results;
}

QString
rce::accuracy::
vectorToCSV(const std::vector<cv::Point3d> &vec,
            int idx)
{
    if(idx < 0)
    {
        idx = 0;
    }

    QString results;
    for(size_t i = 0;
        i < vec.size();
        ++i)
    {
        if(idx == 0)
        {
            results.push_back(QString::number(vec[i].x,'f', 13)+";");
        }
        else if(idx == 1)
        {
            results.push_back(QString::number(vec[i].y,'f', 13)+";");
        }
        else // if(idx == 2)
        {
            results.push_back(QString::number(vec[i].z,'f', 13)+";");
        }
    }

    return results;
}
