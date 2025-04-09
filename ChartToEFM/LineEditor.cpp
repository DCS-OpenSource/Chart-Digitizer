#include "LineEditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QColorDialog>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>

// Constructor: initialize with the given line name and a default color.
LineEditor::LineEditor(const QString& lineName, QWidget* parent)
    : QWidget(parent),
    currentColor(Qt::red)  // default color; adjust as needed
{
    setupUI(lineName);
}

// Sets up the UI elements for the line editor.
void LineEditor::setupUI(const QString& lineName)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // Line name edit.
    nameLineEdit = new QLineEdit(lineName, this);
    mainLayout->addWidget(nameLineEdit);

    // A horizontal layout for the buttons.
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    // Button to choose line color.
    chooseColorButton = new QPushButton("Choose Color", this);
    buttonLayout->addWidget(chooseColorButton);
    connect(chooseColorButton, &QPushButton::clicked, this, &LineEditor::onChooseColor);

    // Button to activate point definition mode.
    definePointsButton = new QPushButton("Define Points", this);
    buttonLayout->addWidget(definePointsButton);
    // When clicked, we emit the lineUpdated() signal so that the main window can mark this as the active line.
    connect(definePointsButton, &QPushButton::clicked, this, &LineEditor::lineUpdated);

    // "Remove Line" button—renamed from "Clear Points".
    clearPointsButton = new QPushButton("Remove Line", this);
    buttonLayout->addWidget(clearPointsButton);
    // When clicked, this widget will signal removal.
    connect(clearPointsButton, &QPushButton::clicked, this, &LineEditor::onClearPoints);

    mainLayout->addLayout(buttonLayout);

    // Label that shows the count of defined points.
    pointsDisplay = new QLabel("Points: 0", this);
    mainLayout->addWidget(pointsDisplay);

    setLayout(mainLayout);
}

// Returns the current line colour.
QColor LineEditor::lineColor() const
{
    return currentColor;
}

// Adds a point (e.g. when a click is processed by the main window)
// and updates the points count display.
void LineEditor::addPoint(const QPointF& pt)
{
    pointsList.append(pt);
    pointsDisplay->setText(QString("Points: %1").arg(pointsList.size()));
    emit lineUpdated();
}

// Returns the current set of points.
const QVector<QPointF>& LineEditor::points() const
{
    return pointsList;
}

// Registers a drawn QGraphicsItem (such as a marker or label) as belonging to this line.
// This will allow updating their color later.
void LineEditor::registerDrawingItem(QGraphicsItem* item)
{
    drawingItems.append(item);
}

// Iterates through all registered drawn items and updates their colors.
void LineEditor::updateDrawingItemsColor()
{
    for (auto item : drawingItems) {
        // If the item is an ellipse (marker), update its pen and brush.
        if (auto ellipse = dynamic_cast<QGraphicsEllipseItem*>(item)) {
            QPen pen = ellipse->pen();
            pen.setColor(currentColor);
            ellipse->setPen(pen);
            QBrush brush = ellipse->brush();
            brush.setColor(currentColor);
            ellipse->setBrush(brush);
        }
        // If the item is a text item (e.g. a label), update its text color.
        if (auto textItem = dynamic_cast<QGraphicsTextItem*>(item)) {
            textItem->setDefaultTextColor(currentColor);
        }
        // For other item types, add additional updates as needed.
    }
    // Optionally, you could also force a refresh or update on the scene.
}

// Slot: Open a colour chooser; when the user picks a color, update the current color,
// update the colour of any drawn items, and emit a signal that the line has been updated.
void LineEditor::onChooseColor()
{
    QColor chosenColor = QColorDialog::getColor(currentColor, this, "Select Line Color");
    if (chosenColor.isValid()) {
        currentColor = chosenColor;
        // Update the appearance of the choose colour button.
        QString style = QString("background-color: %1; color: white; border: none; border-radius: 4px; padding: 6px 12px;")
            .arg(currentColor.name());
        chooseColorButton->setStyleSheet(style);
        // Update the colors of all already drawn items.
        updateDrawingItemsColor();
        emit lineUpdated();
    }
}

// Slot: Remove the line. This emits a removeRequested() signal so that the main window
// can remove this line editor from the UI and also remove any associated drawn items from the scene.
void LineEditor::onClearPoints()
{
    emit removeRequested(this);
}
