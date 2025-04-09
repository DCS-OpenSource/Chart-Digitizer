#include "ChartToEFM.h"
#include "AxisManager.h"
#include "DataPointManager.h"
#include "CustomGraphicsView.h"
#include "LineEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QScrollArea>
#include <QEvent>
#include <QMouseEvent>
#include <QInputDialog>
#include <QGraphicsTextItem>
#include <QFont>
#include <QString>
#include <QtMath>

// Helper function to format numbers.
QString formatCalibratedValue(double value)
{
    QString formatted = QString::number(value, 'f', 6);
    while (formatted.endsWith('0'))
        formatted.chop(1);
    if (formatted.endsWith('.'))
        formatted.chop(1);
    return formatted;
}

ChartToEFM::ChartToEFM(QWidget* parent)
    : QMainWindow(parent),
    axisMode(NoAxis),
    waitingForSecondClick(false),
    currentLineEditor(nullptr),
    imageView(nullptr),
    scene(nullptr),
    axisManager(nullptr),
    dataPointManager(nullptr)
{
    axisManager = new AxisManager();
    dataPointManager = new DataPointManager(axisManager);
    setupUI();
}

ChartToEFM::~ChartToEFM()
{
    delete axisManager;
    delete dataPointManager;
}

void ChartToEFM::setupUI()
{
    // Set dark mode with orange buttons.
    this->setStyleSheet(
        "QMainWindow { background-color: #1E1E1E; }"
        "QPushButton { background-color: #FF9800; color: white; border: none; border-radius: 4px; padding: 6px 12px; font: 12pt 'Segoe UI'; }"
        "QPushButton:hover { background-color: #e65100; }"
        "QLineEdit { background-color: #424242; color: white; border: 1px solid #555; padding: 4px; font: 11pt 'Segoe UI'; border-radius: 4px; }"
        "QGraphicsView { background-color: #212121; border: 1px solid #555; }"
        "QGroupBox { border: 1px solid #555; margin-top: 2ex; color: white; font: 11pt 'Segoe UI'; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top center; padding: 0 3px; }"
    );
    QFont baseFont("Segoe UI", 10);
    this->setFont(baseFont);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // --- Left Panel ---
    QWidget* leftPanel = new QWidget(centralWidget);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(10, 10, 10, 10);
    leftLayout->setSpacing(8);

    uploadChartButton = new QPushButton("Upload Chart", leftPanel);
    leftLayout->addWidget(uploadChartButton);
    connect(uploadChartButton, &QPushButton::clicked, this, &ChartToEFM::onUploadChartClicked);

    chartNameLineEdit = new QLineEdit(leftPanel);
    chartNameLineEdit->setPlaceholderText("Chart Name");
    leftLayout->addWidget(chartNameLineEdit);

    // --- Horizontal Axis Group ---
    QGroupBox* hGroup = new QGroupBox("Horizontal Axis", leftPanel);
    QVBoxLayout* hLayout = new QVBoxLayout(hGroup);
    defineHorizontalButton = new QPushButton("Define Horizontal Axis", hGroup);
    hLayout->addWidget(defineHorizontalButton);
    connect(defineHorizontalButton, &QPushButton::clicked, this, &ChartToEFM::onDefineHorizontalClicked);
    horizontalAxisNameLineEdit = new QLineEdit(hGroup);
    horizontalAxisNameLineEdit->setPlaceholderText("Axis Name");
    hLayout->addWidget(horizontalAxisNameLineEdit);
    horizontalMinLineEdit = new QLineEdit(hGroup);
    horizontalMinLineEdit->setPlaceholderText("Min Value");
    hLayout->addWidget(horizontalMinLineEdit);
    horizontalMaxLineEdit = new QLineEdit(hGroup);
    horizontalMaxLineEdit->setPlaceholderText("Max Value");
    hLayout->addWidget(horizontalMaxLineEdit);
    addHorizontalCalibButton = new QPushButton("Add Calibration", hGroup);
    hLayout->addWidget(addHorizontalCalibButton);
    connect(addHorizontalCalibButton, &QPushButton::clicked, this, &ChartToEFM::onAddHorizontalCalibrationClicked);
    leftLayout->addWidget(hGroup);

    // --- Vertical Axis Group ---
    QGroupBox* vGroup = new QGroupBox("Vertical Axis", leftPanel);
    QVBoxLayout* vLayout = new QVBoxLayout(vGroup);
    defineVerticalButton = new QPushButton("Define Vertical Axis", vGroup);
    vLayout->addWidget(defineVerticalButton);
    connect(defineVerticalButton, &QPushButton::clicked, this, &ChartToEFM::onDefineVerticalClicked);
    verticalAxisNameLineEdit = new QLineEdit(vGroup);
    verticalAxisNameLineEdit->setPlaceholderText("Axis Name");
    vLayout->addWidget(verticalAxisNameLineEdit);
    verticalMinLineEdit = new QLineEdit(vGroup);
    verticalMinLineEdit->setPlaceholderText("Min Value");
    vLayout->addWidget(verticalMinLineEdit);
    verticalMaxLineEdit = new QLineEdit(vGroup);
    verticalMaxLineEdit->setPlaceholderText("Max Value");
    vLayout->addWidget(verticalMaxLineEdit);
    addVerticalCalibButton = new QPushButton("Add Calibration", vGroup);
    vLayout->addWidget(addVerticalCalibButton);
    connect(addVerticalCalibButton, &QPushButton::clicked, this, &ChartToEFM::onAddVerticalCalibrationClicked);
    leftLayout->addWidget(vGroup);

    // --- Lines Group ---
    QGroupBox* linesGroup = new QGroupBox("Lines", leftPanel);
    QVBoxLayout* linesGroupLayout = new QVBoxLayout(linesGroup);
    // A container widget to host the dynamic line editors.
    lineEditorsContainer = new QWidget();
    lineEditorsLayout = new QVBoxLayout(lineEditorsContainer);
    lineEditorsLayout->setContentsMargins(0, 0, 0, 0);
    lineEditorsLayout->setSpacing(5);
    linesGroupLayout->addWidget(lineEditorsContainer);
    leftLayout->addWidget(linesGroup);

    // New Line Button.
    newLineButton = new QPushButton("New Line", leftPanel);
    leftLayout->addWidget(newLineButton);
    connect(newLineButton, &QPushButton::clicked, this, &ChartToEFM::onNewLineButtonClicked);

    leftLayout->addStretch();

    // --- Right Panel ---
    QWidget* rightPanel = new QWidget(centralWidget);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // Top bar: Rotate and Undo buttons.
    QWidget* topBar = new QWidget(rightPanel);
    QHBoxLayout* topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);
    topBarLayout->addStretch();
    rotateButton = new QPushButton("Rotate", topBar);
    topBarLayout->addWidget(rotateButton);
    connect(rotateButton, &QPushButton::clicked, this, &ChartToEFM::onRotateButtonClicked);
    undoButton = new QPushButton("Undo", topBar);
    topBarLayout->addWidget(undoButton);
    connect(undoButton, &QPushButton::clicked, this, &ChartToEFM::onUndoButtonClicked);
    rightLayout->addWidget(topBar);

    imageView = new CustomGraphicsView(rightPanel);
    scene = new QGraphicsScene(this);
    imageView->setScene(scene);
    rightLayout->addWidget(imageView);
    imageView->viewport()->installEventFilter(this);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    mainLayout->addWidget(splitter);

    setWindowTitle("Chart Digitizer");
}

