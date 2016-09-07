#include "RImageSequencePointAnnotationWidget.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QDir>
#include <QFileDialog>
#include <QFile>
#include <QDomDocument>
#include <QTextDocument>
#include <QMessageBox>
#include <QScrollArea>
#include <QSpinBox>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "rce/gui/RPointClickingScene.h"
#include "rce/gui/RScalableGraphicsView.h"
#include "rce/gui/RGuiUtils.h"
#include "rce/utility/RXmlConversion.h"

rce::gui::RImageSequencePointAnnotationWidget::
RImageSequencePointAnnotationWidget(QWidget *parent):
    QWidget(parent),
    currentImageID_(-1)
{
    prepareGUI();
}

void
rce::gui::RImageSequencePointAnnotationWidget::
retrieveAnnotations(std::vector<std::vector<cv::Point2d> > &annotatedPoints,
                    std::vector<std::vector<uchar> > &annotationFlags) const
{
    annotatedPoints = annotatedPoints_;
    annotationFlags = annotationFlags_;
}

void
rce::gui::RImageSequencePointAnnotationWidget::
setNumberOfPoints(int numOfPoints)
{
    // prepare data
    for(int i = 0;
        i < annotatedPoints_.size();
        ++i)
    {
        annotatedPoints_[i].resize(numOfPoints, cv::Point2d(0,0));
        annotationFlags_[i].resize(numOfPoints, 0);
    }

    // clear the widgets
    // repopulate widgets
    ptIndexSpinBox_->setRange(-1, numOfPoints-1);

    if(numOfPoints > 0)
    {
        ptIndexSpinBox_->setValue(0);
    }
}

void
rce::gui::RImageSequencePointAnnotationWidget::
setImageFolderPath(const QString &path)
{
    QDir pathDir(path);
    QStringList paths = pathDir.entryList(QStringList()<<"*.png" <<"*.jpg",
                                                QDir::Readable | QDir::Files | QDir::NoDotAndDotDot);

    images_.clear();
    imagePaths_.clear();
    annotatedPoints_.clear();
    annotationFlags_.clear();
    for(int i = 0;
        i < paths.size();
        ++i)
    {
        QString fullPath = pathDir.absoluteFilePath(paths[i]);
        cv::Mat image = cv::imread(fullPath.toStdString());
        if(!image.empty())
        {
            images_.push_back(image);
            imagePaths_.push_back(fullPath);
            annotatedPoints_.push_back(std::vector<cv::Point2d>(getNumberOfPoints()));
            annotationFlags_.push_back(std::vector<uchar>(getNumberOfPoints(), 0));
        }
    }
    if(getNumberOfPoints() > 0)
    {
        ptIndexSpinBox_->setValue(0);
    }
    currentImageID_ = 0;
    updateCurrentImage();
}

void rce::gui::RImageSequencePointAnnotationWidget::setCameraParamsFile(const QString &path)
{
    if(loadCameraIntrinsics(path))
    {
        cv::Mat mapX, mapY;
        cv::initUndistortRectifyMap(cameraMatrix_,
                                    distortionCoefficients_,
                                    cv::noArray(),
                                    cameraMatrix_,
                                    cameraFrameSize_,
                                    CV_16SC2,
                                    mapX,
                                    mapY);

        for(int i = 0;
            i < images_.size();
            ++i)
        {
            cv::Mat tmp;
            cv::remap(images_[i],
                      tmp,
                      mapX,
                      mapY,
                      cv::INTER_CUBIC);
            images_[i] = tmp;
        }

        currentImageID_ = 0;
        updateCurrentImage();
    }
    else
    {
        qDebug() << "Could not load camera intrinsics";
    }
}

void
rce::gui::RImageSequencePointAnnotationWidget::
moveToPreviousImage()
{
    if(getNumberOfPoints() > 0)
    {
        ptIndexSpinBox_->setValue(0);
    }
    currentImageID_ = qBound<int>(0, currentImageID_ - 1, static_cast<int>(images_.size()) - 1);
    updateCurrentImage();
}

void
rce::gui::RImageSequencePointAnnotationWidget::
moveToNextImage()
{
    if(getNumberOfPoints() > 0)
    {
        ptIndexSpinBox_->setValue(0);
    }
    currentImageID_ = qBound<int>(0, currentImageID_ + 1, static_cast<int>(images_.size()) - 1);
    updateCurrentImage();
}

void
rce::gui::RImageSequencePointAnnotationWidget::
onPointClickedInScene(const QPointF &scenePos)
{
    int currentPtIdx = getCurrentPointIdx();
    if((currentPtIdx >= 0) &&
       (isCurrentImageValid()))
    {
        scene_->addFlag(currentPtIdx,
                        scenePos,
                        QString::number(currentPtIdx));
        annotatedPoints_[currentImageID_][currentPtIdx] = cv::Point2d(scenePos.x(),
                                                                      scenePos.y());
        annotationFlags_[currentImageID_][currentPtIdx] = 255;
        moveToAnotherPoint();
    }
}

