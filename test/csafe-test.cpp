#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>

#include "csafe.h"
#include "utils.h"

/*
 * generic test
 */
int test1(void) {
    unsigned char buffer[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int len;

    csafe_t csafe;

    csafe_t *c;

    c = &csafe;

    TEST_HEADER

    assert(csafe_add(c, GETSTATUS, NULL) == 0);

    if (c->buff[0] != GETSTATUS) {
        fprintf(stderr, "%s: GETSTATUS failed\n", __FUNCTION__);
        return -1;
    }
    if (c->len != 1) {
        fprintf(stderr, "%s: GETSTATUS length incorrect\n", __FUNCTION__);
        return -2;
    }

    assert(csafe_get_frame(c, buffer, &len) == 0);
    if (memcmp(buffer, "\xf1\x80\x80\xf2", 4)) {
        fprintf(stderr, "%s: csafe_get_frame failed\n", __FUNCTION__);
        hexdump(buffer, len);
        return -3;
    }

    assert(csafe_set_frame(c, buffer, len) == 0);
    if (memcmp(c->buff, "\x80", 1)) {
        fprintf(stderr, "%s: csafe_set_frame failed\n", __FUNCTION__);
        hexdump(c->buff, c->len);
        return -4;
    }

    return 0;
}

int test_byte_stuff_byte_unstuff(void) {
    unsigned char *from_pm = (unsigned char*)"\xf1\xf3\x01\xf3\x02\xf3\x03\xf3\x00\xf2";
    unsigned char *res = (unsigned char*)"\xf1\xf2\xf3";
    unsigned int len;
    unsigned char buff[128];
    csafe_t csafe;

    TEST_HEADER

    /* unpack received frame into csafe context */
    assert(csafe_set_frame(&csafe, from_pm, 10) == 0);
    assert(memcmp(csafe.buff, res, csafe.len) == 0);

    /* pack csafe context into buffer; should match what we started with */
    assert(csafe_get_frame(&csafe, buff, &len) == 0);
    assert(memcmp(from_pm, buff, 10) == 0);

    return 0;
}

int test_failed_checksum_inbound(void) {
    unsigned char bad_from_pm[] = { 0xf1, 0x01, 0x05, 0x80, 0x02, 0x00, 0x01, 0xf9, 0xf2 };
    unsigned char good_from_pm[] = { 0xf1, 0x01, 0x80, 0x01, 0x01, 0x81, 0xf2 };

    csafe_t c;

    TEST_HEADER

    assert(csafe_set_frame(&c, bad_from_pm, sizeof(bad_from_pm)) == -3);
    assert(csafe_set_frame(&c, good_from_pm, sizeof(good_from_pm)) == 0);

    return 0;
}

int test_sizeof_csafe_vars() {
    TEST_HEADER

    printf("csave_vars_req_t: %lu\n", (unsigned long)sizeof(csafe_vars_req_t));
    printf("csave_vars_resp_t: %lu\n", (unsigned long)sizeof(csafe_vars_resp_t));

    return 0;
}

int test_add_short_reqs(void) {
    csafe_t csafe;
    csafe_vars_req_t args;

    csafe_t *c;
    csafe_vars_req_t *a;

    unsigned char commands[] = {
        GETSTATUS,
        RESET,
        GOIDLE,
        GOHAVEID,
        GOINUSE,
        GOFINISHED,
        GOREADY,
        BADID,
        GETVERSION,
        GETID,
        GETUNITS,
        GETSERIAL,
        GETODOMETER,
        GETERRORCODE,
        GETTWORK,
        GETHORIZONTAL,
        GETCALORIES,
        GETPROGRAM,
        GETPACE,
        GETCADENCE,
        GETUSERINFO,
        GETHRCUR,
        GETPOWER
    };
    int i;

    TEST_HEADER

    c = &csafe;
    a = &args;

    csafe_init(c);
    for (i = 0; i < sizeof(commands); i++) {
        csafe_add(c, commands[i], a);
    }

    assert(memcmp(c->buff, commands, c->len) == 0);

    return 0;
}

static void set_time_and_date(csafe_vars_req_t *a) {
    struct timeval tv = { 0, 0 };
    struct tm r;

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &r);

    a->setdate_year = r.tm_year;
    a->setdate_month = r.tm_mon + 1;
    a->setdate_day = r.tm_mday;

    a->settime_hour = r.tm_hour;
    a->settime_minute = r.tm_min;
    a->settime_second = r.tm_sec;
}
void set_twork(csafe_vars_req_t *a) {
    unsigned int seconds = 8741; /* 02:25:41; 0x02 0x19 0x24 */

    a->settwork_hours = seconds / 3600;
    a->settwork_minutes = (seconds % 3600) / 60;
    a->settwork_seconds = seconds % 60;
}
void set_horizontal(csafe_vars_req_t *a) {
    unsigned int distance = 1500;   /* meters */

    a->sethorizontal_distance = distance;
    a->sethorizontal_distance_units_specifier = UNITS_METRES;
}
void set_calories(csafe_vars_req_t *a) {
    unsigned int calories = 123;

    a->setcalories_total_calories = calories;
}
void set_program(csafe_vars_req_t *a) {
    unsigned int program_number = 1;

    a->setprogram_program_number = program_number;
}
void set_power(csafe_vars_req_t *a) {
    unsigned int stroke_watts = 100;

    a->setpower_stroke_watts = stroke_watts;
    a->setpower_stroke_watts_units_specifier = UNITS_WATTS;
}
void set_capability(csafe_vars_req_t *a) {
    a->getcaps_capability_code = CAPS_CODE_CSAFE_PROTOCOL;
}

