#include "RDistanceWidget.h"

#include <QSpinBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QGridLayout>
#include <QFileDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "rce/utility/RXmlConversion.h"


rce::gui::RDistanceWidget::
RDistanceWidget(QWidget *parent):
    QWidget(parent)
{
    prepareGUI();
}

void
rce::gui::RDistanceWidget::
setDistanceMatrix(const rce::accuracy::RDistanceMatrix &m)
{
    distMatrix_ = m;
    refreshTable();
    numOfPlacesSpinBox_->setValue(distMatrix_.getNumOfPlaces());
}

void
rce::gui::RDistanceWidget::
setValue(int row,
         int column,
         double value)
{
    if(value < 0)
    {
        distMatrix_.clearValue(row, column);
        distMatrixTable_->item(row, column)->setText(QString());
        distMatrixTable_->item(column, row)->setText(QString());
    }
    else
    {
        if((!distMatrix_.isDistanceKnown(row, column)) ||
           (distMatrix_.getDistance(row, column) != value))
        {
            distMatrix_.setDistance(row, column,
                                    value);
            QString newText = QString::number(value, 'f', 4);
            if(newText != distMatrixTable_->item(row, column)->text())
            {
                distMatrixTable_->item(row, column)->setText(newText);
            }

            if(newText != distMatrixTable_->item(column, row)->text())
            {
                distMatrixTable_->item(column, row)->setText(newText);
            }
        }
    }
}

void
rce::gui::RDistanceWidget::
onNumOfPlacesSpinBoxChanged(int newValue)
{
    if(newValue != distMatrix_.getNumOfPlaces())
    {
        distMatrix_.resize(newValue);
        refreshTable();
    }
}

void
rce::gui::RDistanceWidget::
onLoadDistanceMatrixClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Load Distance Matrix"),
                                                    QString(),
                                                    tr("XML files (*.xml)"));
    if(!filePath.isEmpty())
    {
        QFile inputFile(filePath);
        if(inputFile.open(QFile::ReadOnly | QFile::Text))
        {
            QDomDocument doc("distance_matrix");
            if(doc.setContent(&inputFile))
            {
                QDomElement rootElement = doc.firstChildElement("distance_matrix");
                if(!rootElement.isNull())
                {
                    rce::accuracy::RDistanceMatrix m;
                    m.readFromQDomElement(rootElement);
                    setDistanceMatrix(m);
                    return;
                }
            }


            QMessageBox::critical(this,
                                  tr("Could not parse the file"),
                                  tr("The provided file could not be parsed."));
        }
        else
        {
            QMessageBox::critical(this,
                                  tr("Could not open file"),
                                  tr("The provided file could not be open."));
        }
    }
}

void
rce::gui::RDistanceWidget::
onSaveDistanceMatrixClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Save Distance Matrix"),
                                                    QString(),
                                                    tr("XML files (*.xml)"));
    if(!filePath.isEmpty())
    {
        QFile outputFile(filePath);
        if(outputFile.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
        {
            QTextStream s(&outputFile);
            QDomDocument doc("distance_matrix");
            QDomElement rootElement = doc.createElement("distance_matrix");
            distMatrix_.writeToQDomElement(rootElement);
            doc.appendChild(rootElement);
            doc.save(s, 4);
        }
        else
        {
            QMessageBox::critical(this,
                                  tr("Could not open file for writing"),
                                  tr("The applicaiton was unable to open the file for writing."));
        }
    }
}

