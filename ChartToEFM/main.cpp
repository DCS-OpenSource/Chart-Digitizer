#include "ChartToEFM.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ChartToEFM window;
    window.resize(800, 600);  // Set an initial size
    window.show();
    return app.exec();
}