int test_add_long_reqs(void) {
    csafe_t csafe;
    csafe_vars_req_t args;

    csafe_t *c;
    csafe_vars_req_t *a;

    unsigned char commands[] = {
        AUTOUPLOAD,
        UPLIST,
        UPSTATUSSEC,
        UPLISTSEC,
        IDDIGITS,
        SETTIME,
        SETDATE,
        SETTIMEOUT,
        SETUSERCFG1,
        SETUSERCFG2,
        SETTWORK,
        SETHORIZONTAL,
        SETVERTICAL,
        SETCALORIES,
        SETPROGRAM,
        SETSPEED,
        SETGRADE,
        SETGEAR,
        SETUSERINFO,
        SETTORQUE,
        SETLEVEL,
        SETTARGETHR,
        SETMETS,
        SETPOWER,
        SETHRZONE,
        SETHRMAX,
        SETCHANNELRANGE,
        SETVOLUMERANGE,
        SETAUDIOMUTE,
        SETAUDIOCHANNEL,
        SETAUDIOVOLUME,
        STARTTEXT,
        APPENDTEXT,
        GETTEXTSTATUS,
        GETCAPS,
        SETPMCFG,
        SETPMDATA,
        GETPMCFG,
        GETPMDATA,
    };
    int i;

    TEST_HEADER

    c = &csafe;
    a = &args;

    csafe_vars_req_init(a);
    set_time_and_date(a);
    set_twork(a);
    set_horizontal(a);
    set_calories(a);
    set_program(a);
    set_power(a);
    set_capability(a);

    for (i = 0; i < sizeof(commands); i++) {
        csafe_init(c);
        csafe_add(c, commands[i], a);
    }

    return 0;
}


static const char* getstatus_string(unsigned char status) {
    const char *resp;
    switch (status) {
        case GETSTATUS_ERROR: resp = "error"; break;
        case GETSTATUS_READY: resp = "ready"; break;
        case GETSTATUS_IDLE: resp = "idle"; break;
        case GETSTATUS_HAVEID: resp = "haveid"; break;
        case GETSTATUS_INUSE: resp = "inuse"; break;
        case GETSTATUS_PAUSED: resp = "paused"; break;
        case GETSTATUS_FINISHED: resp = "finished"; break;
        case GETSTATUS_MANUAL: resp = "manual"; break;
        case GETSTATUS_OFFLINE: resp = "offline"; break;
        default: resp = "unknown"; break;
    }

    return resp;
}

static void dump_plot(unsigned int *plot, unsigned int len) {
    unsigned int i;

    for (i = 0; i < len; i++) {
        printf("%d,", plot[i]);
    }
    printf("\n");
}

