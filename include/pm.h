#ifndef PM_H
#define PM_H

#ifdef QT_DEBUG
#endif /* QT_DEBUG */
#include <QFile>
#include <QJsonObject>
#include <QList>
#include <QTimer>
#include <QWebSocketServer>

#include "hidapi.h"
#include "csafe.h"

/*
 * Periodic timer; rescan USB, publish activity over websocket etc.
 */
#define TIMEOUT_RESCAN_HID    1000

/*
 * Period for asking PMs for data.
 */
#define TIMEOUT_POLL_DEVICES        100
#define POLL_COUNT_QUERY_STROKE     1       /* query stroke data every TIMEOUT_POLL_DEVICES */
#define POLL_COUNT_QUERY_WORKOUT    5       /* query workout data every 5 TIMEOUT_POLL_DEVICES */

/*
 * Length of Concept2 PM serial number.
 */
#define C2_SERIAL_NUM_LENGTH 9

/*
 * Concept2 USB vendor ID.
 */
#define C2_HID_VENDOR_ID 0x17a4

class Monitor {
    public:
        Monitor(void) {
            dev = NULL;
            pending_responses = 0;
            query_count = 0;

            csafe_vars_resp_init(&resp);
        }

        void update_json(void);

        /*
         * Handle to USB device.
         */
        hid_device *dev;

        /*
         * Number of responses to wait for. Maybe multiple, as we request
         * forceplot data faster than workout data.
         */
        int pending_responses;

        /*
         * State of our PM.
         */
        csafe_vars_resp_t resp;

        /*
         * We send this across to client.
         */
        QJsonObject json;

        /*
         * Count queries for POLL_COUNT_*. Ok if it wraps.
         */
        unsigned int query_count;
};

/*
 * Encapsulates our interfaces.
 */
class PM: public QObject {
    Q_OBJECT

    private:
        /*
         * List of handles to each attached performance monitor.
         */
        QMap<QString, Monitor*> monitors;

        /*
         * Our websocket end point.
         */
        QWebSocketServer *ws;

        /*
         * List of attached websocket clients.
         */
        QList<QWebSocket*> ws_clients;

        /*
         * Timer for querying new monitors.
         */
        QTimer *timer_rescan_hid;
        QTimer *timer_poll_devices;

        /*
         * Debug.
         */
#ifdef QT_DEBUG
        QFile debug_output;
        void dump_strokestate(const QString &serial_number, const csafe_vars_resp_t *resp);
#endif /* QT_DEBUG */

        int hid_tx(hid_device *handle, unsigned char *buffer, unsigned int len);
        int hid_rx(hid_device *handle, unsigned char *buffer, unsigned int len);

#ifndef BATCH_JSON
        void notify_clients(const QString &serial_number, const QJsonObject &json);
#else // BATCH_JSON
        void notify_clients(const QJsonObject &json);
#endif // BATCH_JSON
        void update_json(QJsonObject &json, const csafe_vars_resp_t *resp);

    private slots:
        void rescan_hid(void);
        void poll_devices(void);
        void ws_on_new_connection(void);
        void ws_on_disconnection(void);

    public:
        PM(void);
        ~PM(void);
        QList<QHostAddress> server_address();
        quint16 server_port();

    signals:
        void monitor_connected(const QString &serial_number);
        void monitor_disconnected(const QString &serial_number);
};

#endif // PM_H
