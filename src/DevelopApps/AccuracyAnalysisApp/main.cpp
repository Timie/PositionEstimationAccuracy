/**
  * This app is for analysis of accuracy across different camera heights and viewing angles.
  */


#include <iostream>

#include <QSettings>
#include <QCoreApplication>

#include <omp.h>

#include "rce/accuracy/RVisionExperiment.h"
#include "rce/accuracy/RCSVOutput.h"
#include "dfs/core/DDebug.h"

int
main(int argc,
     char *argv[])
{
    try
    {
        QCoreApplication app(argc, argv);
        QSettings settings("D:/ADAM/Work/Rce/git/develop/DevelopApps/AccuracyAnalysisApp/configs/experiment_config_parallel.ini",
                           QSettings::IniFormat);

        double startAngle = 0;
        double endAngle = 81;
        double stepAngle = 5;

        double startHeight = 50;
        double endHeight = 401;
        double stepHeight = 25;

        std::vector<double> angles;
        for(double angle = startAngle;
            angle < endAngle;
            angle += stepAngle)
        {
            angles.push_back(angle);
        }

        std::vector<double> distances;
        for(double distance = startHeight;
            distance < endHeight;
            distance += stepHeight)
        {
            distances.push_back(distance);
        }

        std::vector<std::vector<cv::Point3d>> avgErrors(angles.size(),
                                                        std::vector<cv::Point3d>(distances.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas02(angles.size(),
                                                                std::vector<cv::Point3d>(distances.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas03(angles.size(),
                                                                std::vector<cv::Point3d>(distances.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas04(angles.size(),
                                                                std::vector<cv::Point3d>(distances.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas05(angles.size(),
                                                                std::vector<cv::Point3d>(distances.size()));

        for(size_t i = 0;
            i < angles.size();
            ++i)
        {
            double angle = angles[i];
            double angleRad = angle * 2.0 * M_PI / 360.0;
            double z = cos(angleRad);
            double y = sin(angleRad);


            #pragma omp parallel for
            for(int j = 0;
                j < distances.size();
                ++j)
            {
                double distance = distances[j];
                rce::accuracy::RVisionExperiment e;

                #pragma omp critical
                {
                    settings.setValue("scene/camera/position_y",
                                      y * distance);
                    settings.setValue("scene/camera/position_z",
                                      z * distance);
                    settings.sync();
                    e.loadSettings(settings);
                }


                if(e.run())
                {

                    avgErrors[i][j] = cv::Point3d(angle,
                                                  distance,
                                                  e.getAvgError());
                    coveredAreas02[i][j] = cv::Point3d(angle,
                                                  distance,
                                                  e.getTargetsUnder02());
                    coveredAreas03[i][j] = cv::Point3d(angle,
                                                  distance,
                                                  e.getTargetsUnder03());
                    coveredAreas04[i][j] = cv::Point3d(angle,
                                                  distance,
                                                  e.getTargetsUnder04());
                    coveredAreas05[i][j] = cv::Point3d(angle,
                                                  distance,
                                                  e.getTargetsUnder05());
                }
                else
                {

                    avgErrors[i][j] = cv::Point3d(angle,
                                                 distance,
                                                 0);
                    coveredAreas02[i][j] = cv::Point3d(angle,
                                                 distance,
                                                 0);
                    coveredAreas03[i][j] = cv::Point3d(angle,
                                                 distance,
                                                 0);
                    coveredAreas04[i][j] = cv::Point3d(angle,
                                                 distance,
                                                 0);

                    coveredAreas05[i][j] = cv::Point3d(angle,
                                                  distance,
                                                  0);
                }

                std::cerr << "Finished calculation of experiment configuration " << angle << " " << distance << " with error " << avgErrors[i][j].z << std::endl;
                std::cout << "Finished calculation of experiment configuration " << angle << " " << distance << " " << y << " " << z <<  " with error " << avgErrors[i][j].z << std::endl;
                //dDebug() << "Finished calculation of experiment configuration " << angle << distance << y << z << " with error " << e.getAvgError();
                //qDebug() << "Finished calculation of experiment configuration " << angle << distance << y << z << " with error " << e.getAvgError();

            }
        }


        dWarning() << "Angles:" << rce::accuracy::vectorToCSV(angles);
        dWarning() << "Distances:" << rce::accuracy::vectorToCSV(distances);

        dWarning() << "Error matrix:";
        for(size_t i = 0;
            i < angles.size();
            ++i)
        {
            dWarning() << rce::accuracy::vectorToCSV(avgErrors[i],2);
        }


        dWarning() << "Cov02 matrix:";
        for(size_t i = 0;
            i < angles.size();
            ++i)
        {
            dWarning() << rce::accuracy::vectorToCSV(coveredAreas02[i],2);
        }



        dWarning() << "Cov03 matrix:";
        for(size_t i = 0;
            i < angles.size();
            ++i)
        {
            dWarning() << rce::accuracy::vectorToCSV(coveredAreas03[i],2);
        }


        dWarning() << "Cov04 matrix:";
        for(size_t i = 0;
            i < angles.size();
            ++i)
        {
            dWarning() << rce::accuracy::vectorToCSV(coveredAreas04[i],2);
        }


        dWarning() << "Cov05 matrix:";
        for(size_t i = 0;
            i < angles.size();
            ++i)
        {
            dWarning() << rce::accuracy::vectorToCSV(coveredAreas05[i],2);
        }







        return 0;
    }
    catch(const char *e)
    {
        dCritical() << "Exception:" << e;
        return -1;
    }
    catch(const std::exception &e)
    {
        dCritical() << "Exception:" << e.what();
        return -2;
    }
    catch(...)
    {
        dCritical() << "Unknown Exception";
        throw;
    }
}