static void dump_diff_args(csafe_vars_resp_t *a, csafe_vars_resp_t *b) {
    if (a->status != b->status) {
        printf("status: %s / %s\n", getstatus_string(a->status), getstatus_string(b->status));
    }

    if (a->getversion_mfg_id != b->getversion_mfg_id) {
        printf("getversion_mfg_id: %02x / %02x\n", a->getversion_mfg_id , b->getversion_mfg_id);
    }
    if (a->getversion_cid != b->getversion_cid) {
        printf("getversion_cid: %02x / %02x\n", a->getversion_cid, b->getversion_cid);
    }
    if (a->getversion_model != b->getversion_model) {
        printf("getversion_model: %02x / %02x\n", a->getversion_model, b->getversion_model);
    }
    if (a->getversion_hw_version != b->getversion_hw_version) {
        printf("getversion_hw_version: %d/ %d\n", a->getversion_hw_version,b->getversion_hw_version);
    }
    if (a->getversion_sw_version != b->getversion_sw_version) {
        printf("getversion_sw_version: %d / %d\n", a->getversion_sw_version,b->getversion_sw_version);
    }
    if (memcmp(a->getid_id, b->getid_id, 4)) {
        printf("getid_id: diff\n");
    }
    if (a->getunits_units_type != b->getunits_units_type) {
        printf("getunits_units_type: %02x / %02x\n", a->getunits_units_type,b->getunits_units_type);
    }
    if (memcmp(a->getserial_serial_number, b->getserial_serial_number, 8)) {
        printf("getserial_serial_number: diff\n");
    }
    if (a->getodometer_odometer != b->getodometer_odometer) {
        printf("getodometer_odometer: %d / %d\n", a->getodometer_odometer,b->getodometer_odometer);
    }
    if (a->getodometer_units_specifier != b->getodometer_units_specifier) {
        printf("getodometer_units_specifier: %02x / %02x\n", a->getodometer_units_specifier,b->getodometer_units_specifier);
    }
    if (a->geterrorcode_error_code != b->geterrorcode_error_code) {
        printf("geterrorcode_error_code: %02x / %02x\n", a->geterrorcode_error_code, b->geterrorcode_error_code);
    }
    if (a->gettwork_hours != b->gettwork_hours) {
        printf("gettwork_hours: %02x / %02x\n", a->gettwork_hours, b->gettwork_hours);
    }
    if (a->gettwork_minutes != b->gettwork_minutes) {
        printf("gettwork_minutes: %02x / %02x\n", a->gettwork_minutes, b->gettwork_minutes);
    }
    if (a->gettwork_seconds != b->gettwork_seconds) {
        printf("gettwork_seconds: %02x / %02x\n", a->gettwork_seconds, b->gettwork_seconds);
    }
    if (a->gethorizontal_distance != b->gethorizontal_distance) {
        printf("gethorizontal_distance: %02x / %02x\n", a->gethorizontal_distance, b->gethorizontal_distance);
    }
    if (a->gethorizontal_distance_units_specifier != b->gethorizontal_distance_units_specifier) {
        printf("gethorizontal_distance_units_specifier: %02x / %02x\n", a->gethorizontal_distance_units_specifier, b->gethorizontal_distance_units_specifier);
    }
    if (a->getcalories_total_calories != b->getcalories_total_calories) {
        printf("getcalories_total_calories: %02x / %02x\n", a->getcalories_total_calories, b->getcalories_total_calories);
    }
    if (a->getprogram_workout_number != b->getprogram_workout_number) {
        printf("getprogram_workout_number: %02x / %02x\n", a->getprogram_workout_number, b->getprogram_workout_number);
    }
    if (a->getpace_stroke_pace != b->getpace_stroke_pace) {
        printf("getpace_stroke_pace: %02x / %02x\n", a->getpace_stroke_pace, b->getpace_stroke_pace);
    }
    if (a->getpace_units_specifier != b->getpace_units_specifier) {
        printf("getpace_units_specifier: %02x / %02x\n", a->getpace_units_specifier, b->getpace_units_specifier);
    }
    if (a->getcadence_stroke_rate != b->getcadence_stroke_rate) {
        printf("getcadence_stroke_rate: %02x / %02x\n", a->getcadence_stroke_rate, b->getcadence_stroke_rate);
    }
    if (a->getcadence_units_specifier != b->getcadence_units_specifier) {
        printf("getcadence_units_specifier: %02x / %02x\n", a->getcadence_units_specifier, b->getcadence_units_specifier);
    }
    if (a->getuserinfo_weight != b->getuserinfo_weight) {
        printf("getuserinfo_weight: %02x / %02x\n", a->getuserinfo_weight, b->getuserinfo_weight);
    }
    if (a->getuserinfo_weight_units_specifier != b->getuserinfo_weight_units_specifier) {
        printf("getuserinfo_weight_units_specifier: %02x / %02x\n", a->getuserinfo_weight_units_specifier, b->getuserinfo_weight_units_specifier);
    }
    if (a->getuserinfo_age != b->getuserinfo_age) {
        printf("getuserinfo_age: %02x / %02x\n", a->getuserinfo_age, b->getuserinfo_age);
    }
    if (a->getuserinfo_gender != b->getuserinfo_gender) {
        printf("getuserinfo_gender: %02x / %02x\n", a->getuserinfo_gender, b->getuserinfo_gender);
    }
    if (a->gethrcur_bpm != b->gethrcur_bpm) {
        printf("gethrcur_bpm: %02x / %02x\n", a->gethrcur_bpm, b->gethrcur_bpm);
    }
    if (a->getpower_stroke_watts != b->getpower_stroke_watts) {
        printf("getpower_stroke_watts: %d / %d\n", a->getpower_stroke_watts, b->getpower_stroke_watts);
    }
    if (a->getpower_units_specifier != b->getpower_units_specifier) {
        printf("getpower_units_specifier: %d / %d\n", a->getpower_units_specifier, b->getpower_units_specifier);
    }
    if (a->getcaps_max_rx_frame != b->getcaps_max_rx_frame) {
        printf("getcaps_max_rx_frame: %02x / %02x\n", a->getcaps_max_rx_frame, b->getcaps_max_rx_frame);
    }
    if (a->getcaps_max_tx_frame != b->getcaps_max_tx_frame) {
        printf("getcaps_max_tx_frame: %02x / %02x\n", a->getcaps_max_tx_frame, b->getcaps_max_tx_frame);
    }
    if (a->getcaps_min_interframe != b->getcaps_min_interframe) {
        printf("getcaps_min_interframe: %02x / %02x\n", a->getcaps_min_interframe, b->getcaps_min_interframe);
    }
    if (memcmp(a->getcaps_capcode_0x01, b->getcaps_capcode_0x01, sizeof(a->getcaps_capcode_0x01))) {
        printf("getcaps_capcode_0x01: diff\n");
        hexdump(a->getcaps_capcode_0x01, sizeof(a->getcaps_capcode_0x01));
    }
    if (memcmp(a->getcaps_capcode_0x02, b->getcaps_capcode_0x02, sizeof(a->getcaps_capcode_0x02))) {
        printf("getcaps_capcode_0x02: diff\n");
        hexdump(a->getcaps_capcode_0x02, sizeof(a->getcaps_capcode_0x02));
    }
    if (a->pm_get_workout_type != b->pm_get_workout_type) {
        printf("pm_get_workout_type: %02x / %02x\n", a->pm_get_workout_type, b->pm_get_workout_type);
    }
    if (a->pm_get_drag_factor != b->pm_get_drag_factor) {
        printf("pm_get_drag_factor: %02x / %02x\n", a->pm_get_drag_factor, b->pm_get_drag_factor);
    }
    if (a->pm_get_stroke_state != b->pm_get_stroke_state) {
        printf("pm_get_stroke_state: %02x / %02x\n", a->pm_get_stroke_state, b->pm_get_stroke_state);
    }
    if (a->pm_get_work_time != b->pm_get_work_time) {
        printf("pm_get_work_time: %d / %d\n", a->pm_get_work_time, b->pm_get_work_time);
    }
    if (a->pm_get_work_time_fractional != b->pm_get_work_time_fractional) {
        printf("pm_get_work_time_fractional: %02x / %02x\n", a->pm_get_work_time_fractional, b->pm_get_work_time_fractional);
    }
    if (a->pm_get_work_distance != b->pm_get_work_distance) {
        printf("pm_get_work_distance: %d / %d\n", a->pm_get_work_distance, b->pm_get_work_distance);
    }
    if (a->pm_get_work_distance_fractional != b->pm_get_work_distance_fractional) {
        printf("pm_get_work_distance_fractional: %02x / %02x\n", a->pm_get_work_distance_fractional, b->pm_get_work_distance_fractional);
    }
    if (a->pm_get_errorvalue != b->pm_get_errorvalue) {
        printf("pm_get_errorvalue: %d / %d\n", a->pm_get_errorvalue, b->pm_get_errorvalue);
    }
    if (a->pm_get_workout_state != b->pm_get_workout_state) {
        printf("pm_get_workout_state: %02x / %02x\n", a->pm_get_workout_state, b->pm_get_workout_state);
    }
    if (a->pm_get_workoutintervalcount != b->pm_get_workoutintervalcount) {
        printf("pm_get_workoutintervalcount: %02x / %02x\n", a->pm_get_workoutintervalcount, b->pm_get_workoutintervalcount);
    }
    if (a->pm_get_interval_type != b->pm_get_interval_type) {
        printf("pm_get_interval_type: %02x / %02x\n", a->pm_get_interval_type, b->pm_get_interval_type);
    }
    if (a->pm_get_resttime != b->pm_get_resttime) {
        printf("pm_get_resttime: %d / %d\n", a->pm_get_resttime, b->pm_get_resttime);
    }
    if (memcmp(a->pm_get_forceplotdata_data, b->pm_get_forceplotdata_data, sizeof(a->pm_get_forceplotdata_data))) {
        printf("pm_get_forceplotdata_data: diff\n");
        dump_plot(a->pm_get_forceplotdata_data, a->pm_get_forceplotdata_len);
        //hexdump(a->pm_get_forceplotdata_data, sizeof(a->pm_get_forceplotdata_data));
    }
    if (memcmp(a->pm_get_heartbeatdata_data, b->pm_get_heartbeatdata_data, sizeof(a->pm_get_heartbeatdata_data))) {
        printf("pm_get_heartbeatdata_data: diff\n");
        dump_plot(a->pm_get_heartbeatdata_data, a->pm_get_heartbeatdata_len);
        //hexdump(a->pm_get_heartbeatdata_data, sizeof(a->pm_get_heartbeatdata_data));
    }
}

