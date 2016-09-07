#ifndef RCE_GUI_RDISTANCEANNOTATIONWIDGET_H
#define RCE_GUI_RDISTANCEANNOTATIONWIDGET_H

#include <QWidget>

#include "rce/accuracy/RRealExperimentAnalyzer.h"

class QTabWidget;


namespace rce {
    namespace gui {
        class RDistanceMatrixScene;
        class RDistanceWidget;
        class RScalableGraphicsView;
        class RImageSequencePointAnnotationWidget;

        class RDistanceAnnotationWidget : public QWidget
        {
            Q_OBJECT
        public:
            RDistanceAnnotationWidget(QWidget *parent = NULL);




        protected slots:
            void onTabChanged(int idx);
            void onEstimateAccuracyRequested();

        private: // methods
            void prepareGUI();
            std::vector<cv::Point2d> getPoints();
            bool loadPoints();
        private: // attributes
            bool hasCSVPoints_;
            std::vector<cv::Point2d> csvPoints_;


            rce::accuracy::RRealExperimentAnalyzer analyzer_;
            // gui stuff
            RDistanceWidget *editWidget_;
            RDistanceMatrixScene *visScene_;
            RScalableGraphicsView *view_;
            RImageSequencePointAnnotationWidget *annotWidget_;
            QTabWidget *tabWidget_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RDISTANCEANNOTATIONWIDGET_H
