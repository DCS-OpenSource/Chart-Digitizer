#ifndef CHARTTOEFM_H
#define CHARTTOEFM_H

#include <QMainWindow>

class QGraphicsView;
class QGraphicsScene;
class QPushButton;
class QLineEdit;

class ChartToEFM : public QMainWindow
{
    Q_OBJECT
public:
    explicit ChartToEFM(QWidget* parent = nullptr);

private slots:
    void onUploadChartClicked();

private:
    void setupUI();

    // Left panel UI components
    QPushButton* uploadChartButton;
    QLineEdit* chartNameLineEdit;
    QPushButton* defineHorizontalButton;
    QLineEdit* horizontalAxisNameLineEdit;
    QPushButton* defineVerticalButton;
    QLineEdit* verticalAxisNameLineEdit;
    QPushButton* outputDataButton;

    // Right panel: image display area
    QGraphicsView* imageView;
    QGraphicsScene* scene;
};

#endif // CHARTTOEFM_H
