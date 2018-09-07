/*
 * TODO
 */

#ifdef QT_DEBUG
#include <QDebug>
#endif // QT_DEBUG
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkInterface>
#include <QTextStream>
#include <QThread>
#include <QWebSocket>

#include "csafe.h"
#include "pm.h"
#include "util.h"

/* pm_query_reset */
/*
 * cmd_reset_req
 */
unsigned char pm_query_reset[4] = {
    0xf1, 0x81, 0x81, 0xf2
};
/* pm_query_machine */
/*
 * cmd_getversion_req
 * cmd_getid_req
 * cmd_getserial_req
 * cmd_getodometer_req
 * cmd_geterrorcode_req
 * cmd_getuserinfo_req
 * cmd_getcaps_req
 */
unsigned char pm_query_machine[12] = {
    0xf1, 0x91, 0x92, 0x94, 0x9b, 0x9c, 0xab, 0x70,
    0x01, 0x00, 0x4a, 0xf2
};
/* pm_query_workout */
/*
 * cmd_getstatus_req
 * cmd_gettwork_req
 * cmd_gethorizontal_req
 * cmd_getpace_req
 * cmd_getpower_req
 * cmd_getcalories_req
 * cmd_getcadence_req
 * cmd_gethrcur_req
 * cmd_setusercfg1_req
 * cmd_pm_get_worktime_req
 * cmd_setusercfg1_req
 * cmd_pm_get_workdistance_req
 * cmd_setusercfg1_req
 * cmd_pm_get_dragfactor_req
 * cmd_setusercfg1_req
 * cmd_pm_get_workouttype_req
 * cmd_setusercfg1_req
 * cmd_pm_get_workoutstate_req
 */
unsigned char pm_query_workout[26] = {
    0xf1, 0x80, 0xa0, 0xa1, 0xa6, 0xb4, 0xa3, 0xa7,
    0xb0, 0x1a, 0x01, 0xa0, 0x1a, 0x01, 0xa3, 0x1a,
    0x01, 0xc1, 0x1a, 0x01, 0x89, 0x1a, 0x01, 0x8d,
    0xfa, 0xf2
};
/* pm_query_per_stroke */
/*
 * cmd_getpower_req
 * cmd_setusercfg1_req
 * cmd_pm_get_dragfactor_req
 * cmd_setusercfg1_req
 * cmd_pm_get_strokestate_req
 * cmd_setusercfg1_req
 * cmd_pm_get_forceplotdata_req
 */
unsigned char pm_query_per_stroke[15] = {
    0xf1, 0xb4, 0x1a, 0x01, 0xc1, 0x1a, 0x01, 0xbf,
    0x1a, 0x03, 0x6b, 0x01, 0x20, 0x99, 0xf2
};

PM::PM(void) {
    QThread::currentThread()->setPriority(QThread::HighestPriority);

    /*
     * Our websocket server.
     */
    ws = new QWebSocketServer(
            QStringLiteral("c2proxy-server"),
            QWebSocketServer::NonSecureMode,
            this);

    if (ws->listen(QHostAddress::Any)) {
        qDebug() << "Websocket listening on port" << ws->serverPort();
        fprintf(stderr, "%d\n", ws->serverPort());
        connect(ws, SIGNAL(newConnection()), this, SLOT(ws_on_new_connection()));
    }

    /*
     * Setup HID scanner.
     */
    timer_rescan_hid = new QTimer(this);
    timer_poll_devices = new QTimer(this);

    connect(timer_rescan_hid, SIGNAL(timeout()), this, SLOT(rescan_hid()));
    connect(timer_poll_devices, SIGNAL(timeout()), this, SLOT(poll_devices()));

    timer_rescan_hid->start(TIMEOUT_RESCAN_HID);

#ifdef QT_DEBUG
    QString output = QString::asprintf("%s-%lld.json", "debug",
            QDateTime::currentSecsSinceEpoch());

    debug_output.setFileName(output);
    if (!debug_output.open(QIODevice::WriteOnly)) {
        qDebug() << "failed to open" << output << "for debug log";
    } else {
        qDebug() << "writing json log to" << output;
        debug_output.flush();
    }
#endif // QT_DEBUG
}