void
rce::gui::RImageSequencePointAnnotationWidget::
selectImageFolder()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Select folder with images"));
    if(!path.isEmpty())
    {
        setImageFolderPath(path);
    }

    path = QFileDialog::getOpenFileName(this,
                                        tr("Select camera parameters file"));
    if(!path.isEmpty())
    {
        setCameraParamsFile(path);
    }
}

void
rce::gui::RImageSequencePointAnnotationWidget::
rejectCurrentImage()
{
    if(isCurrentImageValid())
    {
        imagePaths_.removeAt(currentImageID_);
        images_.removeAt(currentImageID_);
        annotatedPoints_.erase(annotatedPoints_.begin() + currentImageID_);
        annotationFlags_.erase(annotationFlags_.begin() + currentImageID_);
        currentImageID_ = qBound<int>(0, currentImageID_, static_cast<int>(images_.size()) - 1);
        updateCurrentImage();
    }
}

void
rce::gui::RImageSequencePointAnnotationWidget::
saveAnnotations()
{

    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Export annotations"),
                                                    QString(),
                                                    tr("XML file (*.xml)"));
    if(!filePath.isEmpty())
    {
        QFile outFile(filePath);
        if(outFile.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
        {
            QTextStream s(&outFile);
            QDomDocument doc("image_list_annotations");
            QDomElement rootElement = doc.createElement("list");
            for(int i = 0;
                i < imagePaths_.size();
                ++i)
            {
                QDomElement annotationElement = doc.createElement("annotation");
                annotationElement.setAttribute("path",
                                                imagePaths_[i]);
                QDomElement ptsElement = doc.createElement("points");
                rce::utility::RXmlConversion::writeToQDomElement(ptsElement,
                                                                 cv::Mat(annotatedPoints_[i]));
                annotationElement.appendChild(ptsElement);
                QDomElement flagsElement = doc.createElement("flags");
                rce::utility::RXmlConversion::writeToQDomElement(flagsElement,
                                                                 cv::Mat(annotationFlags_[i]));
                annotationElement.appendChild(flagsElement);
                rootElement.appendChild(annotationElement);

                QDomElement cameraMatrixElement = doc.createElement("cam_matrix");
                rce::utility::RXmlConversion::writeToQDomElement(cameraMatrixElement,
                                                                 cameraMatrix_);
                cameraMatrixElement.setAttribute("camera_frame_width",
                                                 cameraFrameSize_.width);
                cameraMatrixElement.setAttribute("camera_frame_height",
                                                 cameraFrameSize_.height);
                rootElement.appendChild(cameraMatrixElement);
                QDomElement cameraDistortionElement = doc.createElement("cam_distortion");
                rce::utility::RXmlConversion::writeToQDomElement(cameraDistortionElement,
                                                                 distortionCoefficients_);
                rootElement.appendChild(cameraDistortionElement);
            }
            doc.appendChild(rootElement);
            doc.save(s, 4);

        }
        else
        {
            QMessageBox::critical(this,
                                  tr("Could not write the file"),
                                  tr("The application could not write the file."));
        }
    }
}

