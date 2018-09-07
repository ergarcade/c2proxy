#ifndef CSAFE_H
#define CSAFE_H

#ifdef __cplusplus
extern "C" {
#endif

/* CSAFE 2.1.1 */
#define CSAFE_FRAME_START_MASK  0xf0
#define CSAFE_EXT_START_FLAG    0xf0
#define CSAFE_START_FLAG        0xf1
#define CSAFE_STOP_FLAG         0xf2
#define CSAFE_BYTE_STUFF_FLAG   0xf3

/*
 * Sample usage:
 *
 * csafe_t some_csafe_obj;
 * csafe_vars_req_t some_csafe_args;
 * int len;
 * unsigned char buff[CSAFE_MAX_FRAME_SIZE_HID];
 *
 * csafe_t *c = &some_csafe_obj;
 * csafe_vars_req_t *a = &some_csafe_args;
 *
 * a->day = the_day_right_now();
 * a->month = the_month_right_now();
 * a->year = the_year_right_now();
 * a->hour = the_hour_right_now();
 * a->minute = the_minute_right_now();
 * a->second = the_second_right_now();
 *
 * csafe_init(c);
 * csafe_add(c, CSAFE_SETDATE_CMD, a);
 *
 * csafe_get_frame(c, buff, &len);
 * tx(buff, len);
 * ...
 * rx(buff, len);
 * csafe_set_frame(c, buff, len);
 * csafe_unpack(c, a);      <- if this is not a response
 * csafe_unpack_resp(c, a); <- if this is a response 
 */

/*
 * PM3 specific commands:
 *
 * csafe_init(c);
 * a->pm3_specific_cmd = PM_GET_WORKOUTTYPE;
 * csafe_add(c, SETUSERCFG1, a);
 *
 * a->pm3_specific_cmd = PM_GET_DRAGFACTOR;
 * csafe_add9c, SETUSERCFG1, a);
 *
 * a->pm3_specific_cmd = PM_SET_SPLITDURATION;
 * a->set_splitduration_type = SPLITDURATION_TIME;
 * a->set_splitduration_duration = 60;
 * csafe_add(c, PM_SET_SPLITDURATION, a);
 *
 * a->pm3_specific_cmd = PM_GET_FORCEPLOTDATA;
 * csafe_add(c, PM_GET_FORCEPLOTDATA, a);
 */

/*
 * The maximum frame size in the C2 spec is 96 bytes,
 * but HID seems to work best with a report id of 2
 * and a frame size of 120. This frame size is also
 * listed in C2 CID page 23.
 */
#define CSAFE_MAX_FRAME_SIZE_HID    120

/*
 * Concept2 report ID.
 */
#define CSAFE_HID_REPORT_ID         0x02

/* CSAFE 3.1.3.3 Slave capabilities */
#define CAPS_CODE_CSAFE_PROTOCOL    0
#define CAPS_CODE_CSAFE_POWER       1
#define CAPS_CODE_TEXT              2
#define CAPS_CODE_COMMAND_SUPPORT   3

/*
 * CSAFE 3.2.2 Units.
 */
#define UNITS_MILE                          1
#define UNITS_TENTH_MILE                    2
#define UNITS_HUNDRETH_MILE                 3
#define UNITS_THOUSANDTH_MILE               4
#define UNITS_FEET                          5
#define UNITS_INCH                          6
#define UNITS_POUNDS                        7
#define UNITS_TENTH_POUNDS                  8
#define UNITS_TEN_FEET                      10
#define UNITS_MILES_PER_HOUR                16
#define UNITS_TENTH_MILES_PER_HOUR          17
#define UNITS_HUNDRETH_MILES_PER_HOUR       18
#define UNITS_FEET_PER_MINUTE               19
#define UNITS_KILOMETRE                     33
#define UNITS_TENTH_KILOMETRE               34
#define UNITS_HUNDREDTH_KILOMETRE           35
#define UNITS_METRES                        36
#define UNITS_TENTH_METRE                   37
#define UNITS_CENTIMETRE                    38
#define UNITS_KILOGRAM                      39
#define UNITS_TENTH_KILOGRAM                40
#define UNITS_KILOMETRE_PER_HOUR            48
#define UNITS_TENTH_KILOMETRE_PER_HOUR      49
#define UNITS_HUNDREDTH_KILOMETRE_PER_HOUR  50
#define UNITS_METRES_PER_MINUTE             51
#define UNITS_MINUTES_PER_MILE              55
#define UNITS_MINUTES_PER_KILOMETRE         56
#define UNITS_SECONDS_PER_KILOMETRE         57
#define UNITS_SECONDS_PER_MILE              58
#define UNITS_FLOORS                        65
#define UNITS_TENTH_FLOORS                  66
#define UNITS_STEPS                         67
#define UNITS_REVOLUTIONS                   68
#define UNITS_STRIDES                       69
#define UNITS_STROKES                       70
#define UNITS_BEATS                         71
#define UNITS_CALORIES                      72
#define UNITS_KILOPASCALS                   73
#define UNITS_PERCENT_GRADE                 74
#define UNITS_HUNDREDTH_PERCENT_GRADE       75
#define UNITS_TENTH_PERCENT_GRADE           76
#define UNITS_TENTH_FLOORS_PER_MINUTE       79
#define UNITS_FLOORS_PER_MINUTE             80
#define UNITS_STEPS_PER_MINUTE              81
#define UNITS_REVS_PER_MINUTE               82
#define UNITS_STRIDES_PER_MINUTE            83
#define UNITS_STROKES_PER_MINUTE            84
#define UNITS_BEATS_PER_MINUTE              85
#define UNITS_CALORIES_PER_MINUTE           86
#define UNITS_CALORIES_PER_HOUR             87
#define UNITS_WATTS                         88
#define UNITS_KPM                           89
#define UNITS_INCH_POUNDS                   90
#define UNITS_FOOT_POUNDS                   91
#define UNITS_NEWTON_METRES                 92
#define UNITS_VOLTS                         99
#define UNITS_THOUSANDTHS_VOLTS             100

/* CSAFE 3.1.2.1 */
/* short commands */
#define GETSTATUS           0x80
#define RESET               0x81
#define GOIDLE              0x82
#define GOHAVEID            0x83
#define GOINUSE             0x84
#define GOFINISHED          0x86
#define GOREADY             0x87
#define BADID               0x88
#define GETVERSION          0x91
#define GETID               0x92
#define GETUNITS            0x93
#define GETSERIAL           0x94
#define GETLIST             0x98
#define GETUTILIZATION      0x99
#define GETMOTORCURRENT     0x9a
#define GETODOMETER         0x9b
#define GETERRORCODE        0x9c
#define GETSERVICECODE      0x9d
#define GETUSERCFG1         0x9e
#define GETUSERCFG2         0x9f
#define GETTWORK            0xa0
#define GETHORIZONTAL       0xa1
#define GETVERTICAL         0xa2
#define GETCALORIES         0xa3
#define GETPROGRAM          0xa4
#define GETSPEED            0xa5
#define GETPACE             0xa6
#define GETCADENCE          0xa7
#define GETGRADE            0xa8
#define GETGEAR             0xa9
#define GETUPLIST           0xaa
#define GETUSERINFO         0xab
#define GETTORQUE           0xac
#define GETHRCUR            0xb0
#define GETHRTZONE          0xb2
#define GETMETS             0xb3
#define GETPOWER            0xb4
#define GETHRAVG            0xb5
#define GETHRMAX            0xb6
#define GETUSERDATA1        0xbe
#define GETUSERDATA2        0xbf
#define GETAUDIOCHANNEL     0xc0
#define GETAUDIOVOLUME      0xc1
#define GETAUDIOMUTE        0xc2
#define ENDTEXT             0xe0
#define DISPLAYPOPUP        0xe1
#define GETPOPUPSTATUS      0xe5

/* Long commands */
#define AUTOUPLOAD          0x01
#define UPLIST              0x02
#define UPSTATUSSEC         0x04
#define UPLISTSEC           0x05
#define IDDIGITS            0x10
#define SETTIME             0x11
#define SETDATE             0x12
#define SETTIMEOUT          0x13
#define SETUSERCFG1         0x1a
#define SETUSERCFG2         0x1b
#define SETTWORK            0x20
#define SETHORIZONTAL       0x21
#define SETVERTICAL         0x22
#define SETCALORIES         0x23
#define SETPROGRAM          0x24
#define SETSPEED            0x25
#define SETGRADE            0x28
#define SETGEAR             0x29
#define SETUSERINFO         0x2b
#define SETTORQUE           0x2c
#define SETLEVEL            0x2d
#define SETTARGETHR         0x30
#define SETMETS             0x33
#define SETPOWER            0x34
#define SETHRZONE           0x35
#define SETHRMAX            0x36
#define SETCHANNELRANGE     0x40
#define SETVOLUMERANGE      0x41
#define SETAUDIOMUTE        0x42
#define SETAUDIOCHANNEL     0x43
#define SETAUDIOVOLUME      0x44
#define STARTTEXT           0x60
#define APPENDTEXT          0x61
#define GETTEXTSTATUS       0x65
#define GETCAPS             0x70
#define SETPMCFG            0x76
#define SETPMDATA           0x77
#define GETPMCFG            0x7e
#define GETPMDATA           0x7f

/*
 * C2 CID CSAFE PM3-Specific Commands: short commands (page 53)
 */
#define PM_GET_STROKERATE           0x83    /* C2 PM Bluetooth CID */
#define PM_GET_WORKOUTTYPE          0x89
#define PM_GET_DRAGFACTOR           0xc1
#define PM_GET_STROKESTATE          0xbf
#define PM_GET_WORKTIME             0xa0
#define PM_GET_WORKDISTANCE         0xa3
#define PM_GET_ERRORVALUE           0xc9
#define PM_GET_WORKOUTSTATE         0x8d
#define PM_GET_ROWINGSTATE          0x93
#define PM_GET_WORKOUTINTERVALCOUNT 0x9f
#define PM_GET_INTERVALTYPE         0x8e
#define PM_GET_RESTTIME             0xcf

/*
 * C2 CID CSAFE PM3-Specific Commands: long commands (page 54)
 */
#define PM_SET_SPLITDURATION        0x05
#define PM_GET_FORCEPLOTDATA        0x6b
#define PM_SET_SCREENERRORMODE      0x27
#define PM_GET_HEARTBEATDATA        0x6c

/* CSAFE 2.5.3 Slave status; GETSTATUS response codes */
#define GETSTATUS_ERROR             0
#define GETSTATUS_READY             1
#define GETSTATUS_IDLE              2
#define GETSTATUS_HAVEID            3
#define GETSTATUS_INUSE             5
#define GETSTATUS_PAUSED            6
#define GETSTATUS_FINISHED          7
#define GETSTATUS_MANUAL            8
#define GETSTATUS_OFFLINE           9

/* C2 CID PM3 specific long commands (page 54) */
#define SPLITDURATION_TIME      0
#define SPLITDURATION_DISTANCE  128

#define SCREENERRORMODE_DISABLE 0
#define SCREENERRORMODE_ENABLE  1

/* C2 PM Bluetooth CID Appendix A - PM_GET_WORKOUTTYPE */
#define WORKOUTTYPE_JUSTROW_NOSPLITS                0
#define WORKOUTTYPE_JUSTROW_SPLITS                  1
#define WORKOUTTYPE_FIXEDDIST_NOSPLITS              2
#define WORKOUTTYPE_FIXEDDIST_SPLITS                3
#define WORKOUTTYPE_FIXEDTIME_NOSPLITS              4
#define WORKOUTTYPE_FIXEDTIME_SPLITS                5
#define WORKOUTTYPE_FIXEDTIME_INTERVAL              6
#define WORKOUTTYPE_FIXEDDIST_INTERVAL              7
#define WORKOUTTYPE_VARIABLE_INTERVAL               8
#define WORKOUTTYPE_VARIABLE_UNDEFINEDREST_INTERVAL 9
#define WORKOUTTYPE_FIXED_CALORIE                   10
#define WORKOUTTYPE_FIXED_WATTMINUTES               11
#define WORKOUTTYPE_FIXEDCALS_INTERVAL              12
#define WORKOUTTYPE_NUM                             13

/* C2 PM Bluetooth CID Appendix A - PM_GET_INTERVALTYPE */
#define INTERVALTYPE_TIME                           0
#define INTERVALTYPE_DIST                           1
#define INTERVALTYPE_REST                           2
#define INTERVALTYPE_TIMERESTUNDEFINED              3
#define INTERVALTYPE_DISTANCERESTUNDEFINED          4
#define INTERVALTYPE_RESTUNDEFINED                  5
#define INTERVALTYPE_CAL                            6
#define INTERVALTYPE_CALRESTUNDEFINED               7
#define INTERVALTYPE_WATTMINUTE                     8
#define INTERVALTYPE_WATTMINUTERESTUNDEFINED        9
#define INTERVALTYPE_NONE                           0xff

/* C2 PM Bluetooth CID Appendix A - PM_GET_WORKOUTSTATE */
#define WORKOUTSTATE_WAITTOBEGIN                    0
#define WORKOUTSTATE_WORKOUTROW                     1
#define WORKOUTSTATE_COUNTDOWNPAUSE                 2
#define WORKOUTSTATE_INTERVALREST                   3
#define WORKOUTSTATE_INTERVALWORKTIME               4
#define WORKOUTSTATE_INTERVALWORKDISTANCE           5
#define WORKOUTSTATE_INTERVALRESTENDTOWORKTIME      6
#define WORKOUTSTATE_INTERVALRESTENDTOWORKDISTANCE  7
#define WORKOUTSTATE_INTERVALWORKTIMETOREST         8
#define WORKOUTSTATE_INTERVALWORKDISTANCETOREST     9
#define WORKOUTSTATE_WORKOUTEND                     10
#define WORKOUTSTATE_TERMINATE                      11
#define WORKOUTSTATE_WORKOUTLOGGED                  12
#define WORKOUTSTATE_REARM                          13

/* C2 PM Bluetooth CID Appendix A - PM_GET_STROKESTATE */
#define STROKESTATE_WAITING_FOR_WHEEL_TO_REACH_MIN_SPEED_STATE  0
#define STROKESTATE_WAITING_FOR_WHEEL_TO_ACCELERATE_STATE       1
#define STROKESTATE_DRIVING_STATE                               2
#define STROKESTATE_DWELLING_AFTER_DRIVE_STATE                  3
#define STROKESTATE_RECOVERY_STATE                              4

/*
 * Heart beat data.
 */
#define HEARTBEATDATA_BYTES     32
#define HEARTBEATDATA_LENGTH    16

#define FORCEPLOTDATA_BYTES     32  /* in request: how many bytes we want to capture */
#define FORCEPLOTDATA_LENGTH    144 /* how much we store */

/*
 * Structures.
 */
typedef struct csafe_st {
    unsigned char buff[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int len;
} csafe_t;

typedef struct csafe_vars_resp_st {
    unsigned char status;

    /* C2 CID Appendix A: Short commands - GETVERSION */
    unsigned char getversion_mfg_id;
    unsigned char getversion_cid;
    unsigned char getversion_model;
    unsigned int getversion_hw_version;
    unsigned int getversion_sw_version;

    /* C2 CID Appendix A: Short commands - GETID */
    unsigned char getid_id[5];

    /* C2 CID Appendix A: Short commands - GETUNITS */
    unsigned char getunits_units_type;

    /* C2 CID Appendix A: Short commands - GETSERIAL */
    unsigned char getserial_serial_number[9];

    /* C2 CID Appendix A: Short commands - GETODOMETER */
    unsigned int getodometer_odometer;
    unsigned char getodometer_units_specifier;

    /* C2 CID Appendix A: Short commands - GETERRORCODE */
    unsigned int geterrorcode_error_code;

    /* C2 CID Appendix A: Short commands - GETTWORK */
    unsigned char gettwork_hours;
    unsigned char gettwork_minutes;
    unsigned char gettwork_seconds;

    /* C2 CID Appendix A: Short commands - GETHORIZONTAL */
    unsigned int gethorizontal_distance;
    unsigned char gethorizontal_distance_units_specifier;

    /* C2 CID Appendix A: Short commands - GETCALORIES */
    unsigned int getcalories_total_calories;

    /* C2 CID Appendix A: Short commands - GETPROGRAM */
    unsigned char getprogram_workout_number;

    /* C2 CID Appendix A: Short commands - GETPACE */
    unsigned int getpace_stroke_pace;
    unsigned char getpace_units_specifier;

    /* C2 CID Appendix A: Short commands - GETCADENCE */
    unsigned int getcadence_stroke_rate;
    unsigned char getcadence_units_specifier;

    /* C2 CID Appendix A: Short commands - GETUSERINFO */
    unsigned int getuserinfo_weight;
    unsigned char getuserinfo_weight_units_specifier;
    unsigned char getuserinfo_age;
    unsigned char getuserinfo_gender;

    /* C2 CID Appendix A: Short commands - GETHRCUR */
    unsigned char gethrcur_bpm;

    /* C2 CID Appendix A: Short commands - GETPOWER */
    unsigned int getpower_stroke_watts;
    unsigned char getpower_units_specifier;

    /* C2 CID Appendix A: Long commands - GETCAPS */
    unsigned char getcaps_max_rx_frame;
    unsigned char getcaps_max_tx_frame;
    unsigned char getcaps_min_interframe;
    unsigned char getcaps_capcode_0x01[4];
    unsigned char getcaps_capcode_0x02[11];

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKOUTTYPE */
    unsigned char pm_get_workout_type;

    /* C2 PM Bluetooth CID - PM_GET_STROKERATE */
    unsigned char pm_get_strokerate;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_DRAGFACTOR */
    unsigned char pm_get_drag_factor;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_STROKESTATE */
    unsigned char pm_get_stroke_state;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKTIME */
    unsigned int pm_get_work_time;
    unsigned char pm_get_work_time_fractional;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKDISTANCE */
    unsigned int pm_get_work_distance;
    unsigned char pm_get_work_distance_fractional;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_ERRORVALUE */
    unsigned int pm_get_errorvalue;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKOUTSTATE */
    unsigned char pm_get_workout_state;

    /* C2 PM Bluetooth CID - PM_GET_ROWINGSTATE */
    unsigned char pm_get_rowing_state;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKOUTINTERVALCOUNT */
    unsigned char pm_get_workoutintervalcount;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_INTERVALTYPE */
    unsigned char pm_get_interval_type;

    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_RESTTIME */
    unsigned int pm_get_resttime;

    /* C2 CID Appendix A: PM3 specific long commands - PM_GET_FORCEPLOTDATA */
    unsigned char pm_get_forceplotdata_len;

    /* Full frame - clients should use this */
    unsigned int pm_get_forceplotdata_data[FORCEPLOTDATA_LENGTH];

    /*
     * Part of a frame - this is updated per-request; clients
     * shouldn't use this one as it is incomplete data.
     *
     * See csafe/csafe.cpp: cmd_pm_get_forceplotdata_resp for more info.
     */
    unsigned int pm_get_forceplotdata_idx;
    unsigned int pm_get_forceplotdata_data_cur[FORCEPLOTDATA_LENGTH];

    unsigned int pm_get_stroke_state_last;  /* track previous stroke state */

    /* C2 CID Appendix A: PM3 specific long commands - PM_GET_HEARTBEATDATA */
    unsigned char pm_get_heartbeatdata_len;
    unsigned int pm_get_heartbeatdata_data[HEARTBEATDATA_LENGTH];

} csafe_vars_resp_t;

typedef struct csafe_vars_req_st {
    /* C2 CID Appendix A: long commands - AUTOUPLOAD */
    unsigned char autoupload_configuration;

    /* C2 CID Appendix A: long commands - IDDIGITS */
    unsigned char iddigits_num;

    /* C2 CID Appendix A: long commands - SETTIME */
    unsigned char settime_hour;
    unsigned char settime_minute;
    unsigned char settime_second;

    /* C2 CID Appendix A: long commands - SETDATE */
    unsigned char setdate_year;
    unsigned char setdate_month;
    unsigned char setdate_day;

    /* C2 CID Appendix A: long commands - SETTIMEOUT */
    unsigned char settimeout_state_timeout;

    /* C2 CID Appendix A: long commands - SETTWORK */
    unsigned char settwork_hours;
    unsigned char settwork_minutes;
    unsigned char settwork_seconds;

    /* C2 CID Appendix A: long commands - SETHORIZONTAL */
    unsigned int sethorizontal_distance;
    unsigned char sethorizontal_distance_units_specifier;

    /* C2 CID Appendix A: long commands - SETCALORIES */
    unsigned int setcalories_total_calories;

    /* C2 CID Appendix A: long commands - SETPROGRAM */
    unsigned int setprogram_program_number;

    /* C2 CID Appendix A: long commands - SETPOWER */
    unsigned int setpower_stroke_watts;
    unsigned char setpower_stroke_watts_units_specifier;

    /* C2 CID Appendix A: long commands - GETCAPS */
    unsigned char getcaps_capability_code;

    /* C2 CID CSAFE PM3 long commands (page 54) - PM_SET_SPLITDURATION */
    unsigned char set_splitduration_type;
    unsigned int set_splitduration_duration;

    /* C2 CID CSAFE PM3 long commands (page 54) - PM_SET_SCREENERRORMODE */
    unsigned char set_screenerrormode_mode;

    /*
     * PM3 specific commands are handled through SETUSERCFG1.
     * Caller must set pm3_specific_cmd and appropriate args
     * before using csafe_add().
     */
    unsigned char pm3_specific_cmd;

    /* C2 CID Appendix A: PM3 specific long commands - PM_GET_FORCEPLOTDATA */
    unsigned char pm_get_forceplotdata_length;

    /* C2 CID Appendix A: PM3 specific long commands - PM_GET_HEARTBEATDATA */
    unsigned char pm_get_heartbeatdata_length;

} csafe_vars_req_t;

typedef unsigned char csafe_cmd_t;


/*
 * Function prototypes.
 */

int csafe_init(csafe_t *c);                     /* only standard frames for now */
int csafe_vars_req_init(csafe_vars_req_t *a);
int csafe_vars_resp_init(csafe_vars_resp_t *a);

int csafe_add(csafe_t *c, csafe_cmd_t cmd, csafe_vars_req_t *a);
int csafe_unpack(csafe_t *c, csafe_vars_resp_t *r);
int csafe_unpack_resp(csafe_t *c, csafe_vars_resp_t *r);

int csafe_get_frame(csafe_t *c, unsigned char *buff, unsigned int *len);
int csafe_set_frame(csafe_t *c, const unsigned char *buff, const unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* CSAFE_H */
