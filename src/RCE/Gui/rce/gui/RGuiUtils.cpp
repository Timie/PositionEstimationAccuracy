#include "RGuiUtils.h"

#include <QStandardItem>
#include <QPushButton>
#include <QStyleOptionButton>
#include <QSplitter>
#include <QSplitterHandle>
#include <QFrame>
#include <QHBoxLayout>
#include <QPointF>
#include <QGraphicsView>
#include <QRectF>
#include <QDebug>
#include <QColor>
#include <QGroupBox>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QComboBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>

#include <cmath>

void
rce::gui::
disableInteractionItem(QStandardItem *it)
{
    it->setCheckable(false);
    it->setDragEnabled(false);
    it->setDropEnabled(false);
    it->setEditable(false);
    it->setSelectable(false);
    it->setTristate(false);
}


rce::gui::RGuiUtils::
RGuiUtils()
{

}


void
rce::gui::
clearItemsFromItem(QStandardItem *it)
{
    while(it->rowCount() > 0)
    {
        QList< QStandardItem* > items = it->takeRow(0);
        qDeleteAll( items ); // children items will be desctructed as well
    }
}


void
rce::gui::
setMinimumButtonSize(QPushButton *button)
{
    //qDebug() << "Setting minimum size for button" << button->text() << "...";
    QSize textSize = button->fontMetrics().size(Qt::TextShowMnemonic, button->text());
    QStyleOptionButton opt;
    opt.initFrom(button);
    opt.rect.setSize(textSize);
    QSize minimumSize = button->style()->sizeFromContents(QStyle::CT_PushButton,
                                                          &opt,
                                                          textSize,
                                                          button);
    if(minimumSize.width() < minimumSize.height())
        minimumSize.setWidth(minimumSize.height());
    button->setMinimumSize(minimumSize);

    //qDebug() << "Minimum size" << minimumSize << "for button" << button->text() << "has been set.";
}


void
rce::gui::
decorateSplitter(QSplitter *splitter,
                 int index)
{
    // source: http://stackoverflow.com/questions/2545577/qsplitter-becoming-undistinguishable-between-qwidget-and-qtabwidget
    const int gripLength = 25;
    const int gripWidth = 1;
    const int grips = 3;

    QSplitterHandle* handle = splitter->handle(index);
    Qt::Orientation orientation = splitter->orientation();
    QHBoxLayout* layout = new QHBoxLayout(handle);
    layout->setSpacing(1);
    layout->setMargin(0);

    if (orientation == Qt::Horizontal)
    {
        for (int i=0;i<grips;++i)
        {
            QFrame* line = new QFrame(handle);
            line->setMinimumSize(gripWidth, gripLength);
            line->setMaximumSize(gripWidth, gripLength);
            line->setFrameShape(QFrame::StyledPanel);
            layout->addWidget(line);
        }
    }
    else
    {
        //this will center the vertical grip
        //add a horizontal spacer
        layout->addStretch();
        //create the vertical grip
        QVBoxLayout* vbox = new QVBoxLayout;
        for (int i=0;i<grips;++i)
        {
            QFrame* line = new QFrame(handle);
            line->setMinimumSize(gripLength, gripWidth);
            line->setMaximumSize(gripLength, gripWidth);
            line->setFrameShape(QFrame::StyledPanel);
            vbox->addWidget(line);
        }
        layout->addLayout(vbox);
        //add another horizontal spacer
        layout->addStretch();
    }

    handle->setContentsMargins(0,0,0,0);
}


QRectF
rce::gui::
getSceneRectAroundScreenPos(const QPointF &screenPos,
                            const QPointF &scenePos,
                            const QGraphicsView *view)
{
    if(view != NULL)
    {

        QPolygonF transformedRect;
        transformedRect.push_back(QPointF(screenPos.x() - RCE_SELECTION_TOLERANCY,
                                          screenPos.y() - RCE_SELECTION_TOLERANCY));
        transformedRect.push_back(QPointF(screenPos.x() + RCE_SELECTION_TOLERANCY,
                                          screenPos.y() + RCE_SELECTION_TOLERANCY));
        transformedRect.push_back(QPointF(screenPos.x() - RCE_SELECTION_TOLERANCY,
                                          screenPos.y() + RCE_SELECTION_TOLERANCY));
        transformedRect.push_back(QPointF(screenPos.x() + RCE_SELECTION_TOLERANCY,
                                          screenPos.y() - RCE_SELECTION_TOLERANCY));

        for(int i = 0;
            i < transformedRect.size();
            ++i)
        {
            transformedRect[i] = view->mapToScene(view->mapFromGlobal(transformedRect[i].toPoint()));
        }

        return transformedRect.boundingRect();
    }
    else
    {

        //qDebug() << "rce::gui::getSceneRectAroundScreenPos: View is null...";
        return QRectF(QPointF(scenePos.x() - RCE_SELECTION_TOLERANCY,
                              scenePos.y() - RCE_SELECTION_TOLERANCY),
                      QPointF(scenePos.x() + RCE_SELECTION_TOLERANCY,
                              scenePos.y() + RCE_SELECTION_TOLERANCY));
    }
}

QPointF
rce::gui::
getScenePosFromScreen(const QPointF &screenPos,
                      const QGraphicsView *view)
{
    if(view != NULL)
    {
        QPointF transformedPos = view->mapToScene(view->mapFromGlobal(screenPos.toPoint()));

        return transformedPos;
    }
    return QPointF();
}

