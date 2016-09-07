#include "RDistanceAnnotationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>



#include "rce/gui/RDistanceMatrixScene.h"
#include "rce/gui/RDistanceWidget.h"
#include "rce/gui/RScalableGraphicsView.h"
#include "rce/gui/RImageSequencePointAnnotationWidget.h"
#include "rce/accuracy/RDistanceMatrix.h"


rce::gui::RDistanceAnnotationWidget::
RDistanceAnnotationWidget(QWidget *parent):
    QWidget(parent),
    hasCSVPoints_(false)
{
    prepareGUI();
}

void
rce::gui::RDistanceAnnotationWidget::
onTabChanged(int idx)
{
    if(tabWidget_->currentWidget() == view_)
    {
        std::vector<cv::Point2d> positions = getPoints();
        if(positions.empty())
        {
            QMessageBox::critical(this,
                                  tr("Could not calculate positions"),
                                  tr("The positions of the points could not be calculated"));
            visScene_->clearScene();
        }
        else
        {
            visScene_->showPoints(positions,
                                  editWidget_->getDistanceMatrix());
            view_->centerOn(positions[0].x,
                    positions[0].y);
        }
    }
    else if(tabWidget_->currentWidget() == annotWidget_)
    {
        std::vector<cv::Point2d> positions = getPoints();
        if(positions.empty())
        {
            QMessageBox::warning(this,
                                 tr("Could not retrieve positions"),
                                 tr("The positions of the points could not be calculated."));
            tabWidget_->setCurrentWidget(editWidget_);
        }
        else
        {
            annotWidget_->setNumberOfPoints(positions.size());
        }
    }

}

void rce::gui::RDistanceAnnotationWidget::onEstimateAccuracyRequested()
{

    std::vector<cv::Point2d> positions = getPoints();


    std::vector<std::vector<cv::Point2d>> annotatedPoints;
    std::vector<std::vector<uchar>> annotationFlags;
    annotWidget_->retrieveAnnotations(annotatedPoints,
                                 annotationFlags);

    analyzer_.setData(positions,
                      annotatedPoints,
                      annotationFlags,
                      annotWidget_->getImages());

    QSettings settings("real_experiment_settings.ini",
                       QSettings::IniFormat);
    analyzer_.loadSettings(settings,
                           "analysis");
    analyzer_.setCameraParameters(annotWidget_->getCameraMatrix());

    analyzer_.performAnalysis();
}

void
rce::gui::RDistanceAnnotationWidget::
prepareGUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    tabWidget_ = new QTabWidget(this);
    connect(tabWidget_, SIGNAL(currentChanged(int)),
            this, SLOT(onTabChanged(int)));
    mainLayout->addWidget(tabWidget_);

    {
        // add edit widget
        editWidget_ = new RDistanceWidget(NULL);
        tabWidget_->addTab(editWidget_, tr("Edit distances"));
    }

    {
        // add scene and view
        visScene_ = new RDistanceMatrixScene(this);
        view_ = new RScalableGraphicsView(visScene_,
                                          NULL);
        view_->setScaleBounds(1,800);
        view_->setScaleStep(5);
        tabWidget_->addTab(view_, tr("Visualisation"));
    }

    {
        // add image scene annotation widget
        annotWidget_ = new RImageSequencePointAnnotationWidget(NULL);
        connect(annotWidget_, SIGNAL(estimateAccuracyRequested()),
                this, SLOT(onEstimateAccuracyRequested()));
        tabWidget_->addTab(annotWidget_,
                           tr("Annotation"));
    }


    setLayout(mainLayout);

}

std::vector<cv::Point2d> rce::gui::RDistanceAnnotationWidget::getPoints()
{
    if(!hasCSVPoints_)
    {
        rce::accuracy::RDistanceMatrix m = editWidget_->getDistanceMatrix();
        std::vector<cv::Point2d> positions = m.generatePositions3Points();
        if(positions.empty())
        {
            if(QMessageBox::warning(this,
                                  tr("Could not calculate positions"),
                                  tr("The positions of the points could not be calculated.\n"
                                     "Do you want to load points from CSV file instead?"),
                                    QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                if(loadPoints())
                {
                    qDebug() << "Loaded" << csvPoints_.size() << "points...";
                    return csvPoints_;
                }
            }
            return std::vector<cv::Point2d>();
        }
        else
        {
            return positions;
        }
    }
    else
    {
        return csvPoints_;
    }
}


bool rce::gui::RDistanceAnnotationWidget::loadPoints()
{
    QSettings settings;

    QString filePath = QFileDialog::getOpenFileName(this,
                                                    QString(),
                                                    settings.value("points_dir").toString());
    if(!filePath.isEmpty())
    {
        settings.setValue("points_dir",
                          QFileInfo(filePath).absoluteDir().absolutePath());

        hasCSVPoints_ = false;
        QFile csvFile(filePath);
        if(csvFile.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream ts(&csvFile);

            csvPoints_.clear();
            while(!ts.atEnd())
            {
                QStringList lineStr = ts.readLine().split(QChar(';'));
                if(lineStr.size() == 4)
                {
                    csvPoints_.push_back(cv::Point2d(lineStr[1].toDouble(),
                                         lineStr[2].toDouble()));
                }

            }

            hasCSVPoints_ = (csvPoints_.size() != 0);

            if(!hasCSVPoints_)
            {
                QMessageBox::critical(this,
                                      tr("No Points in file"),
                                      tr("No points in the provided file"));
            }

            return hasCSVPoints_;
        }
    }



    return false;
}
