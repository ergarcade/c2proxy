#ifndef PMS_H
#define PMS_H

#include <QFile>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QWebSocketServer>

#include "hidapi.h"

/*
 * Concept2 USB vendor ID.
 */
#define C2_HID_VENDOR_ID 0x17a4

/*
 * Acts as the manager for all Performance Monitors.
 *
 * - Watches USB for new monitors / monitors that have disconnected.
 *   Starts / stops threads that contain the PM object as required.
 * - Passes PM requests along to monitors.
 */
class PMs: public QObject {
    Q_OBJECT

    private:
        /*
         * Timer for querying new / disconnected monitors.
         */
        QTimer *timer_rescan_enumerate;

        /*
         * Socket for publishing data and list of connections.
         */
        QWebSocketServer *ws;
        QList<QWebSocket*>ws_clients;

        /*
         * Kick off a new PM.
         */
        void start_PM(struct hid_device_info *d);

        /*
         * Output file for debugging.
         */
        QFile debug_output;

    public:
        /*
         * Constructor.
         */
        PMs(QObject *parent = 0);

        /*
         * Destructor.
         */
        ~PMs(void);

        /*
         * Return address proxy is attached to.
         */
        QList<QHostAddress> server_address();

        /*
         * Return port number proxy is listening on.
         */
        quint16 server_port();

    private slots:
        void rescan(void);
        void ws_on_new_connection(void);
        void ws_on_disconnection(void);
        void ws_on_ssl_errors(const QList<QSslError> &errors);

    signals:
        /*
         * Tell PM thread that we are going to finish up.
         */
        void notify_PM_finish(const QString &str);

        /*
         * Notify PM thread of a new workout.
         */
        void notify_PM_workout(const QString &str);

        /*
         * Notify PM to reset back to offline state.
         */
        void notify_PM_reset(void);
};

#endif // PMS_H
