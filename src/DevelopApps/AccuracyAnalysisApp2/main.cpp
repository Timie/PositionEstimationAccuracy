/**
  * This app is for analysis of accuracy across error magnitudes and view angles.
  */

#include <iostream>

#include <QSettings>
#include <QCoreApplication>

#include <omp.h>

#include "rce/accuracy/RVisionExperiment.h"
#include "rce/accuracy/RCSVOutput.h"
#include "dfs/core/DDebug.h"
#include "dfs/core/DFileDebug.h"


enum RVariableType
{
    RCameraDistance = 1,
    RCameraAngle = 2,
    RLandmarkImageError = 3,
    RLandmarkWorldError = 4,
    RLandmarkElevationError = 5,
    RTargetImageError = 6,
    RTargetElevationError = 7
};


void setVariable(RVariableType type,
                 double value,
                 QSettings &settings)
{
    switch(type)
    {
        case RCameraDistance:
            {
                // preserve orientation
                cv::Point3d formerPos(settings.value("scene/camera/position_x").toDouble(),
                                      settings.value("scene/camera/position_y").toDouble(),
                                      settings.value("scene/camera/position_z").toDouble());
                double distance = cv::norm(formerPos);
                cv::Point3d newPos = formerPos * (value / distance);
                settings.setValue("scene/camera/position_x",
                                  newPos.x);
                settings.setValue("scene/camera/position_y",
                                  newPos.y);
                settings.setValue("scene/camera/position_z",
                                  newPos.z);
            }
            break;
        case RCameraAngle:
            {
                // preserve distance
                cv::Point3d formerPos(settings.value("scene/camera/position_x").toDouble(),
                                      settings.value("scene/camera/position_y").toDouble(),
                                      settings.value("scene/camera/position_z").toDouble());
                double distance = cv::norm(formerPos);

                double angleRad = value * 2.0 * M_PI / 360.0;
                double z = cos(angleRad);
                double y = sin(angleRad);

                settings.setValue("scene/camera/position_x",
                                  0);
                settings.setValue("scene/camera/position_y",
                                  y * distance);
                settings.setValue("scene/camera/position_z",
                                  z * distance);

            }
            break;
        case RLandmarkImageError:
            if(value == 0)
            {
                settings.setValue("tie_point_image_error/generator_type",
                                  "constant_error");
                settings.setValue("tie_point_image_error/error",
                                  0);
            }
            else
            {
                settings.setValue("tie_point_image_error/generator_type",
                                  "gaussian_noise");
                settings.setValue("tie_point_image_error/sigma",
                                  value);
            }
            break;
        case RLandmarkWorldError:
            if(value == 0)
            {
                settings.setValue("tie_point_world_error/generator_type",
                                  "constant_error");
                settings.setValue("tie_point_world_error/error",
                                  0);
            }
            else
            {
                settings.setValue("tie_point_world_error/generator_type",
                                  "gaussian_noise");
                settings.setValue("tie_point_world_error/sigma",
                                  value);
            }
            break;
        case RLandmarkElevationError:
            if(value == 0)
            {
                settings.setValue("tie_point_elevation_error/generator_type",
                                  "constant_error");
                settings.setValue("tie_point_elevation_error/error",
                                  0);
            }
            else
            {
                settings.setValue("tie_point_elevation_error/generator_type",
                                  "gaussian_noise");
                settings.setValue("tie_point_elevation_error/sigma",
                                  value);
            }
            break;
        case RTargetImageError:
            if(value == 0)
            {
                settings.setValue("target_image_error/generator_type",
                                  "constant_error");
                settings.setValue("target_image_error/error",
                                  0);
            }
            else
            {
                settings.setValue("target_image_error/generator_type",
                                  "gaussian_noise");
                settings.setValue("target_image_error/sigma",
                                  value);
            }
            break;
        case RTargetElevationError:
            if(value == 0)
            {
                settings.setValue("target_elevation_error/generator_type",
                                  "constant_error");
                settings.setValue("target_elevation_error/error",
                                  0);
            }
            else
            {
                settings.setValue("target_elevation_error/generator_type",
                                  "gaussian_noise");
                settings.setValue("target_elevation_error/sigma",
                                  value);
            }
            break;
        default:
            {
                dCritical() << "Unsupported variable type" << static_cast<int>(type);
                throw "Unsupported variable type";
            }
    }
}

