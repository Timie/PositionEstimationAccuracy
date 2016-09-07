#ifndef RCE_GUI_RGUIUTILS_H
#define RCE_GUI_RGUIUTILS_H

class QStandardItem;
class QPushButton;
class QSplitter;
class QPointF;
class QGraphicsView;
class QRectF;
class QColor;
class QGroupBox;
class QWidget;
class QLayout;
class QString;
class QMenu;
class QToolBar;
class QComboBox;
class QTableView;
class QStyle;


#define RCE_ITEM_ID_ROLE (Qt::UserRole + 1)
#define RCE_ITEM_TYPE_ROLE (Qt::UserRole + 2)
#define RCE_ITEM_TARGET_ID_ROLE (Qt::UserRole + 3)
#define RCE_ITEM_TARGET_TYPE_ROLE (Qt::UserRole + 4)

#define RCE_ITEM_INDEX_ROLE (Qt::UserRole + 5)
#define RCE_ITEM_TAG_ROLE (Qt::UserRole + 6)

// size of the rectangle around cursor which will be used to select items
#define RCE_SELECTION_TOLERANCY (5)


#define RCE_FLAG_DSPLCMNT_X (7)
#define RCE_FLAG_DSPLCMNT_Y (7)
#define RCE_EMPHASED_LINE_WIDTH (2) // FIXME: Stop using this
// #define RCE_ITEM_LABEL_FONT_SIZE (10)


namespace rce {
    namespace gui {


        struct RTypeVisiblity
        {
            RTypeVisiblity():
                showAtAll_(true),
                showTrajectory_(true),
                showInfoFlag_(true)
            {}


            bool showAtAll_; // if false, nothing is rendered at all, when object is not selected
            bool showTrajectory_; // if false, trajectory is not shown when object is not selected
            bool showInfoFlag_; // if false, info flag is not shown when object is not selected (just red dot)
        };


        void enableItem(QComboBox *comboBox,
                        int itemIdx,
                        bool enable = true);
        void hideMenuIfActionsHidden(QMenu *menu);
        void hideUselessSeparators(QToolBar *toolbar);
        void disableInteractionItem(QStandardItem *it);
        void clearItemsFromItem(QStandardItem *it);
        void setMinimumButtonSize(QPushButton *button);
        void decorateSplitter(QSplitter* splitter,
                              int index);

        QRectF getSceneRectAroundScreenPos(const QPointF &screenPos,
                                           const QPointF &scenePos,
                                           const QGraphicsView *view);

        QPointF getScenePosFromScreen(const QPointF &screenPos,
                                      const QGraphicsView *view);

        QColor getColdHotColour(double min,
                                double value,
                                double max);

        QColor getContrastingColour(const QColor &colour);

        QStyle *getStyleForWidget(QWidget *w);

        int calculateMinimalTableHeight(const QTableView* w);
        int calculateMinimalTableWidth(const QTableView* w);


        QGroupBox *wrapInGroupBox(QWidget *widget,
                                  const QString &title,
                                  QWidget *parent = nullptr);
        QGroupBox *wrapInGroupBox(QLayout *layout,
                                  const QString &title,
                                  QWidget *parent = nullptr);

        double estimateViewScale(const QGraphicsView *view);

        class RGuiUtils
        {
        public:
            RGuiUtils();
        };

    } // namespace gui
} // namespace rce

#endif // RCE_GUI_RGUIUTILS_H