QColor
rce::gui::
getColdHotColour(double vmin,
                 double v,
                 double vmax)
{
    // http://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale

    QColor c(Qt::white);
    double dv;

    if (v < vmin)
       v = vmin;
    if (v > vmax)
       v = vmax;
    dv = vmax - vmin;

    if (v < (vmin + 0.25 * dv)) {
       c.setRedF(0);
       c.setGreenF(4 * (v - vmin) / dv);
    } else if (v < (vmin + 0.5 * dv)) {
       c.setRedF(0);
       c.setBlueF(1 + 4 * (vmin + 0.25 * dv - v) / dv);
    } else if (v < (vmin + 0.75 * dv)) {
       c.setRedF(4 * (v - vmin - 0.5 * dv) / dv);
       c.setBlueF(0);
    } else {
       c.setGreenF(1 + 4 * (vmin + 0.75 * dv - v) / dv);
       c.setBlueF(0);
    }

    return(c);
}


QGroupBox *
rce::gui::
wrapInGroupBox(QLayout *layout,
               const QString &title,
               QWidget *parent)
{
    QGroupBox *result = new QGroupBox(title, parent);

    result->setLayout(layout);

    return result;
}


QGroupBox *
rce::gui::
wrapInGroupBox(QWidget *widget,
               const QString &title,
               QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(nullptr);
    layout->addWidget(widget);

    return wrapInGroupBox(layout,
                          title,
                          parent);
}


void
rce::gui::
hideMenuIfActionsHidden(QMenu *menu)
{
    QList<QAction*> menuActions = menu->actions();
    for(int i = 0;
        i < menuActions.size();
        ++i)
    {
        if(menuActions[i]->isSeparator())
        {
            // skip over it
        }
        else if(menuActions[i]->isVisible())
        {
            return;
        }
    }

    menu->menuAction()->setVisible(false);
}


void
rce::gui::
hideUselessSeparators(QToolBar *toolbar)
{
    QList<QAction*> toolbarActions = toolbar->actions();

    QAction *lastSeparator = NULL;
    bool filledGap = false;
    for(int i = 0;
        i < toolbarActions.size();
        ++i)
    {
        if(toolbarActions[i]->isSeparator())
        {
            if(!filledGap)
            {
                lastSeparator = NULL;
                toolbarActions[i]->setVisible(false);
            }
            else
            {
                filledGap = false;
                lastSeparator = toolbarActions[i];
            }
        }
        else
        {
            if(toolbarActions[i]->isVisible())
            {
                filledGap = true;
            }
        }
    }

    if((!filledGap) &&
       (lastSeparator != NULL))
    {
        lastSeparator->setVisible(false);
    }


}


void
rce::gui::
enableItem(QComboBox *comboBox,
           int itemIdx,
           bool enable)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(comboBox->model());
    QStandardItem* item = model->item(itemIdx);

    item->setFlags(enable ?
                              (Qt::ItemIsSelectable|Qt::ItemIsEnabled)
                          :
                              (item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled)));

    // visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
    item->setData(enable ?
                             QVariant()
                         :
                             QVariant(comboBox->palette().color(QPalette::Disabled, QPalette::Text)), // clear item data in order to use default color
                  Qt::TextColorRole);
}

double
rce::gui::
estimateViewScale(const QGraphicsView *view)
{
    if(view != NULL)
    {
        const QTransform t = view->transform();
        double scaleHoriz = std::sqrt(t.m11() * t.m11() + t.m21() * t.m21());
        double scaleVertical = std::sqrt(t.m22() * t.m22() + t.m12() * t.m12());
        return (scaleHoriz + scaleVertical) / 2.0;
    }
    else
    {
        return 1;
    }

}

int
rce::gui::
calculateMinimalTableHeight(const QTableView *tv)
{
    // based on http://stackoverflow.com/questions/8766633/how-to-determine-the-correct-size-of-a-qtablewidget


    int desH = QApplication::desktop()->screen()->height();
    int extraTopHeight = tv->frameWidth();
    int extraBottomHeight =  tv->frameWidth();


    int h = tv->horizontalHeader()->height() + extraTopHeight + extraBottomHeight;
    int rowCount = 0;
    if(tv->model() != nullptr)
    {
        rowCount = tv->model()->rowCount() ;
    }
    for(int row = 0; row < rowCount; ++row ) {
        h += tv->rowHeight(row);
    }

    QStyle *style;
    if(tv->style())
    {
        style = tv->style();
    }
    else
    {
        style = QApplication::style();
    }

    if((h - extraTopHeight - extraBottomHeight - style->pixelMetric(QStyle::PM_TitleBarHeight)) > desH) {

        h = desH - (extraTopHeight + style->pixelMetric(QStyle::PM_TitleBarHeight) + extraBottomHeight);
    }

    return h;
}

QColor
rce::gui::
getContrastingColour(const QColor &colour)
{
    int wbCoeff = qGray(colour.rgb());
    if(wbCoeff > 127)
    {
        return QColor(Qt::black);
    }
    else
    {
        return QColor(Qt::white);
    }
}

int
rce::gui::
calculateMinimalTableWidth(const QTableView *tv)
{
    int desW = QApplication::desktop()->screen()->width();
    int extraLeftWidth = tv->frameWidth();
    int extraRightWidth =  tv->frameWidth();


    int w = (tv->verticalHeader()->isVisible() ? tv->verticalHeader()->width() : 0) +
            extraLeftWidth +
            extraRightWidth;
    int columnCount = 0;
    if(tv->model() != nullptr)
    {
        columnCount = tv->model()->columnCount();
    }
    for(int col = 0; col < columnCount; ++col) {
        w += tv->columnWidth(col);
    }

    if((w - extraLeftWidth - extraRightWidth) > desW) {

        w = desW - (extraLeftWidth + extraRightWidth);
    }

    return w;
}

QStyle *
rce::gui::
getStyleForWidget(QWidget *w)
{
    QStyle *style;
    if(w->style())
    {
        style = w->style();
    }
    else
    {
        style = QApplication::style();
    }

    return style;
}
