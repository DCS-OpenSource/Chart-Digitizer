#include "ChartToEFM.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLineEdit>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QFileDialog>
#include <QPixmap>

ChartToEFM::ChartToEFM(QWidget* parent) : QMainWindow(parent)
{
    setupUI();
}

void ChartToEFM::setupUI()
{
    // Create a central widget for the main window.
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create a horizontal layout to hold the left and right panels.
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // ================================
    // Left Panel: Controls
    // ================================
    QWidget* leftPanel = new QWidget(centralWidget);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // Upload Chart Button
    uploadChartButton = new QPushButton("Upload Chart", leftPanel);
    leftLayout->addWidget(uploadChartButton);
    connect(uploadChartButton, &QPushButton::clicked, this, &ChartToEFM::onUploadChartClicked);

    // Chart Name field
    chartNameLineEdit = new QLineEdit(leftPanel);
    chartNameLineEdit->setPlaceholderText("Chart Name");
    leftLayout->addWidget(chartNameLineEdit);

    // Define Horizontal Axis Button
    defineHorizontalButton = new QPushButton("Define Horizontal Axis", leftPanel);
    leftLayout->addWidget(defineHorizontalButton);

    // Horizontal Axis Name field
    horizontalAxisNameLineEdit = new QLineEdit(leftPanel);
    horizontalAxisNameLineEdit->setPlaceholderText("Horizontal Axis Name");
    leftLayout->addWidget(horizontalAxisNameLineEdit);

    // Define Vertical Axis Button
    defineVerticalButton = new QPushButton("Define Vertical Axis", leftPanel);
    leftLayout->addWidget(defineVerticalButton);

    // Vertical Axis Name field
    verticalAxisNameLineEdit = new QLineEdit(leftPanel);
    verticalAxisNameLineEdit->setPlaceholderText("Vertical Axis Name");
    leftLayout->addWidget(verticalAxisNameLineEdit);

    // Output Data Button
    outputDataButton = new QPushButton("Output Data", leftPanel);
    leftLayout->addWidget(outputDataButton);

    // Add a stretch to push the widgets to the top.
    leftLayout->addStretch();

    // ================================
    // Right Panel: Image Display Area
    // ================================
    imageView = new QGraphicsView(centralWidget);
    scene = new QGraphicsScene(this);
    imageView->setScene(scene);

    // ================================
    // Combine Panels Using a Splitter
    // ================================
    QSplitter* splitter = new QSplitter(Qt::Horizontal, centralWidget);
    splitter->addWidget(leftPanel);
    splitter->addWidget(imageView);

    // Adjust the stretch factors so that the left panel stays narrow, and the right panel expands.
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    // Add the splitter to the main layout.
    mainLayout->addWidget(splitter);

    // Set the window title.
    setWindowTitle("Chart Digitizer");
}

void ChartToEFM::onUploadChartClicked()
{
    // Open a file dialog to select an image.
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Chart Image",
        "",
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)"
    );

    if (!fileName.isEmpty())
    {
        QPixmap pixmap(fileName);
        if (!pixmap.isNull())
        {
            // Clear any previous image or drawings.
            scene->clear();

            // Add the pixmap to the scene.
            scene->addPixmap(pixmap);

            // Adjust the scene's size to match the image.
            scene->setSceneRect(pixmap.rect());

            // Optionally, resize the view to better fit the image.
            imageView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }
    }
}