void
rce::gui::RDistanceWidget::
onSavePointsClicked()
{
    std::vector<cv::Point2d> points = distMatrix_.generatePositions3Points();
    if(points.empty())
    {
        QMessageBox::critical(this,
                              tr("Could not generate points"),
                              tr("The application could not generate the points"));
    }
    else
    {
        QString filePath = QFileDialog::getSaveFileName(this,
                                                        tr("Export points"),
                                                        QString(),
                                                        tr("XML file (*.xml)"));
        if(!filePath.isEmpty())
        {
            QFile outFile(filePath);
            if(outFile.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
            {
                QTextStream s(&outFile);
                QDomDocument doc("coordinates_2d");
                QDomElement rootElement = doc.createElement("points");
                rce::utility::RXmlConversion::writeToQDomElement(rootElement,
                                                                 cv::Mat(points));
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
}

void
rce::gui::RDistanceWidget::
onDistMatrixCellChanged(int row, int column)
{
    double formerValue;
    if(distMatrix_.isDistanceKnown(row, column))
    {
        formerValue = distMatrix_.getDistance(row, column);
    }
    else
    {
        formerValue = -1;
    }

    QTableWidgetItem *item = distMatrixTable_->item(row, column);
    if(item->text().simplified().isEmpty())
    {
        if(distMatrix_.isDistanceKnown(row, column))
        {
            setValue(row, column,
                     -1);
        }
    }
    else
    {
        bool ok;
        double newValue = item->text().toDouble(&ok);
        if(!ok)
        {
            setValue(row, column,
                     formerValue);
        }
        else
        {
            if(row == column)
            {
                newValue = 0;
            }

            setValue(row, column,
                     newValue);
        }
    }
}

void
rce::gui::RDistanceWidget::
prepareGUI()
{
    QGridLayout *mainLayout = new QGridLayout(this);

    numOfPlacesSpinBox_ = new QSpinBox(this);
    numOfPlacesSpinBox_->setRange(0,100);
    numOfPlacesSpinBox_->setValue(0);
    connect(numOfPlacesSpinBox_, SIGNAL(valueChanged(int)),
            this, SLOT(onNumOfPlacesSpinBoxChanged(int)));
    mainLayout->addWidget(numOfPlacesSpinBox_,
                          0,0,1,1);

    QPushButton *loadButton = new QPushButton(tr("Load Matrix"),
                                              this);
    connect(loadButton, SIGNAL(clicked()),
            this, SLOT(onLoadDistanceMatrixClicked()));
    mainLayout->addWidget(loadButton,
                          0,1,1,1);


    QPushButton *saveButton = new QPushButton(tr("Save Matrix"),
                                              this);
    connect(saveButton, SIGNAL(clicked()),
            this, SLOT(onSaveDistanceMatrixClicked()));
    mainLayout->addWidget(saveButton,
                          0,2,1,1);

    QPushButton *exportPointsButton = new QPushButton(tr("Export Points"),
                                                      this);
    connect(exportPointsButton, SIGNAL(clicked()),
            this, SLOT(onSavePointsClicked()));
    mainLayout->addWidget(exportPointsButton,
                          0,3,1,1);


    distMatrixTable_ = new QTableWidget(this);
    connect(distMatrixTable_, SIGNAL(cellChanged(int,int)),
            this, SLOT(onDistMatrixCellChanged(int,int)));
    mainLayout->addWidget(distMatrixTable_,
                          1,0,1,mainLayout->columnCount());

    mainLayout->setRowStretch(0,0);
    mainLayout->setRowStretch(1,1);

    refreshTable();

    setLayout(mainLayout);
}

void
rce::gui::RDistanceWidget::
refreshTable()
{
    int numOfPlaces = distMatrix_.getNumOfPlaces();
    distMatrixTable_->setColumnCount(numOfPlaces);
    distMatrixTable_->setRowCount(numOfPlaces);

    for(int i = 0;
        i < numOfPlaces;
        ++i)
    {
        for(int j = 0;
            j < numOfPlaces;
            ++j)
        {
            if(distMatrix_.isDistanceKnown(i,j))
            {
                distMatrixTable_->setItem(i,j,
                                      new QTableWidgetItem(QString::number(distMatrix_.getDistance(i,j), 'f', 4)));
            }
            else
            {
                distMatrixTable_->setItem(i,j,
                                      new QTableWidgetItem());
            }
        }
    }
}