PM::~PM(void) {
#ifdef QT_DEBUG
    qDebug() << "PM::~PM()";
#endif // QT_DEBUG

    timer_rescan_hid->stop();
    timer_poll_devices->stop();

    delete ws;

    /*
     * XXX iterate monitors and release device handles
     */
}

/*
 * private slot
 */
void PM::rescan_hid(void) {
    struct hid_device_info *devs = NULL;
    struct hid_device_info *d = NULL;
    Monitor *m;
    QString serial_number;
    QList<QWebSocket*>::iterator i;

    /*
     * List all C2 connected devices.
     */
    devs = hid_enumerate(C2_HID_VENDOR_ID, 0);

    /*
     * Add new monitors.
     */
    for (d = devs; d; d = d->next) {
        serial_number = QString::fromWCharArray(d->serial_number, C2_SERIAL_NUM_LENGTH);

        if (!monitors.contains(serial_number)) {
            qDebug() << "found new monitor" << serial_number;

            m = new Monitor;
            if (!m) {
#ifdef QT_DEBUG
                qDebug() << "Couldn't alloc memory" << serial_number;
#endif // QT_DEBUG
                return;
            }

            m->dev = hid_open(
                    d->vendor_id,
                    d->product_id,
                    d->serial_number);
            if (!m->dev) {
#ifdef QT_DEBUG
                qDebug() << "Failed to open device" << serial_number;
#endif // QT_DEBUG
                delete m;
                return;
            }

            hid_set_nonblocking(m->dev, 1);
            monitors.insert(serial_number, m);

            /*
             * Setup some initial state.
             */
            hid_tx(m->dev, pm_query_reset, sizeof(pm_query_reset));
            m->pending_responses++;
            hid_tx(m->dev, pm_query_machine, sizeof(pm_query_machine));
            m->pending_responses++;

            emit monitor_connected(serial_number);
        }
    }


    hid_free_enumeration(devs);

    /*
     * Start polling if we have devices and aren't already running.
     */
    if (monitors.isEmpty()) {
        if (timer_poll_devices->isActive()) {
            timer_poll_devices->stop();
        }
    } else if (!timer_poll_devices->isActive()) {
        timer_poll_devices->start(TIMEOUT_POLL_DEVICES);
    }
}

/*
 * private slot
 */
void PM::poll_devices(void) {
    QMutableMapIterator<QString, Monitor*> i(monitors);
    Monitor *m = NULL;
    int len;
    unsigned char buffer[256];
    int err;
    csafe_t c;
    QString serial_number;

#ifdef BATCH_JSON
    QJsonObject batch;
#endif // BATCH_JSON

    /*
     * Send request.
     */
    for (i.toFront(); i.hasNext(); ) {
        i.next();
        m = i.value();
        serial_number = i.key();

        if (m->pending_responses) {  /* don't send new requests if waiting for response */
            continue;
        }

        m->query_count++;

        if (m->query_count % POLL_COUNT_QUERY_STROKE == 0) {
            hid_tx(m->dev, pm_query_per_stroke, sizeof(pm_query_per_stroke));
            m->pending_responses++;
        }

        if (m->query_count % POLL_COUNT_QUERY_WORKOUT == 0) {
            hid_tx(m->dev, pm_query_workout, sizeof(pm_query_workout));
            m->pending_responses++;
        }
    }

    /*
     * Handle responses.
     */
    for (i.toFront(); i.hasNext(); ) {
        i.next();
        m = i.value();

        do {
            len = hid_rx(m->dev, buffer, sizeof(buffer));

            if (len < 0) {
#ifdef QT_DEBUG
                qDebug() << serial_number << ": lost connection to monitor";
#endif // QT_DEBUG
                delete m;
                i.remove();

                emit monitor_disconnected(serial_number);
            } else if (len > 0) {
                m->pending_responses > 0 && m->pending_responses--;

                csafe_init(&c);
                err = csafe_set_frame(&c, buffer, len);

                if (err != 0) {
#ifdef QT_DEBUG
                    qDebug() << serial_number << ": failed to unpack" << err;
#endif // QT_DEBUG
                    return;
                }

                csafe_unpack_resp(&c, &m->resp);
                update_json(m->json, &m->resp);

#ifdef QT_DEBUG
                // dump_strokestate(serial_number, &m->resp);
#endif // QT_DEBUG

#ifndef BATCH_JSON
                notify_clients(serial_number, m->json);
#else // BATCH_JSON
                batch[serial_number] = m->json;
#endif // BATCH_JSON
            }
        } while (len > 0);
    }

#ifdef BATCH_JSON
    notify_clients(batch);
#endif // BATCH_JSON
}

