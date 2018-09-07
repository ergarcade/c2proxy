#ifndef WINDOW_H
#define WINDOW_H

/*
 * Majority from Qt Systray example.
 * http://doc.qt.io/qt-5/qtwidgets-desktop-systray-example.html
 */

#include "pm.h"

#include <QAction>
#include <QDialog>
#include <QHostAddress>
#include <QMenu>
#include <QSystemTrayIcon>

class Window: public QDialog {
    Q_OBJECT

    public:
        Window(PM *pm);

    private slots:
        void launchBrowser(void);

    public slots:
        void monitor_connected(const QString &serial_number);
        void monitor_disconnected(const QString &serial_number);

    private:
        void createActions(void);
        void createTrayIcon(void);
        void populateInterfaceMenu(void);
        void populateMonitorsMenu(void);

        QAction *launchBrowserAction;
        QAction *quitAction;

        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;
        QMenu *trayInterfaceMenu;
        QMenu *trayMonitorsMenu;

        PM *pm;
};

#endif // WINDOW_H