void
rce::gui::RImageSequencePointAnnotationWidget::
loadAnnotations()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Load annotations"),
                                                    QString(),
                                                    tr("XML file (*.xml)"));
    if(!filePath.isEmpty())
    {
        QFile inputFile(filePath);
        if(inputFile.open(QFile::ReadOnly | QFile::Text))
        {
            QDomDocument doc("image_list_annotations");
            if(doc.setContent(&inputFile))
            {
                imagePaths_.clear();
                images_.clear();
                annotatedPoints_.clear();
                annotationFlags_.clear();

                QDomElement rootElement = doc.firstChildElement("list");
                // load distortion data
                QDomElement cameraElement = rootElement.firstChildElement("cam_matrix");
                rce::utility::RXmlConversion::readFromQDomElement(cameraElement,
                                                                  cameraMatrix_);
                cv::Mat mapX, mapY;
                if(!cameraMatrix_.empty())
                {
                    cameraFrameSize_.width = cameraElement.attribute("camera_frame_width").toInt();
                    cameraFrameSize_.height = cameraElement.attribute("camera_frame_height").toInt();

                    QDomElement cameraDistortionElement = rootElement.firstChildElement("cam_distortion");
                    rce::utility::RXmlConversion::readFromQDomElement(cameraDistortionElement,
                                                                      distortionCoefficients_);

                    cv::initUndistortRectifyMap(cameraMatrix_,
                                                distortionCoefficients_,
                                                cv::noArray(),
                                                cameraMatrix_,
                                                cameraFrameSize_,
                                                CV_16SC2,
                                                mapX,
                                                mapY);
                }

                QDomElement annotationElement = rootElement.firstChildElement("annotation");
                while(!annotationElement.isNull())
                {
                    QString imgPath = annotationElement.attribute("path");

                    std::string imgPathStd = imgPath.toStdString();
                    cv::Mat img = cv::imread(imgPathStd);
                    if(!img.empty())
                    {
                        QDomElement ptsElement = annotationElement.firstChildElement("points");
                        cv::Mat pts;
                        rce::utility::RXmlConversion::readFromQDomElement(ptsElement,
                                                                          pts);
                        QDomElement flagsElement = annotationElement.firstChildElement("flags");
                        cv::Mat flags;
                        rce::utility::RXmlConversion::readFromQDomElement(flagsElement,
                                                                          flags);

                        imagePaths_.push_back(imgPath);
                        if(mapX.empty())
                        {
                            images_.push_back(img);
                        }
                        else
                        {
                            cv::Mat undistortedImg;
                            cv::remap(img,undistortedImg,
                                      mapX,mapY,
                                      cv::INTER_CUBIC);
                            images_.push_back(undistortedImg);
                        }
                        annotatedPoints_.push_back(pts);
                        annotationFlags_.push_back(flags);
                    }

                    annotationElement = annotationElement.nextSiblingElement("annotation");
                }

                if(annotationFlags_.size() > 0)
                {
                    setNumberOfPoints(annotationFlags_[0].size());
                }
                else
                {
                    setNumberOfPoints(0);
                }

                if(getNumberOfPoints() > 0)
                {
                    ptIndexSpinBox_->setValue(0);
                }
                currentImageID_ = 0;
                updateCurrentImage();
            }
            else
            {
                QMessageBox::critical(this,
                                      tr("Could not parse the file"),
                                      tr("The provided file could not be parsed."));
            }

        }
        else
        {
            QMessageBox::critical(this,
                                  tr("Could not open file"),
                                  tr("The provided file could not be open."));
        }
    }
}

void rce::gui::RImageSequencePointAnnotationWidget::onFlagRemoveRequested(qint64 id)
{
    annotationFlags_[currentImageID_][id] = 0;
    scene_->removeFlag(id);
}

void
rce::gui::RImageSequencePointAnnotationWidget::
prepareGUI()
{
    QGridLayout *mainLayout = new QGridLayout(this);

    // add scene and view
    scene_ = new rce::gui::RPointClickingScene(this);
    connect(scene_, SIGNAL(clicked(QPointF)),
            this, SLOT(onPointClickedInScene(QPointF)));
    connect(scene_, SIGNAL(flagRemoveRequested(qint64)),
            this, SLOT(onFlagRemoveRequested(qint64)));
    view_ = new rce::gui::RScalableGraphicsView(scene_,
                                                this);
    view_->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    mainLayout->addWidget(view_,
                          0,0,10,1);

    QPushButton *selectImagesButton = new QPushButton(tr("Select images"),
                                                      this);
    connect(selectImagesButton, SIGNAL(clicked()),
            this, SLOT(selectImageFolder()));
    mainLayout->addWidget(selectImagesButton,
                          0,1,1,1);

    QPushButton *nextImageButton = new QPushButton(tr("Next Image"),
                                                   this);
    connect(nextImageButton, SIGNAL(clicked()),
            this, SLOT(moveToNextImage()));
    mainLayout->addWidget(nextImageButton,
                          1,1,1,1);

    QPushButton *previousImageButton = new QPushButton(tr("Previous Image"),
                                                       this);
    connect(previousImageButton, SIGNAL(clicked()),
            this, SLOT(moveToPreviousImage()));
    mainLayout->addWidget(previousImageButton,
                          2,1,1,1);


    QPushButton *rejectImageButton = new QPushButton(tr("Reject Image"),
                                                     this);
    connect(rejectImageButton, SIGNAL(clicked()),
            this, SLOT(rejectCurrentImage()));
    mainLayout->addWidget(rejectImageButton,
                          3,1,1,1);

    statusLabel_ = new QLabel(tr("Uninitialised"),
                              this);
    mainLayout->addWidget(statusLabel_,
                          4,1,1,1);




    mainLayout->addWidget(new QLabel(tr("Current Point Idx:")),
                          5,1,1,1);
    ptIndexSpinBox_ = new QSpinBox(this);
    ptIndexSpinBox_->setRange(-1,-1);
    ptIndexSpinBox_->setSpecialValueText(tr("Invalid Point Index"));
    ptIndexSpinBox_->setValue(-1);
    mainLayout->addWidget(ptIndexSpinBox_,
                          6,1,1,1);

    QPushButton *saveAnnotationsButton = new QPushButton(tr("Save annotations"),
                                                         this);
    connect(saveAnnotationsButton, SIGNAL(clicked()),
            this, SLOT(saveAnnotations()));
    mainLayout->addWidget(saveAnnotationsButton,
                          7,1,1,1);

    QPushButton *loadAnnotationsButton = new QPushButton(tr("Load Annotations"),
                                                         this);
    connect(loadAnnotationsButton, SIGNAL(clicked(bool)),
            this, SLOT(loadAnnotations()));
    mainLayout->addWidget(loadAnnotationsButton,
                          8,1,1,1);

    QPushButton *estimateAccuracyButton = new QPushButton(tr("Estimate Accuracy"),
                                                          this);
    connect(estimateAccuracyButton, SIGNAL(clicked()),
            this, SIGNAL(estimateAccuracyRequested()));
    mainLayout->addWidget(estimateAccuracyButton,
                          9,1,1,1);

    mainLayout->setColumnStretch(0,1);
    mainLayout->setColumnStretch(1,0);
    mainLayout->setRowStretch(0,0);
    mainLayout->setRowStretch(1,0);
    mainLayout->setRowStretch(2,0);
    mainLayout->setRowStretch(3,0);
    mainLayout->setRowStretch(4,0);
    mainLayout->setRowStretch(5,0);
    mainLayout->setRowStretch(6,0);







    setLayout(mainLayout);
}

