/*
 * Generate queries. These can be included in the proxy
 * at compile time, saving having to build the queries
 * at runtime.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "csafe.h"
#include "utils.h"

/*
 * Dump out a C string consisting of buffer of length len;
 * prefix with function name so we know what is going on.
 */
void dump_c_string(const char *function, unsigned char *buffer, unsigned int len) {
    unsigned int i;

    printf("/* pm_%s */\n", function);
    printf("unsigned char pm_%s[%d] = ", function, len);

    printf("{\n    ");
    for (i = 0; i < len; ) {
        printf("0x%02x", buffer[i]);

        if (i < len - 1) {
            printf(", ");
        }

        i++;

        if ((i % 8) == 0) {
            printf("\n    ");
        }
    }
    printf("\n};\n");
}

void query_versions(void) {
    unsigned char buffer[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int len;

    csafe_t csafe_obj;
    csafe_vars_req_t csafe_vars;

    csafe_t *c = &csafe_obj;
    csafe_vars_req_t *a = &csafe_vars;

    /*
     * Set request arguments.
     */
    a->getcaps_capability_code = 0;

    /*
     * Create our frame.
     */
    assert(csafe_init(c) == 0);
    assert(csafe_add(c, GETVERSION, a) == 0);
    assert(csafe_add(c, GETID, a) == 0);
    assert(csafe_add(c, GETSERIAL, a) == 0);
    assert(csafe_add(c, GETODOMETER, a) == 0);
    assert(csafe_add(c, GETCAPS, a) == 0);

    csafe_get_frame(c, buffer, &len);

    dump_c_string(__FUNCTION__, buffer, len);
}

/*
 * messing about with commands. Ignore this guy.
 */
void query_workout_test_play_dont_use(void) {
    unsigned char buffer[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int len;

    int pm3_short[] = {
        PM_GET_WORKTIME,
        PM_GET_WORKDISTANCE,
        PM_GET_DRAGFACTOR,
        PM_GET_STROKESTATE,
        PM_GET_FORCEPLOTDATA,
        PM_GET_WORKOUTSTATE
    };
    int i;

    csafe_t csafe_obj;
    csafe_vars_req_t csafe_vars;

    csafe_t *c = &csafe_obj;
    csafe_vars_req_t *a = &csafe_vars;

    memset(a, 0, sizeof(csafe_vars_req_t));

    /*
     * Create our frame.
     */
    assert(csafe_init(c) == 0);

    /*
     * Workout stuff.
     */
    assert(csafe_add(c, GETSTATUS, a) == 0);
    assert(csafe_add(c, GETERRORCODE, a) == 0);

    /*
     * PM3 specific commands.
     */
    for (i = 0; i < sizeof(pm3_short) / sizeof(pm3_short[0]); i++) {
        a->pm3_specific_cmd = pm3_short[i];
        assert(csafe_add(c, SETUSERCFG1, a) == 0);
    }

    assert(csafe_get_frame(c, buffer, &len) == 0);

    dump_c_string(__FUNCTION__, buffer, len);
}

void query_reset(void) {
    unsigned char buffer[CSAFE_MAX_FRAME_SIZE_HID] = { 0 };
    unsigned int len;

    csafe_t csafe_obj;
    csafe_vars_req_t csafe_vars = { 0 };

    csafe_t *c = &csafe_obj;
    csafe_vars_req_t *a = &csafe_vars;

    assert(csafe_init(c) == 0);
    assert(csafe_add(c, RESET, a) == 0);
    assert(csafe_get_frame(c, buffer, &len) == 0);

    dump_c_string(__FUNCTION__, buffer, len);
}

static void create_frame(const char *title,
        unsigned char *short_cmds, unsigned int num_short_cmds,
        unsigned char *short_pm3_cmds, unsigned int num_short_pm3_cmds) {
    unsigned char buffer[CSAFE_MAX_FRAME_SIZE_HID] = { 0 };
    unsigned int len;
    unsigned int i;

    csafe_t csafe_obj;
    csafe_vars_req_t csafe_vars = { 0 };

    csafe_t *c = &csafe_obj;
    csafe_vars_req_t *a = &csafe_vars;

    assert(csafe_init(c) == 0);

    for (i = 0; i < num_short_cmds; i++) {
        assert(csafe_add(c, short_cmds[i], a) == 0);
    }

    for (i = 0; i < num_short_pm3_cmds; i++) {
        a->pm3_specific_cmd = short_pm3_cmds[i];
        assert(csafe_add(c, SETUSERCFG1, a) == 0);
    }
    assert(csafe_get_frame(c, buffer, &len) == 0);

    dump_c_string(title, buffer, len);
}

/*
 * C2 Bluetooth CID UUID 0x0031
 */
void query_0x0031(void) {
    unsigned char short_pm3_cmds[] = {
        PM_GET_WORKOUTTYPE,
        PM_GET_INTERVALTYPE,
        PM_GET_WORKOUTSTATE,
        PM_GET_WORKDISTANCE,
        PM_GET_ROWINGSTATE,
        PM_GET_STROKESTATE,
        PM_GET_WORKDISTANCE,
        /*PM_GET_WORKOUTDURATION, */        /* In C2 Bluetooth CID, but not defined? */
        PM_GET_DRAGFACTOR
    };

    create_frame(__FUNCTION__,
            NULL, 0,
            short_pm3_cmds, sizeof(short_pm3_cmds));
}

/*
 * Most of the below PM3 specific commands are missing from
 * the C2 CID spec. Forget these for now.
 */
#if 0
/*
 * C2 Bluetooth CID UUID 0x0032
 */
void query_0x0032(void) {
    unsigned char short_cmds[] = {
        GETSPEED,
    };
    unsigned char short_pm3_cmds[] = {
        PM_GET_STROKERATE,
        PM_GET_AVG_HEARTRATE,
        PM_GET_STROKE_500MPACE,
        PM_GET_TOTAL_AVG_500MPACE,
        PM_GET_RESTDISTANCE,
        PM_GET_RESTTIME
    };

    create_frame(__FUNCTION__,
            short_cmds, sizeof(short_cmds),
            short_pm3_cmds, sizeof(short_pm3_cmds));
}

/*
 * Intervals / splits
 */
void query_0x0033(void) {
    unsigned char short_pm3_cmds[] = {
        PM_GET_WORKOUTINTERVALCOUNT,
        PM_GET_TOTAL_AVG_POWER,
        PM_GET_TOTAL_AVG_CALORIES,
        PM_GET_SPLIT_AVG_500MPACE,
        PM_GET_SPLIT_AVG_POWER,
        PM_GET_SPLIT_AVG_CALORIES,
        PM_GET_LAST_SPLITTIME,
        PM_GET_LAST_SPLITDISTANCE
    };

    create_frame(__FUNCTION__,
            NULL, 0,
            short_pm3_cmds, sizeof(short_pm3_cmds) / sizeof(unsigned char));
}

/*
 * Stroke statistics
 */
void query_0x0035(void) {
    unsigned char short_pm3_cmds[] = {
        PM_GET_STROKESTATS,
    };

    create_frame(__FUNCTION__,
            NULL, 0,
            short_pm3_cmds, sizeof(short_pm3_cmds) / sizeof(unsigned char));
}

void query_0x0036(void) {
    unsigned char short_pm3_cmds[] = {
        PM_GET_STROKE_POWER,
        PM_GET_STROKE_CALORICBURNRATE,
        PM_GET_STROKESTATS
    };

    create_frame(__FUNCTION__,
            NULL, 0,
            short_pm3_cmds, sizeof(short_pm3_cmds) / sizeof(unsigned char));
}
#endif // 0

void query_workout(void) {
    unsigned char short_cmds[] = {
        GETSTATUS,
        GETTWORK,
        GETHORIZONTAL,
        GETPACE,
        GETPOWER,
        GETCALORIES,
        GETCADENCE,
        GETHRCUR
    };
    unsigned char short_pm3_cmds[] = {
        PM_GET_WORKTIME,
        PM_GET_WORKDISTANCE,
        PM_GET_DRAGFACTOR,
        PM_GET_WORKOUTTYPE,
        PM_GET_WORKOUTSTATE
    };

    create_frame(__FUNCTION__,
            short_cmds, sizeof(short_cmds),
            short_pm3_cmds, sizeof(short_pm3_cmds));
}

void query_machine(void) {
    unsigned char short_cmds[] = {
        GETVERSION,
        GETID,
        GETSERIAL,
        GETODOMETER,
        GETERRORCODE,
        GETUSERINFO,
        GETCAPS
    };

    create_frame(__FUNCTION__,
            short_cmds, sizeof(short_cmds),
            NULL, 0);
}

void query_forceplot(void) {
    unsigned char short_cmds[] = {
        GETPOWER
    };
    unsigned char short_pm3_cmds[] = {
        PM_GET_STROKESTATE,
        PM_GET_FORCEPLOTDATA
    };

    create_frame(__FUNCTION__,
            short_cmds, sizeof(short_cmds),
            short_pm3_cmds, sizeof(short_pm3_cmds));
}

void query_per_stroke(void) {
    unsigned char short_cmds[] = {
        GETPOWER
    };
    unsigned char short_pm3_cmds[] = {
        PM_GET_DRAGFACTOR,
        PM_GET_STROKESTATE,
        PM_GET_FORCEPLOTDATA 
    };

    create_frame(__FUNCTION__,
            short_cmds, sizeof(short_cmds),
            short_pm3_cmds, sizeof(short_pm3_cmds));
}

int main(void) {
    //query_workout_test_play_dont_use();

    /*
    query_versions();
    query_0x0031();
    query_workout();
    */

    /*
    query_0x0032();
    query_0x0033();
    query_0x0035();
    query_0x0036();
    */

    query_reset();
    query_machine();
    query_workout();
    query_per_stroke();

    return 0;
}


#if 0
/*
 * Hardware queries
 */
    /* C2 CID Appendix A: Short commands - GETVERSION */
    /* C2 CID Appendix A: Short commands - GETSERIAL */
    /* C2 CID Appendix A: Short commands - GETUNITS */
    /* C2 CID Appendix A: Short commands - GETODOMETER */

/*
 * Workout setup queries
 */
    /* C2 CID Appendix A: Short commands - GETID */
    /* C2 CID Appendix A: Short commands - GETPROGRAM */

/*
 * "Workout In-progress" queries
 */
    /* C2 CID Appendix A: Short commands - GETERRORCODE */
    /* C2 CID Appendix A: Short commands - GETTWORK */
    /* C2 CID Appendix A: Short commands - GETHORIZONTAL */
    /* C2 CID Appendix A: Short commands - GETCALORIES */
    /* C2 CID Appendix A: Short commands - GETPACE */
    /* C2 CID Appendix A: Short commands - GETHRCUR */
    /* C2 CID Appendix A: Short commands - GETCADENCE */
    /* C2 CID Appendix A: Short commands - GETUSERINFO */
    /* C2 CID Appendix A: Short commands - GETPOWER */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_ERRORVALUE */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKTIME */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKDISTANCE */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKOUTSTATE */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKOUTINTERVALCOUNT */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_INTERVALTYPE */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_RESTTIME */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_WORKOUTTYPE */
    /* C2 CID Appendix A: Long commands - GETCAPS */

/*
 * "Per-stroke" commands
 */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_DRAGFACTOR */
    /* C2 CID Appendix A: PM3 specific short commands - PM_GET_STROKESTATE */
    /* C2 CID Appendix A: PM3 specific long commands - PM_GET_FORCEPLOTDATA */
    /* C2 CID Appendix A: PM3 specific long commands - PM_GET_HEARTBEATDATA */
    /* C2 PM Bluetooth CID - PM_GET_STROKERATE */
    /* C2 PM Bluetooth CID - PM_GET_ROWINGSTATE */
#endif