// Slot: New Line Button clicked.
void ChartToEFM::onNewLineButtonClicked()
{
    bool ok = false;
    QString lineName = QInputDialog::getText(this, "New Line", "Enter line name:", QLineEdit::Normal, "", &ok);
    if (ok && !lineName.isEmpty()) {
        LineEditor* editor = new LineEditor(lineName, this);
        lineEditorsLayout->addWidget(editor);
        // Connect the editor's "Define Points" button so that when clicked, it sets the current line editor.
        // For example, assume LineEditor emits a signal when its define points button is clicked.
        // Here we set currentLineEditor to that editor.
        connect(editor, &LineEditor::lineUpdated, [this, editor]() {
            currentLineEditor = editor; // For simplicity, set currentLineEditor when updated.
            });
    }
}

// Slot: Undo last drawn item.
void ChartToEFM::onUndoButtonClicked()
{
    if (!undoStack.isEmpty()) {
        QGraphicsItem* item = undoStack.last();
        undoStack.pop_back();
        scene->removeItem(item);
        delete item;
    }
}

// Slot: Upload Chart.
void ChartToEFM::onUploadChartClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select Chart Image", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (!fileName.isEmpty()) {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            scene->clear();
            undoStack.clear();
            axisManager->clearHorizontalAxis(scene);
            axisManager->clearVerticalAxis(scene);
            scene->addPixmap(pixmap);
            scene->setSceneRect(pixmap.rect());
            imageView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }
    }
}

// Slot: Rotate the image.
void ChartToEFM::onRotateButtonClicked()
{
    imageView->rotate(90);
}

void ChartToEFM::onDefineHorizontalClicked()
{
    axisMode = DefiningHorizontal;
    waitingForSecondClick = false;
}

void ChartToEFM::onDefineVerticalClicked()
{
    axisMode = DefiningVertical;
    waitingForSecondClick = false;
}

void ChartToEFM::onAddHorizontalCalibrationClicked()
{
    axisMode = CalibratingHorizontal;
}

void ChartToEFM::onAddVerticalCalibrationClicked()
{
    axisMode = CalibratingVertical;
}

