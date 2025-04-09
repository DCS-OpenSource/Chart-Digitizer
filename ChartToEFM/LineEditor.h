#ifndef LINEEDITOR_H
#define LINEEDITOR_H

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPointF>

class QGraphicsItem; // Forward declaration
class QLabel;
class QPushButton;
class QLineEdit;

class LineEditor : public QWidget
{
    Q_OBJECT
public:
    // Construct a new LineEditor with an initial line name.
    explicit LineEditor(const QString& lineName, QWidget* parent = nullptr);

    // Returns the currently selected line colour.
    QColor lineColor() const;

    // Add a new point (in scene coordinates) to this line.
    void addPoint(const QPointF& pt);

    // Retrieve all points stored in this editor.
    const QVector<QPointF>& points() const;

    // Register a drawing item (marker, label, etc.) that belongs to this line.
    void registerDrawingItem(QGraphicsItem* item);

    // Update the colour of all registered drawing items.
    void updateDrawingItemsColor();

signals:
    // Emitted when the line is updated (e.g., a point is added or its properties change).
    void lineUpdated();
    // Emitted when the user requests that this line be removed.
    void removeRequested(LineEditor* editor);

private slots:
    // Slot triggered when the user clicks the colour chooser button.
    void onChooseColor();
    // Slot triggered when the "Remove Line" button is clicked.
    void onClearPoints();

private:
    // Set up the user interface.
    void setupUI(const QString& lineName);

    // UI elements for editing line properties.
    QLineEdit* nameLineEdit;         // To display and edit the line's name.
    QPushButton* chooseColorButton;    // Opens a colour picker.
    QPushButton* definePointsButton;   // Activates "define points" mode (emit signal).
    QPushButton* clearPointsButton;    // Now functions as "Remove Line".
    QLabel* pointsDisplay;        // Displays a summary (such as a count) of defined points.

    // The current colour for this line.
    QColor currentColor;

    // Container for all points defined for this line.
    QVector<QPointF> pointsList;

    // Container for pointers to drawn QGraphicsItems belonging to this line.
    QVector<QGraphicsItem*> drawingItems;
};

#endif // LINEEDITOR_H