unsigned char checksum(unsigned char *b, int len);
int test_pop_short_resps(void) {
    struct foo {
        const char *test;
        const unsigned char *msg;
        int len;
    } msgs[] = {
        {
            "GETSTATUS response",
            (unsigned char*)"\xf1\x01\x80\x01\x01\x81\xf2\x00",
            /* SOH status GETSTATUS length status checksum EOH */
            /* 0xf1 0x01  0x80      0x01   0x01   0x81     0xf2 */
            7
        }
    };
    int i = 0;
    csafe_t csafe;
    csafe_vars_resp_t resp;
    csafe_vars_resp_t empty;

    csafe_t *c;
    csafe_vars_resp_t *a;
    csafe_vars_resp_t *e;

    c = &csafe;
    a = &resp;
    e = &empty;

    TEST_HEADER

    csafe_init(c);
    csafe_vars_resp_init(e);

    //for (i = 0; i < sizeof(msgs) / sizeof(struct foo); i++) {
        csafe_vars_resp_init(a);

        assert(csafe_set_frame(c, msgs[i].msg, msgs[i].len) == 0);
        assert(csafe_unpack_resp(c, a) == 0);

        dump_diff_args(a, e);
    //}

    return 0;
}

/*
 * Test various commands from the C2 CID and CSAFE spec.
 */