#ifdef QT_DEBUG
void PM::dump_strokestate(const QString &serial_number, const csafe_vars_resp_t *resp) {
    const char *state_names[] = {
        /*  0 */ "waiting_for_wheel_to_reach_min_speed_state",
        /*  1 */ "waiting_for_wheel_to_accelerate_state",
        /*  2 */ "driving_state",
        /*  3 */ "dwelling_after_drive_state",
        /*  4 */ "recovery_state"
    };

    qDebug() << serial_number << ": strokestate" << state_names[resp->pm_get_stroke_state] <<
        resp->pm_get_forceplotdata_len;
#if 0
    util::hexdump((unsigned char*)resp->pm_get_forceplotdata_data,
            resp->pm_get_forceplotdata_len * sizeof(resp->pm_get_forceplotdata_data[0]));
#endif
}
#endif // QT_DEBUG

#ifndef BATCH_JSON
void PM::notify_clients(const QString &serial_number, const QJsonObject &json) {
    QList<QWebSocket*>::iterator i;
    QJsonObject out;

    out[serial_number] = json;

    if (!json.empty()) {
        QJsonDocument doc(out);

        for (i = ws_clients.begin(); i != ws_clients.end(); i++) {
            (*i)->sendTextMessage(doc.toJson(QJsonDocument::Compact));
        }

#ifdef QT_DEBUG
        debug_output.write(doc.toJson(QJsonDocument::Indented));
#endif // QT_DEBUG
    }
}
#else // BATCH_JSON
void PM::notify_clients(const QJsonObject &json) {
    QList<QWebSocket*>::iterator i;

    if (!json.empty()) {
        QJsonDocument doc(json);

        for (i = ws_clients.begin(); i != ws_clients.end(); i++) {
            (*i)->sendTextMessage(doc.toJson(QJsonDocument::Compact));
        }

#ifdef QT_DEBUG
        // debug_output.write(doc.toJson(QJsonDocument::Indented));
#endif // QT_DEBUG
    }
}
#endif // BATCH_JSON

/*
 * Transmit a packet across HID. Add report ID.
 */
int PM::hid_tx(hid_device *handle, unsigned char *buffer, unsigned int len) {
    unsigned char buff[CSAFE_MAX_FRAME_SIZE_HID+1] = { 0 }; /* +1 for report ID */

    buff[0] = CSAFE_HID_REPORT_ID;
    memcpy(buff+1, buffer, len);

#ifdef DUMP_PACKETS
    qDebug().noquote() << "tx(" << len << ")";
    util::hexdump(buff+1, len);
#endif // DUMP_PACKETS

    return hid_write(handle, buff, sizeof(buff));
}

int PM::hid_rx(hid_device *handle, unsigned char *buffer, unsigned int len) {
    int rx_bytes;

    rx_bytes = hid_read(handle, buffer, len);

    if (rx_bytes > 0) {
        memmove(buffer, buffer+1, rx_bytes);            /* discard report ID */

        /*
         * We receive an entire packet of CSAFE_MAX_FRAME_SIZE_HID+1 from
         * hid_read. Abuse our abstraction and find the end of the packet, and
         * tell the caller that we only received _that_ number of bytes.
         */
        for (rx_bytes = 0;
                (buffer[rx_bytes-1] != CSAFE_STOP_FLAG) && ((unsigned int)rx_bytes < len);
                rx_bytes++);
#ifdef DUMP_PACKETS
        qDebug().noquote() << "rx(" << rx_bytes << ")";
        util::hexdump(buffer, rx_bytes);
#endif // DUMP_PACKETS
    }

    return rx_bytes;
}

/*
 * private slot
 */
void PM::ws_on_new_connection(void) {
    QWebSocket *s;

    s = ws->nextPendingConnection();
    connect(s, &QWebSocket::disconnected, this, &PM::ws_on_disconnection);
    ws_clients << s;

#ifdef QT_DEBUG
    qDebug() << "new websocket client " << s->peerAddress();
#endif // QT_DEBUG
}

/*
 * private slot
 */