void
rce::gui::RImageSequencePointAnnotationWidget::
updateCurrentImage()
{
    if(isCurrentImageValid())
    {
        scene_->setBackgroundImage(images_[currentImageID_]);
        scene_->clear();
        for(int i = 0;
            i < annotationFlags_[currentImageID_].size();
            ++i)
        {
            if(annotationFlags_[currentImageID_][i] != 0)
            {
                scene_->addFlag(i,
                                QPointF(annotatedPoints_[currentImageID_][i].x,
                                        annotatedPoints_[currentImageID_][i].y),
                                QString::number(i));
            }
        }

        statusLabel_->setText(tr("Image %1 / %2")
                              .arg(currentImageID_)
                              .arg(annotatedPoints_.size()));
    }
    else
    {

        scene_->setBackgroundImage(cv::Mat());
        scene_->clear();


        ptIndexSpinBox_->setValue(-1);

        statusLabel_->setText(tr("No Image Selected"));
    }
}

bool rce::gui::RImageSequencePointAnnotationWidget::loadCameraIntrinsics(const QString &filePath)
{
    try
    {
        cv::FileStorage cameraParams;
        if(cameraParams.open(filePath.toStdString(), cv::FileStorage::READ))
        {
            cameraParams["camera_matrix"] >> cameraMatrix_;
            cameraParams["distortion_coefficients"] >> distortionCoefficients_;
            cameraParams["image_width"] >> cameraFrameSize_.width;
            cameraParams["image_height"] >> cameraFrameSize_.height;

//            if((cameraParams["new_camera_matrix"].empty()))
//            {
//                newCameraMatrix_ = cameraMatrix_;
//            }
//            else
//            {
//                cameraParams["new_camera_matrix"] >> newCameraMatrix_;
//            }

            if((!cameraMatrix_.empty()) && (!distortionCoefficients_.empty()))
            {
                return true;
            }
        }

        return false;
    }
    catch(...)
    {
        return false;
    }
}

int
rce::gui::RImageSequencePointAnnotationWidget::
getCurrentPointIdx() const
{
    return ptIndexSpinBox_->value();
}

int
rce::gui::RImageSequencePointAnnotationWidget::
getNumberOfPoints() const
{
    return ptIndexSpinBox_->maximum() + 1;
}

void
rce::gui::RImageSequencePointAnnotationWidget::
moveToAnotherPoint() const
{
    int currentPtIdx = getCurrentPointIdx();
    int nextPtIdx;
    if(getNumberOfPoints() == 0)
    {
        nextPtIdx = -1;
    }
    else if(currentPtIdx < 0)
    {
        nextPtIdx = 0;
    }
    else if(currentPtIdx == (getNumberOfPoints() - 1))
    {
        nextPtIdx = 0;
    }
    else
    {
        nextPtIdx = currentPtIdx+1;
    }



    ptIndexSpinBox_->setValue(nextPtIdx);
}

bool
rce::gui::RImageSequencePointAnnotationWidget::
isCurrentImageValid() const
{
    if(images_.size() > 0)
    {
        return ((currentImageID_ >= 0) &&
                (currentImageID_ < images_.size()));
    }
    else
    {
        return false;
    }
}
