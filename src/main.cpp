#include "dialog.h"
#include <wiringPi.h>
#include <QApplication>

int main(int argc, char *argv[])
{

    if (wiringPiSetup() == -1)
    {
            printf("WiringPi setup failed!\n");
            return 1;
    }

    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}