void PM::ws_on_disconnection(void) {
    QWebSocket *s = qobject_cast<QWebSocket*>(sender());
    if (s) {
#ifdef QT_DEBUG
    qDebug() << "websocket disconnection " << s->peerAddress();
#endif // QT_DEBUG
        ws_clients.removeAll(s);
        s->deleteLater();
    }
}

QList<QHostAddress> PM::server_address(void) {
    return QNetworkInterface::allAddresses();
}
quint16 PM::server_port(void) {
    return ws->serverPort();
}

#define JSON_ADD(S) json[#S] = resp->S;
#define JSON_ADD_DOUBLE(S) json[#S] = (double)resp->S;

/*
 * XXX These are garbage. Is this the only way to convert from a byte array to
 * a QJsonArray? There is no array constructor for QJsonValue. Ugh,
 * investigate.
 */
#define JSON_ADD_ARRAY(S) { \
    QJsonArray a; \
    for (unsigned long i = 0; i < sizeof(resp->S) / sizeof(resp->S[0]); i++) { \
        a.append((double)resp->S[i]); \
    } \
    json[#S] = a; \
}

/*
 * private
 *
 * XXX Measure how heavy this is on performance.
 */
void PM::update_json(QJsonObject &json, const csafe_vars_resp_t *resp) {
    JSON_ADD(status);
    JSON_ADD(getversion_mfg_id);
    JSON_ADD(getversion_cid);
    JSON_ADD(getversion_model);
    JSON_ADD_DOUBLE(getversion_hw_version);
    JSON_ADD_DOUBLE(getversion_sw_version);
    JSON_ADD_ARRAY(getid_id);
    JSON_ADD(getunits_units_type);
    JSON_ADD_ARRAY(getserial_serial_number);
    JSON_ADD_DOUBLE(getodometer_odometer);
    JSON_ADD(getodometer_units_specifier);
    JSON_ADD_DOUBLE(geterrorcode_error_code);
    JSON_ADD(gettwork_hours);
    JSON_ADD(gettwork_minutes);
    JSON_ADD(gettwork_seconds);
    JSON_ADD_DOUBLE(gethorizontal_distance);
    JSON_ADD(gethorizontal_distance_units_specifier);
    JSON_ADD_DOUBLE(getcalories_total_calories);
    JSON_ADD(getprogram_workout_number);
    JSON_ADD_DOUBLE(getpace_stroke_pace);
    JSON_ADD(getpace_units_specifier);
    JSON_ADD_DOUBLE(getcadence_stroke_rate);
    JSON_ADD(getcadence_units_specifier);
    JSON_ADD_DOUBLE(getuserinfo_weight);
    JSON_ADD(getuserinfo_weight_units_specifier);
    JSON_ADD(getuserinfo_age);
    JSON_ADD(getuserinfo_gender);
    JSON_ADD(gethrcur_bpm);
    JSON_ADD_DOUBLE(getpower_stroke_watts);
    JSON_ADD_DOUBLE(getpower_units_specifier);
    JSON_ADD(getcaps_max_rx_frame);
    JSON_ADD(getcaps_max_tx_frame);
    JSON_ADD(getcaps_min_interframe);
    JSON_ADD_ARRAY(getcaps_capcode_0x01);
    JSON_ADD_ARRAY(getcaps_capcode_0x02);
    JSON_ADD(pm_get_workout_type);
    JSON_ADD(pm_get_strokerate);
    JSON_ADD(pm_get_drag_factor);
    JSON_ADD(pm_get_stroke_state);
    JSON_ADD_DOUBLE(pm_get_work_time);
    JSON_ADD(pm_get_work_time_fractional);
    JSON_ADD_DOUBLE(pm_get_work_distance);
    JSON_ADD(pm_get_work_distance_fractional);
    JSON_ADD_DOUBLE(pm_get_errorvalue);
    JSON_ADD(pm_get_workout_state);
    JSON_ADD(pm_get_workoutintervalcount);
    JSON_ADD(pm_get_interval_type);
    JSON_ADD_DOUBLE(pm_get_resttime);
    JSON_ADD(pm_get_forceplotdata_len);
    JSON_ADD_ARRAY(pm_get_forceplotdata_data);
    JSON_ADD(pm_get_heartbeatdata_len);
    JSON_ADD_ARRAY(pm_get_heartbeatdata_data);
}
