#include "ChartToEFM.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    ChartToEFM window;
    window.resize(1500, 1000);  // Set an initial size
    window.show();
    return app.exec();
}
