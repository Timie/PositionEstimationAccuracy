#ifndef RCE_GUI_RIMAGESEQUENCEPOINTANNOTATIONWIDGET_H
#define RCE_GUI_RIMAGESEQUENCEPOINTANNOTATIONWIDGET_H

#include <QWidget>
#include <QVector>

#include <opencv2/core/core.hpp>

class QLabel;
class QSpinBox;

namespace rce {
    namespace gui {
        class RPointClickingScene;
        class RScalableGraphicsView;

        class RImageSequencePointAnnotationWidget : public QWidget
        {
            Q_OBJECT
        public:
            RImageSequencePointAnnotationWidget(QWidget *parent = NULL);

            const cv::Mat &getCameraMatrix() const {return cameraMatrix_;}

            void retrieveAnnotations(std::vector<std::vector<cv::Point2d>> &annotatedPoints,
                                     std::vector<std::vector<uchar>> &annotationFlags) const;
            const QVector<cv::Mat> &getImages() const {return images_;}
        public slots:
            void setNumberOfPoints(int numOfPoints);
            void setImageFolderPath(const QString &path);
            void setCameraParamsFile(const QString &path);
        signals:
            void estimateAccuracyRequested();
        protected slots:
            void moveToPreviousImage();
            void moveToNextImage();
            void onPointClickedInScene(const QPointF &scenePos);
            void selectImageFolder();
            void rejectCurrentImage();
            void saveAnnotations();
            void loadAnnotations();

            void onFlagRemoveRequested(qint64 id);
        protected: // methods
            void prepareGUI();
            void updateCurrentImage();
            bool loadCameraIntrinsics(const QString &filePath);
            int getCurrentPointIdx() const;
            int getNumberOfPoints() const;
            void moveToAnotherPoint() const;
            bool isCurrentImageValid() const;
        protected: // attributes
            // gui stuff
            QLabel *statusLabel_;

            RPointClickingScene *scene_;
            RScalableGraphicsView *view_;

            QSpinBox *ptIndexSpinBox_;

            // state
            int currentImageID_;


            // data
            QStringList imagePaths_;
            QVector<cv::Mat> images_;
            std::vector<std::vector<cv::Point2d>> annotatedPoints_;
            std::vector<std::vector<uchar>> annotationFlags_;

            cv::Mat cameraMatrix_;
            cv::Mat distortionCoefficients_;
            cv::Size cameraFrameSize_;



        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RIMAGESEQUENCEPOINTANNOTATIONWIDGET_H