int test_spec_commands(void) {
    csafe_t csafe;
    csafe_vars_req_t args;

    unsigned char buffer[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int len;

    csafe_t *c = &csafe;
    csafe_vars_req_t *a = &args;

    TEST_HEADER

    csafe_init(c);
    a->pm3_specific_cmd = PM_GET_WORKTIME;
    csafe_add(c, SETUSERCFG1, a);
    csafe_get_frame(c, buffer, &len);
    assert(memcmp(buffer, "\xf1\x1a\x01\xa0\xbb\xf2", len) == 0);

    csafe_init(c);
    a->sethorizontal_distance = 2;
    a->sethorizontal_distance_units_specifier = UNITS_KILOMETRE;
    csafe_add(c, SETHORIZONTAL, a);
    csafe_get_frame(c, buffer, &len);
    assert(memcmp(buffer, "\xf1\x21\x03\x02\x00\x21\x01\xf2", len) == 0);

    csafe_init(c);
    a->pm3_specific_cmd = PM_SET_SPLITDURATION;
    a->set_splitduration_type = SPLITDURATION_DISTANCE;
    a->set_splitduration_duration = 500;
    csafe_add(c, SETUSERCFG1, a);
    csafe_get_frame(c, buffer, &len);
    assert(memcmp(buffer, "\xf1\x1a\x07\x05\x05\x80\xf4\x01\x00\x00\x68\xf2", len) == 0);

    return 0;
}

int test_combined_commands() {
    csafe_t csafe;
    csafe_vars_req_t args;

    csafe_t *c = &csafe;
    csafe_vars_req_t *a = &args;

    TEST_HEADER

    csafe_init(c);
    a->sethorizontal_distance = 2;
    a->sethorizontal_distance_units_specifier = UNITS_KILOMETRE;
    csafe_add(c, SETHORIZONTAL, a);

    a->pm3_specific_cmd = PM_SET_SPLITDURATION;
    a->set_splitduration_type = SPLITDURATION_DISTANCE;
    a->set_splitduration_duration = 500;
    csafe_add(c, SETUSERCFG1, a);

    a->setpower_stroke_watts = 300;
    a->setpower_stroke_watts_units_specifier = UNITS_WATTS;
    csafe_add(c, SETPOWER, a);

    a->setprogram_program_number = 0;
    csafe_add(c, SETPROGRAM, a);

    assert(memcmp(c->buff,
                "\x21\x03\x02\x00\x21\x1a\x07\x05\x05\x80\xf4\x01\x00\x00\x34\x03\x2c\x01\x58\x24\x02\x00\x00",\
                c->len) == 0);

    return 0;
}

extern void bytes2val(unsigned char *b, unsigned int l, unsigned int *v);
int test_forceplot(void) {
    csafe_vars_resp_t args;
    csafe_vars_resp_t *a;
    int i, j;
    unsigned int len = 32;
    unsigned char bytes[][32] = {
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb4, 0x00, 0xb4, 0x00, 0xb4, 0x00, 0xb4, 0x00, 0xb3, 0x00, 0xb3, 0x00, 0xb3, 0x00, 0xb3, 0x00, 0xad, 0x00, 0xad, 0x00, 0xad, 0x00, 0xad, 0x00 },
        { 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x26, 0x00, 0x26, 0x00, 0x26, 0x00, 0x49, 0x00, 0x49, 0x00, 0x49, 0x00, 0x68, 0x00, 0x68, 0x00, 0x68, 0x00, 0x89, 0x00, 0x89, 0x00, 0x89, 0x00, 0x9b, 0x00 },
        { 0x1d, 0x00, 0x1d, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x79, 0x00, 0x79, 0x00, 0x79, 0x00, 0x96, 0x00, 0x96, 0x00, 0xa1, 0x00, 0xa1, 0x00, 0xa1, 0x00, 0xa7, 0x00 },
        { 0x16, 0x00, 0x16, 0x00, 0x31, 0x00, 0x31, 0x00, 0x4f, 0x00, 0x4f, 0x00, 0x4f, 0x00, 0x64, 0x00, 0x64, 0x00, 0x82, 0x00, 0x82, 0x00, 0x95, 0x00, 0x95, 0x00, 0x95, 0x00, 0xae, 0x00, 0xae, 0x00 },
        { 0x2b, 0x00, 0x2b, 0x00, 0x50, 0x00, 0x50, 0x00, 0x72, 0x00, 0x72, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0xa2, 0x00, 0xa2, 0x00, 0xb9, 0x00, 0xb9, 0x00, 0xcc, 0x00, 0xcc, 0x00, 0xca, 0x00 },
        { 0x1b, 0x00, 0x1b, 0x00, 0x34, 0x00, 0x34, 0x00, 0x4f, 0x00, 0x4f, 0x00, 0x61, 0x00, 0x61, 0x00, 0x81, 0x00, 0x81, 0x00, 0x94, 0x00, 0x94, 0x00, 0xb5, 0x00, 0xb5, 0x00, 0xb5, 0x00, 0xc2, 0x00 },
        { 0x31, 0x00, 0x31, 0x00, 0x56, 0x00, 0x56, 0x00, 0x56, 0x00, 0x74, 0x00, 0x74, 0x00, 0x7d, 0x00, 0x7d, 0x00, 0xa0, 0x00, 0xa0, 0x00, 0xbb, 0x00, 0xbb, 0x00, 0xd4, 0x00, 0xd4, 0x00, 0xcd, 0x00 },
        { 0x31, 0x00, 0x31, 0x00, 0x57, 0x00, 0x57, 0x00, 0x80, 0x00, 0x80, 0x00, 0x8f, 0x00, 0x8f, 0x00, 0xb4, 0x00, 0xb4, 0x00, 0xe4, 0x00, 0xe4, 0x00, 0xf0, 0x00, 0xf0, 0x00, 0xec, 0x00, 0xec, 0x00 },
        { 0x32, 0x00, 0x32, 0x00, 0x58, 0x00, 0x58, 0x00, 0x83, 0x00, 0x83, 0x00, 0x95, 0x00, 0x95, 0x00, 0xb0, 0x00, 0xb0, 0x00, 0xdb, 0x00, 0xdb, 0x00, 0xec, 0x00, 0xec, 0x00, 0xee, 0x00, 0xd5, 0x00 },
        { 0x22, 0x00, 0x22, 0x00, 0x48, 0x00, 0x48, 0x00, 0x6e, 0x00, 0x6e, 0x00, 0x80, 0x00, 0x80, 0x00, 0xa1, 0x00, 0xa1, 0x00, 0xb8, 0x00, 0xb8, 0x00, 0xd8, 0x00, 0xd8, 0x00, 0xe5, 0x00, 0xe5, 0x00 },
        { 0x2c, 0x00, 0x2c, 0x00, 0x3b, 0x00, 0x3b, 0x00, 0x51, 0x00, 0x51, 0x00, 0x61, 0x00, 0x61, 0x00, 0x72, 0x00, 0x72, 0x00, 0x84, 0x00, 0x84, 0x00, 0x94, 0x00, 0x94, 0x00, 0x9a, 0x00, 0x9a, 0x00 },
        { 0x28, 0x00, 0x28, 0x00, 0x45, 0x00, 0x45, 0x00, 0x6b, 0x00, 0x6b, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x8e, 0x00, 0x8e, 0x00, 0x8e, 0x00, 0xa5, 0x00, 0xa5, 0x00, 0xbe, 0x00, 0xc0, 0x00, 0xc0, 0x00 },
        { 0x19, 0x00, 0x19, 0x00, 0x32, 0x00, 0x32, 0x00, 0x47, 0x00, 0x47, 0x00, 0x6d, 0x00, 0x6d, 0x00, 0x6f, 0x00, 0x6f, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x95, 0x00, 0x95, 0x00, 0xa3, 0x00, 0xa3, 0x00 },
        { 0xa8, 0x00, 0xa8, 0x00, 0x9f, 0x00, 0x9f, 0x00, 0x8e, 0x00, 0x8e, 0x00, 0x93, 0x00, 0x93, 0x00, 0x8c, 0x00, 0x8c, 0x00, 0x80, 0x00, 0x6e, 0x00, 0x6e, 0x00, 0x68, 0x00, 0x68, 0x00, 0x67, 0x00 },
        { 0x67, 0x00, 0x70, 0x00, 0x70, 0x00, 0x6a, 0x00, 0x6a, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x54, 0x00, 0x58, 0x00, 0x58, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x59, 0x00, 0x4b, 0x00, 0x4b, 0x00 },
        { 0x37, 0x00, 0x32, 0x00, 0x32, 0x00, 0x20, 0x00, 0x20, 0x00, 0x16, 0x00, 0x16, 0x00, 0x23, 0x00, 0x23, 0x00, 0x07, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
    };
    unsigned char num_frames = sizeof(bytes) / 32;

    a = &args;

    for (j = 0; j < num_frames; j++) {
        for (i = 0; i < len; i += 2) {
            bytes2val(&bytes[j][i], 2, &a->pm_get_forceplotdata_data[i/2]);
        }

        a->pm_get_forceplotdata_len = len / 2;
        dump_plot(a->pm_get_forceplotdata_data, a->pm_get_forceplotdata_len);
    }

    return 0;
}

int main(int argc, const char *argv[]) {
    csafe_t a_csafe_structure;
    csafe_t *c;
    int res = 0;

    c = &a_csafe_structure;

    assert(csafe_init(c) == 0);

    res += test1();
    res += test_byte_stuff_byte_unstuff();
    res += test_failed_checksum_inbound();

    res += test_sizeof_csafe_vars();

    res += test_add_short_reqs();
    res += test_add_long_reqs();
    res += test_pop_short_resps();

    res += test_spec_commands();
    res += test_combined_commands();

    res += test_forceplot();

    return res;
}