int
main(int argc,
     char *argv[])
{
    try
    {
        // arguments:
        // 0 = command
        // 1 = path to settings
        // 2 = primary variable
        // 3 = primary variable start
        // 4 = primary variable end
        // 5 = primary variable step
        // 6 = secondary variable
        // 7 = secondary variable start
        // 8 = secondary variable end
        // 9 = secondary variable step
        if(argc != 10)
        {
            dCritical() << "Wrong number of parameters";
            return -4;
        }

        QString settingsPath = "D:/ADAM/Work/Rce/git/develop/DevelopApps/AccuracyAnalysisApp/configs/experiment_config_parallel.ini";
        if(argc > 1)
        {
            settingsPath = argv[1];
        }
        QCoreApplication app(argc, argv);
        QSettings settings(settingsPath,
                           QSettings::IniFormat);
        settings.sync();
        if(settings.status() != QSettings::NoError)
        {
            dCritical() << "Could not load settings from path" << settingsPath;
            return -5;
        }
        else
        {
            dfs::core::DDebugInitialiser::init();
            qDebug() << "Starting analysis...";
        }

        RVariableType primaryVariableType = static_cast<RVariableType>(QString(argv[2]).toInt());
        double primaryVariableStart = QString(argv[3]).toDouble();
        double primaryVariableEnd = QString(argv[4]).toDouble();
        double primaryVariableStep = QString(argv[5]).toDouble();


        RVariableType secondaryVariableType = static_cast<RVariableType>(QString(argv[6]).toInt());
        double secondaryVariableStart = QString(argv[7]).toDouble();
        double secondaryVariableEnd = QString(argv[8]).toDouble();
        double secondaryVariableStep = QString(argv[9]).toDouble();

        std::vector<double> primaryValues;
        for(double value = primaryVariableStart;
            value < primaryVariableEnd;
            value += primaryVariableStep)
        {
            primaryValues.push_back(value);
        }
        std::vector<double> secondaryValues;
        for(double value = secondaryVariableStart;
            value < secondaryVariableEnd;
            value += secondaryVariableStep)
        {
            secondaryValues.push_back(value);
        }

        std::vector<uchar> targetCaptureFlags;

        if(true) // set this to false if you want to capture all possible targets in every view
        {
        // get list of all visible targets (in all configurations)
            for(size_t i = 0;
                i < primaryValues.size();
                ++i)
            {
                double primaryValue = primaryValues[i];
#pragma omp parallel for
                for(int j = 0;
                    j < secondaryValues.size();
                    ++j)
                {
                    double secondaryValue = secondaryValues[j];
                    rce::accuracy::RVisionExperiment e;
                    e.setIdentificationString(QString("Config(%1,%2)")
                                              .arg(primaryValue)
                                              .arg(secondaryValue));

                    {
#pragma omp critical
                        {
                            setVariable(primaryVariableType,
                                        primaryValue,
                                        settings);
                            setVariable(secondaryVariableType,
                                        secondaryValue,
                                        settings);
                            settings.sync();
                            e.loadSettings(settings);
                        }
                    }

                    e.analyzeNoisefreeScene();


                    {
#pragma omp critical
                        {
                            if(targetCaptureFlags.empty())
                            {
                                targetCaptureFlags = e.getTargetCaptureFlags();
                            }
                            else
                            {
                                assert(targetCaptureFlags.size() == e.getTargetCaptureFlags());
                                for(int i = 0;
                                    i < targetCaptureFlags.size();
                                    ++i)
                                {
                                    targetCaptureFlags[i] = targetCaptureFlags[i] && e.getTargetCaptureFlags()[i];
                                }
                            }
                        }
                    }
                }
            }
        }


        std::vector<std::vector<cv::Point3d>> avgErrors(primaryValues.size(),
                                                        std::vector<cv::Point3d>(secondaryValues.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas02(primaryValues.size(),
                                                             std::vector<cv::Point3d>(secondaryValues.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas03(primaryValues.size(),
                                                             std::vector<cv::Point3d>(secondaryValues.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas04(primaryValues.size(),
                                                             std::vector<cv::Point3d>(secondaryValues.size()));
        std::vector<std::vector<cv::Point3d>> coveredAreas05(primaryValues.size(),
                                                             std::vector<cv::Point3d>(secondaryValues.size()));



        for(size_t i = 0;
            i < primaryValues.size();
            ++i)
        {
            double primaryValue = primaryValues[i];
#pragma omp parallel for
            for(int j = 0;
                j < secondaryValues.size();
                ++j)
            {
                double secondaryValue = secondaryValues[j];
                rce::accuracy::RVisionExperiment e;
                e.setIdentificationString(QString("Config(%1,%2)")
                                          .arg(primaryValue)
                                          .arg(secondaryValue));

                {
#pragma omp critical
                    {
                        setVariable(primaryVariableType,
                                    primaryValue,
                                    settings);
                        setVariable(secondaryVariableType,
                                    secondaryValue,
                                    settings);
                        settings.sync();
                        e.loadSettings(settings);
                    }
                }

                if(e.run(targetCaptureFlags)) // fixme: comment out targetCaptureFlags to disable rejection of targets that were not captured in some configurations
                {

                    avgErrors[i][j] = cv::Point3d(primaryValue,
                                                  secondaryValue,
                                                  e.getAvgError());
                    coveredAreas02[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       e.getTargetsUnder02());
                    coveredAreas03[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       e.getTargetsUnder03());
                    coveredAreas04[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       e.getTargetsUnder04());
                    coveredAreas05[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       e.getTargetsUnder05());
                }
                else
                {

                    avgErrors[i][j] = cv::Point3d(primaryValue,
                                                  secondaryValue,
                                                  0);
                    coveredAreas02[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       0);
                    coveredAreas03[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       0);
                    coveredAreas04[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       0);

                    coveredAreas05[i][j] = cv::Point3d(primaryValue,
                                                       secondaryValue,
                                                       0);
                }

                std::cerr << "Finished calculation of experiment configuration " << primaryValues[i] << " " << secondaryValues[j] << " with error " << avgErrors[i][j].z << std::endl;
                std::cout << "Finished calculation of experiment configuration " << primaryValues[i]<< " " << secondaryValues[j] <<  " with error " << avgErrors[i][j].z << std::endl;
                qDebug() << "Finished calculation of experiment configuration" << primaryValues[i] << secondaryValues[j] <<  "with error" << avgErrors[i][j].z;

            }
        }

        QString captureFlagsString;
        int targetsCaptured = 0;
        for(int i = 0;
            i < targetCaptureFlags.size();
            ++i)
        {
            if(targetCaptureFlags[i] != 0)
            {
                ++targetsCaptured;
                captureFlagsString.append('1');
            }
            else
            {
                captureFlagsString.append('0');
            }
        }

        dFileOutput("CaptureFlags.txt") << targetsCaptured << captureFlagsString;
        dFileOutput("PrimaryValues.txt") << rce::accuracy::vectorToCSV(primaryValues);
        dFileOutput("SecondaryValues.txt") << rce::accuracy::vectorToCSV(secondaryValues);

        dWarning() << "Error matrix:";
        for(size_t i = 0;
            i < primaryValues.size();
            ++i)
        {
            dFileOutput("ErrorMatrix.txt") << rce::accuracy::vectorToCSV(avgErrors[i],2);
        }


        dWarning() << "Cov02 matrix:";
        for(size_t i = 0;
            i < primaryValues.size();
            ++i)
        {
            dFileOutput("Cov02Matrix.txt") << rce::accuracy::vectorToCSV(coveredAreas02[i],2);
        }



        dWarning() << "Cov03 matrix:";
        for(size_t i = 0;
            i < primaryValues.size();
            ++i)
        {
            dFileOutput("Cov03Matrix.txt") << rce::accuracy::vectorToCSV(coveredAreas03[i],2);
        }


        dWarning() << "Cov04 matrix:";
        for(size_t i = 0;
            i < primaryValues.size();
            ++i)
        {
            dFileOutput("Cov04Matrix.txt") << rce::accuracy::vectorToCSV(coveredAreas04[i],2);
        }


        dWarning() << "Cov05 matrix:";
        for(size_t i = 0;
            i < primaryValues.size();
            ++i)
        {
            dFileOutput("Cov05Matrix.txt") << rce::accuracy::vectorToCSV(coveredAreas05[i],2);
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

    return -50;
}