// Event filter: handles mouse clicks on the image view.
bool ChartToEFM::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == imageView->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                QPointF scenePos = imageView->mapToScene(mouseEvent->pos());
                // Handle axis definitions.
                if (axisMode == DefiningHorizontal || axisMode == DefiningVertical) {
                    if (!waitingForSecondClick) {
                        firstAxisPoint = scenePos;
                        waitingForSecondClick = true;
                    }
                    else {
                        QPen pen;
                        QGraphicsLineItem* line = nullptr;
                        if (axisMode == DefiningHorizontal) {
                            pen.setColor(Qt::red);
                            pen.setWidth(2);
                            line = scene->addLine(QLineF(firstAxisPoint, scenePos), pen);
                            line->setData(0, "axisHorizontal");
                            axisManager->defineHorizontalAxis(firstAxisPoint, scenePos, scene);
                        }
                        else if (axisMode == DefiningVertical) {
                            pen.setColor(Qt::blue);
                            pen.setWidth(2);
                            line = scene->addLine(QLineF(firstAxisPoint, scenePos), pen);
                            line->setData(0, "axisVertical");
                            axisManager->defineVerticalAxis(firstAxisPoint, scenePos, scene);
                        }
                        if (line)
                            undoStack.push_back(line);
                        waitingForSecondClick = false;
                        axisMode = NoAxis;
                    }
                    return true;
                }
                // Handle calibration - horizontal.
                else if (axisMode == CalibratingHorizontal) {
                    if (axisManager->horizontalAxis()) {
                        QLineF hLine = axisManager->horizontalAxis()->line();
                        QPointF A = hLine.p1();
                        QPointF B = hLine.p2();
                        QPointF AB = B - A;
                        double normSq = AB.x() * AB.x() + AB.y() * AB.y();
                        if (normSq != 0) {
                            QPointF AP = scenePos - A;
                            double t = (AP.x() * AB.x() + AP.y() * AB.y()) / normSq;
                            scenePos = A + t * AB;
                        }
                    }
                    bool ok = false;
                    double value = QInputDialog::getDouble(this, "Horizontal Calibration",
                        "Enter calibration value for this point:", 0.0, -1e9, 1e9, 6, &ok);
                    if (ok) {
                        axisManager->addHorizontalCalibration(scenePos, value);
                        QPen markerPen(Qt::red);
                        QBrush markerBrush(Qt::red);
                        qreal markerSize = 4.0;
                        auto ellipse = scene->addEllipse(scenePos.x() - markerSize / 2, scenePos.y() - markerSize / 2,
                            markerSize, markerSize, markerPen, markerBrush);
                        ellipse->setData(0, "calibrationHorizontal");
                        undoStack.push_back(ellipse);
                        QString formattedText = formatCalibratedValue(value);
                        QGraphicsTextItem* textItem = scene->addText(formattedText);
                        textItem->setDefaultTextColor(Qt::red);
                        textItem->setPos(scenePos.x() + markerSize, scenePos.y());
                        textItem->setData(0, "calibrationHorizontalLabel");
                        undoStack.push_back(textItem);
                    }
                    axisMode = NoAxis;
                    return true;
                }
                // Handle calibration - vertical.
                else if (axisMode == CalibratingVertical) {
                    if (axisManager->verticalAxis()) {
                        QLineF vLine = axisManager->verticalAxis()->line();
                        QPointF A = vLine.p1();
                        QPointF B = vLine.p2();
                        QPointF AB = B - A;
                        double normSq = AB.x() * AB.x() + AB.y() * AB.y();
                        if (normSq != 0) {
                            QPointF AP = scenePos - A;
                            double t = (AP.x() * AB.x() + AP.y() * AB.y()) / normSq;
                            scenePos = A + t * AB;
                        }
                    }
                    bool ok = false;
                    double value = QInputDialog::getDouble(this, "Vertical Calibration",
                        "Enter calibration value for this point:", 0.0, -1e9, 1e9, 6, &ok);
                    if (ok) {
                        axisManager->addVerticalCalibration(scenePos, value);
                        QPen markerPen(Qt::blue);
                        QBrush markerBrush(Qt::blue);
                        qreal markerSize = 4.0;
                        auto ellipse = scene->addEllipse(scenePos.x() - markerSize / 2, scenePos.y() - markerSize / 2,
                            markerSize, markerSize, markerPen, markerBrush);
                        ellipse->setData(0, "calibrationVertical");
                        undoStack.push_back(ellipse);
                        QString formattedText = formatCalibratedValue(value);
                        QGraphicsTextItem* textItem = scene->addText(formattedText);
                        textItem->setDefaultTextColor(Qt::blue);
                        textItem->setPos(scenePos.x() + markerSize, scenePos.y());
                        textItem->setData(0, "calibrationVerticalLabel");
                        undoStack.push_back(textItem);
                    }
                    axisMode = NoAxis;
                    return true;
                }
                // Handle line point definition for the current line editor.
                if (currentLineEditor != nullptr) {
                    currentLineEditor->addPoint(scenePos);
                    QPen markerPen(currentLineEditor->lineColor());
                    QBrush markerBrush(currentLineEditor->lineColor());
                    qreal markerSize = 4.0;
                    auto ellipse = scene->addEllipse(scenePos.x() - markerSize / 2, scenePos.y() - markerSize / 2,
                        markerSize, markerSize, markerPen, markerBrush);
                    undoStack.push_back(ellipse);
                    return true;
                }
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
