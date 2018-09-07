#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QThread>

#ifndef QT_NO_SYSTEMTRAYICON

#include "window.h"

PM* start_monitor(void) {
    PM *pm = new PM;
    QThread *t = new QThread;

    pm->moveToThread(t);
    t->start(QThread::HighestPriority);

    return pm;
}

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(c2proxy);

    PM *pm;

    QApplication a(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    pm = start_monitor();
    Window window(pm);

    return a.exec();
}

#else   // QT_NO_SYSTEMTRAYICON

#endif  // QT_NO_SYSTEMTRAYICON
