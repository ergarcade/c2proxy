#include <QCoreApplication>

#include "pm.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    PM *pm = new PM;
    Q_UNUSED(pm);

    return a.exec();
}
