#ifndef CHARTTOEFM_H
#define CHARTTOEFM_H

#include <QMainWindow>
#include <QGraphicsLineItem>
#include <QVector>
#include <QPair>
#include <QGraphicsItem>

class CustomGraphicsView;
class QGraphicsScene;
class QPushButton;
class QLineEdit;
class QVBoxLayout;

class AxisManager;
class DataPointManager;
class LineEditor;

class ChartToEFM : public QMainWindow
{
    Q_OBJECT
public:
    explicit ChartToEFM(QWidget* parent = nullptr);
    ~ChartToEFM();

protected:
    // Event filter will capture clicks on the image view.
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onUploadChartClicked();
    void onRotateButtonClicked();
    void onDefineHorizontalClicked();
    void onDefineVerticalClicked();
    void onAddHorizontalCalibrationClicked();
    void onAddVerticalCalibrationClicked();
    void onNewLineButtonClicked();
    void onUndoButtonClicked();

private:
    void setupUI();

    // Enumeration for axis/callibration modes (for existing axis functionality)
    enum AxisDefinitionMode {
        NoAxis,
        DefiningHorizontal,
        DefiningVertical,
        CalibratingHorizontal,
        CalibratingVertical
    };

    AxisDefinitionMode axisMode;
    bool waitingForSecondClick;
    QPointF firstAxisPoint;

    // Left panel UI elements.
    QPushButton* uploadChartButton;
    QLineEdit* chartNameLineEdit;

    QPushButton* defineHorizontalButton;
    QLineEdit* horizontalAxisNameLineEdit;
    QLineEdit* horizontalMinLineEdit;
    QLineEdit* horizontalMaxLineEdit;
    QPushButton* addHorizontalCalibButton;

    QPushButton* defineVerticalButton;
    QLineEdit* verticalAxisNameLineEdit;
    QLineEdit* verticalMinLineEdit;
    QLineEdit* verticalMaxLineEdit;
    QPushButton* addVerticalCalibButton;

    // Container for dynamic line editors.
    QWidget* lineEditorsContainer;
    QVBoxLayout* lineEditorsLayout;
    QPushButton* newLineButton;

    // Right panel UI elements.
    CustomGraphicsView* imageView;
    QGraphicsScene* scene;
    QPushButton* rotateButton;
    QPushButton* undoButton;

    // Undo stack stores pointers to drawn QGraphicsItems.
    QVector<QGraphicsItem*> undoStack;

    // Manager objects.
    AxisManager* axisManager;
    DataPointManager* dataPointManager;

    // Pointer to current LineEditor when in "define points" mode.
    LineEditor* currentLineEditor;
};

#endif // CHARTTOEFM_H
