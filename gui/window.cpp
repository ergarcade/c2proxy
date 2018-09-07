#include "window.h"

#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

Window::Window(PM *pm) {
    this->pm = pm;

    createActions();
    createTrayIcon();

    trayIcon->show();

    connect(this->pm, SIGNAL(monitor_connected(const QString&)),
            this, SLOT(monitor_connected(const QString&)));
    connect(this->pm, SIGNAL(monitor_disconnected(const QString&)),
            this, SLOT(monitor_disconnected(const QString&)));
}

void Window::createActions(void) {
    launchBrowserAction = new QAction(tr("&Launch Browser"), this);
    connect(launchBrowserAction, &QAction::triggered,
            this, &Window::launchBrowser);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, 
            qApp, &QApplication::quit);
}

void Window::createTrayIcon(void) {
    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction(launchBrowserAction);
    trayIconMenu->addSeparator();

    populateMonitorsMenu();
    trayIconMenu->addSeparator();
    populateInterfaceMenu();

    trayIconMenu->addAction(QString("Port: %1").arg(pm->server_port()))->setEnabled(false);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(QIcon(":images/ergarcade-logo.png"));
    trayIcon->setContextMenu(trayIconMenu);
}

void Window::populateInterfaceMenu(void) {
    QList<QHostAddress> l;

    trayInterfaceMenu = trayIconMenu->addMenu("Interfaces");

    l = pm->server_address();
    for (int i = 0; i < l.size(); i++) {
        if (l[i].protocol() == QAbstractSocket::IPv4Protocol &&
                l[i].toString() != QString("127.0.0.1")) {
            trayInterfaceMenu->addAction(QString(l[i].toString()))->setEnabled(false);
        }
    }
}

void Window::populateMonitorsMenu(void) {
    trayMonitorsMenu = trayIconMenu->addMenu(tr("No monitors"));
}

void Window::launchBrowser(void) {
    QString url = QString("http://www.ergarcade.com?c2proxy_port=%1").arg(pm->server_port());

    if (QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode)) == false) {
        QMessageBox m;
        m.setText(tr("Failed to launch browser"));
        m.exec();
    }
}

/*
 * public slots
 */
void Window::monitor_connected(const QString &serial_number) {
    trayMonitorsMenu->setEnabled(true);
    trayMonitorsMenu->addAction(serial_number)->setEnabled(false);
    trayMonitorsMenu->setTitle(tr("Monitors"));
}
void Window::monitor_disconnected(const QString &serial_number) {
    int i;
    QList<QAction*> actions(trayMonitorsMenu->actions());

    for (i = 0; i < actions.size(); i++) {
        if (actions[i]->text() == serial_number) {
            break;
        }
    }

    if (i != actions.size()) {
        trayMonitorsMenu->removeAction(actions[i]);

        if (trayMonitorsMenu->isEmpty()) {
            trayMonitorsMenu->setTitle(tr("No monitors"));
        }
    }
}
