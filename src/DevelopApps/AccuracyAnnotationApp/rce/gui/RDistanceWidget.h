#ifndef RCE_GUI_RDISTANCEWIDGET_H
#define RCE_GUI_RDISTANCEWIDGET_H

#include <QWidget>

#include "rce/accuracy/RDistanceMatrix.h"

class QSpinBox;
class QTableWidget;

namespace rce {
    namespace accuracy
    {
        class RDistanceMatrix;
    }

    namespace gui {

        class RDistanceWidget : public QWidget
        {
            Q_OBJECT
        public:
            RDistanceWidget(QWidget *parent = NULL);

        signals:
            void matrixChanged();
            void valueChanged(int idx1,
                             int idx2,
                             double newValue);
        public slots:
            void setDistanceMatrix(const rce::accuracy::RDistanceMatrix &m);

            const rce::accuracy::RDistanceMatrix &getDistanceMatrix() const {return distMatrix_;}

            void setValue(int row, int column,
                          double value);

        protected slots:
            void onNumOfPlacesSpinBoxChanged(int newValue);

            void onLoadDistanceMatrixClicked();
            void onSaveDistanceMatrixClicked();
            void onSavePointsClicked();

            void onDistMatrixCellChanged(int row, int column);


        protected: // methods
            void prepareGUI();
            void refreshTable();
        protected: // attributes
            // gui

            QSpinBox *numOfPlacesSpinBox_;
            QTableWidget *distMatrixTable_;

            // data
            rce::accuracy::RDistanceMatrix distMatrix_;
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RDISTANCEWIDGET_H
