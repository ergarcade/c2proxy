/*
 * TODO
 */
#include "csafe.h"

#include <string.h>

#ifdef CSAFE_DEBUG
#include <stdio.h>
#endif /* CSAFE_DEBUG */
#include <stdio.h>

/*
 * Buffer manipulations.
 */
#define ADD_BYTE(C,B) C->buff[C->len++] = (B & 0xff)
#define ADD_SHORT(C,S) val2bytes(S, 2, C->buff+C->len); C->len += 2
#define ADD_INT(C,I) val2bytes(I, 4, C->buff+C->len); C->len += 4

/*
 * XXX We are consuming the buffer here. Rewrite this
 * so we preserve the buffer.
 */
#define DISCARD_BYTE(C) memmove(C->buff, C->buff+1, --C->len);
#define DISCARD_LEN DISCARD_BYTE
#define POP_BYTE(C,B) B = C->buff[0]; DISCARD_BYTE(C)
#define POP_BYTES(C,B,N) memcpy(B, C->buff, N); C->len -= N; memmove(C->buff, C->buff+N, C->len)
#define POP_SHORT(C,S) bytes2val(C->buff, 2, &S); C->len -= 2; memmove(C->buff, C->buff+2, C->len)
#define POP_INT(C,I) bytes2val(C->buff, 4, &I); C->len -= 4; memmove(C->buff, C->buff+4, C->len)
#define POP_NUM(C,I,N) bytes2val(C->buff, N, &I); C->len -= N; memmove(C->buff, C->buff+N, C->len)
#define CHK_LEN(C,N) if (C->buff[0] != N) { return -1; }

/* pack a value v into LSB -> MSB buffer of length l */
void val2bytes(unsigned int v, unsigned int l, unsigned char *b) {
    unsigned int i;
    for (i = 0; i < l; i++) {
        b[i] = (v >> (8 * i)) & 0xff;
    }
}

/* unpack LSB -> MSB buffer of length l into value v */
void bytes2val(unsigned char *b, unsigned int l, unsigned int *v) {
    unsigned int i;
    *v = 0;
    for (i = 0; i < l; i++) {
        *v += b[i] << (8 * i);
    }
}

/*
 *
 */
static int byte_stuff(unsigned char *buff, unsigned int *len) {
    unsigned char t_buff[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int i;
    unsigned char *c;
    unsigned char *e;

    for (i = 0, c = buff, e = buff + *len; c != e; c++) {
        if (*c == CSAFE_EXT_START_FLAG ||
                *c == CSAFE_START_FLAG ||
                *c == CSAFE_STOP_FLAG ||
                *c == CSAFE_BYTE_STUFF_FLAG) {
            t_buff[i++] = CSAFE_BYTE_STUFF_FLAG;
            if (i >= CSAFE_MAX_FRAME_SIZE_HID - 1) {
                return -1;
            }
            t_buff[i++] = *c - CSAFE_FRAME_START_MASK;
        } else {
            t_buff[i++] = *c;
        }

        if (i >= CSAFE_MAX_FRAME_SIZE_HID) {
            return -1;
        }
    }

    memcpy(buff, t_buff, i);        /* save doing memmove() repeatedly */
    *len = i;

    return 0;
}

/*
 *
 */
static void byte_unstuff(unsigned char *buff, unsigned int *len) {
    unsigned char t_buff[CSAFE_MAX_FRAME_SIZE_HID];
    unsigned int i;
    unsigned char *c;
    unsigned char *e;

    for (i = 0, c = buff, e = buff + *len; c != e; i++) {
        if (*c == CSAFE_BYTE_STUFF_FLAG) {
            c++;
            t_buff[i] = *c + CSAFE_FRAME_START_MASK;
        } else {
            t_buff[i] = *c;
        }
        c++;
    }

    memcpy(buff, t_buff, i);
    *len = i;
}

/*
 *
 */
unsigned char checksum(unsigned char *b, int len) {
    unsigned char cksum = 0;
    while (len--) {
        cksum ^= *b++;
    }
    return cksum;
}

/*
 *
 */
int csafe_init(csafe_t *c) {
    c->len = 0;
    return 0;
}

int csafe_vars_req_init(csafe_vars_req_t *a) {
    memset(a, 0, sizeof(csafe_vars_req_t));
    return 0;
}

int csafe_vars_resp_init(csafe_vars_resp_t *a) {
    memset(a, 0, sizeof(csafe_vars_resp_t));
    return 0;
}

/* PM3 specific commands */
static int cmd_pm_get_strokerate_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_STROKERATE);

    return 0;
}
static int cmd_pm_get_strokerate_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_strokerate);

    return 0;
}
static int cmd_pm_get_workouttype_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_WORKOUTTYPE);

    return 0;
}
static int cmd_pm_get_workouttype_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_workout_type);

    return 0;
}
static int cmd_pm_get_dragfactor_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_DRAGFACTOR);

    return 0;
}
static int cmd_pm_get_dragfactor_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_drag_factor);

    return 0;
}
static int cmd_pm_get_strokestate_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_STROKESTATE);

    return 0;
}
static int cmd_pm_get_strokestate_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);

    a->pm_get_stroke_state_last = a->pm_get_stroke_state;   /* track last state */
    POP_BYTE(c, a->pm_get_stroke_state);

    return 0;
}
static int cmd_pm_get_worktime_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_WORKTIME);

    return 0;
}
static int cmd_pm_get_worktime_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 5);
    DISCARD_LEN(c);
    POP_INT(c, a->pm_get_work_time);
    POP_BYTE(c, a->pm_get_work_time_fractional);

    return 0;
}
static int cmd_pm_get_workdistance_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_WORKDISTANCE);

    return 0;
}
static int cmd_pm_get_workdistance_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 5);
    DISCARD_LEN(c);
    POP_INT(c, a->pm_get_work_distance);
    POP_BYTE(c, a->pm_get_work_distance_fractional);

    return 0;
}
static int cmd_pm_get_errorvalue_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_ERRORVALUE);

    return 0;
}
static int cmd_pm_get_errorvalue_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 2);
    DISCARD_LEN(c);
    POP_INT(c, a->pm_get_errorvalue);

    return 0;
}
static int cmd_pm_get_workoutstate_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_WORKOUTSTATE);

    return 0;
}
static int cmd_pm_get_workoutstate_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_workout_state);

    return 0;
}
static int cmd_pm_get_rowingstate_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_ROWINGSTATE);

    return 0;
}
static int cmd_pm_get_rowingstate_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_rowing_state);

    return 0;
}
static int cmd_pm_get_workoutintervalcount_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_WORKOUTINTERVALCOUNT);

    return 0;
}
static int cmd_pm_get_workoutintervalcount_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_workoutintervalcount);

    return 0;
}
static int cmd_pm_get_intervaltype_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_INTERVALTYPE);

    return 0;
}
static int cmd_pm_get_intervaltype_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->pm_get_interval_type);

    return 0;
}
static int cmd_pm_get_resttime_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, PM_GET_RESTTIME);

    return 0;
}
static int cmd_pm_get_resttime_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 2);
    DISCARD_LEN(c);
    POP_SHORT(c, a->pm_get_resttime);

    return 0;
}
static int cmd_pm_set_splitduration_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, 7); /* length of data */
    ADD_BYTE(c, PM_SET_SPLITDURATION);
    ADD_BYTE(c, 5); /* length of data */
    ADD_BYTE(c, a->set_splitduration_type);
    ADD_INT(c, a->set_splitduration_duration);

    return 0;
}
static int cmd_pm_set_splitduration_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG
    /* N/A */

    (void)c;
    (void)a;
    return 0;
}
static int cmd_pm_get_forceplotdata_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 3); /* length of data */
    ADD_BYTE(c, PM_GET_FORCEPLOTDATA);
    ADD_BYTE(c, 1); /* length of data */
    ADD_BYTE(c, FORCEPLOTDATA_BYTES);

    return 0;
}
static int cmd_pm_get_forceplotdata_resp(csafe_t *c, csafe_vars_resp_t *a) {
    unsigned char len = 0;
    unsigned char num_vals = 0;
    unsigned char bytes[FORCEPLOTDATA_BYTES] = { 0 };
    unsigned int i;

#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    /*
     * A couple of notes with the forceplot stuff:
     *
     * - C2 CID page 54: there is a footnote regarding the forceplotdata
     *   command: "... but a complete 33 bytes of response data will be
     *   returned. The first byte of the response will indicate how many valid
     *   data bytes are returned."
     *
     * - There are up to 16 values provided in for each
     *   CSAFE_PM_GET_FORCEPLOT_DATA request. According to
     *   http://www.c2forum.com/viewtopic.php?f=15&t=83, there are 144 values
     *   recorded by the PM3 for each stroke. This gives us a 9 requests to get
     *   the full forceplot data if we get a full 16 values per response. The
     *   post also mentions that we should use the stroke state to determine
     *   when the forceplot data is reset, but fails to mention what stroke
     *   state should be to trigger new data / end of old data.
     *
     * - C2 CID gives an update rate of 100Hz for CSAFE_PM_GET_STROKESTATE and
     *   per-request for CSAFE_PM_GET_FORCEPLOTDATA.
     *
     * - C2 CID says forceplot recorded every 15.625msec from catch to end of
     *   drive, equivalent to 64Hz.
     *
     * - C2 CID states for Stroke State "End-of-stroke would be the transition
     *   from driving to dwelling after drive."
     *
     * Our interrogation rate for forceplot is determined by our caller, but we
     * should note that we need at least 9 calls for
     * CSAFE_PM_GET_FORCEPLOT_DATA to populate an entire "frame" of force data.
     * How well we hit this depends on our interrogation rate and stroke time
     * of the user.
     *
     * To attempt to provide meaningful information to our caller, we'll use a
     * double buffer - one for the in-progress capture which is copied out when
     * we register a completed stroke.
     */

    CHK_LEN(c, 33);
    DISCARD_LEN(c);

    POP_BYTE(c, len);                   /* the number of valid forceplot data bytes */
    POP_BYTES(c, bytes, FORCEPLOTDATA_BYTES);   /* we always get FORCEPLOTDATA_BYTES */

    /*
     * We use pm_get_forceplotdata_len as an indicator to the caller that we
     * have completed a stroke. Always set this to zero each call to ensure we
     * don't mistakenly report a completed stroke multiple times.
     */
    a->pm_get_forceplotdata_len = 0;

    if (len) {
        /*
         * C2 CID page 54: The forceplot data is packed as LSB-MSB pairs.
         *
         * Unpack into our accumulation buffer.
         *
         * XXX Efficiency.
         */
        num_vals = len / 2;
        if (a->pm_get_forceplotdata_idx + num_vals < FORCEPLOTDATA_LENGTH) {
            for (i = 0; i < len; i += 2) {
                bytes2val(bytes+i,
                        2,
                        &a->pm_get_forceplotdata_data_cur[a->pm_get_forceplotdata_idx+(i/2)]);
            }
            a->pm_get_forceplotdata_idx += num_vals;
        }

        /*
         * If we completed a stroke, copy the data out for our caller.  Note
         * that we may miss DWELLING if we poll either side of that state
         * change.
         */
        if (((a->pm_get_stroke_state_last == STROKESTATE_DRIVING_STATE) &&
             (a->pm_get_stroke_state == STROKESTATE_DWELLING_AFTER_DRIVE_STATE)) ||
            ((a->pm_get_stroke_state_last == STROKESTATE_DRIVING_STATE) &&
             (a->pm_get_stroke_state == STROKESTATE_RECOVERY_STATE))) {
            if (a->pm_get_forceplotdata_idx != 0) {
                memset(a->pm_get_forceplotdata_data, 0, sizeof(a->pm_get_forceplotdata_data));
                memcpy(a->pm_get_forceplotdata_data,
                        a->pm_get_forceplotdata_data_cur,
                        a->pm_get_forceplotdata_idx * sizeof(a->pm_get_forceplotdata_data[0]));
                a->pm_get_forceplotdata_len = a->pm_get_forceplotdata_idx;
                a->pm_get_forceplotdata_idx = 0;
            }
        }
    }

    return 0;
}
static int cmd_pm_get_screenerrormode_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, 3); /* length of data */
    ADD_BYTE(c, PM_SET_SCREENERRORMODE);
    ADD_BYTE(c, 1); /* length of data */
    ADD_BYTE(c, a->set_screenerrormode_mode);

    return 0;
}
static int cmd_pm_get_screenerrormode_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* N/A */
    return 0;
}
static int cmd_pm_get_heartbeatdata_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;

    ADD_BYTE(c, 3); /* length of data */
    ADD_BYTE(c, PM_GET_HEARTBEATDATA);
    ADD_BYTE(c, 1); /* length of data */
    ADD_BYTE(c, HEARTBEATDATA_BYTES);

    return 0;
}
static int cmd_pm_get_heartbeatdata_resp(csafe_t *c, csafe_vars_resp_t *a) {
    unsigned char len;
    unsigned char bytes[HEARTBEATDATA_BYTES];
    unsigned int i;

#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    /*
     * C2 CID page 54: there is a footnote regarding the heartbeatdata
     * command: "... but a complete 33 bytes of response data will
     * be returned... The first byte of the response will indicate how
     * many valid data bytes are returned."
     *
     * Ignore the length bytes, grab everything.
     */
    POP_BYTE(c, len);   /* bytes read + number of forceplot bytes */
    POP_BYTE(c, len);   /* number of forceplot bytes (XXX check) */
    POP_BYTES(c, bytes, HEARTBEATDATA_BYTES);  /* hearbeat data */

    /*
     * C2 CID page 54: The forceplot data is packed as LSB-MSB pairs.
     * Unpack this into our structure.
     */
    for (i = 0; i < len; i += 2) {
        bytes2val(bytes, 2, &a->pm_get_heartbeatdata_data[i/2]);
    }
    a->pm_get_heartbeatdata_len = len / 2;

    return 0;
}

typedef int (*csafe_handler_req_t)(csafe_t*, csafe_vars_req_t*);
typedef int (*csafe_handler_resp_t)(csafe_t*, csafe_vars_resp_t*);
typedef struct csafe_handler_st {
    csafe_handler_req_t req;
    csafe_handler_resp_t resp;
} csafe_handler_t;

static csafe_handler_t csafe_pm3_cmds[] = {
    /* 0x00 */ { NULL,                  NULL },
    /* 0x01 */ { NULL,                  NULL },
    /* 0x02 */ { NULL,                  NULL },
    /* 0x03 */ { NULL,                  NULL },
    /* 0x04 */ { NULL,                  NULL },
    /* 0x05 */ { cmd_pm_set_splitduration_req, cmd_pm_set_splitduration_resp },
    /* 0x06 */ { NULL,                  NULL },
    /* 0x07 */ { NULL,                  NULL },
    /* 0x08 */ { NULL,                  NULL },
    /* 0x09 */ { NULL,                  NULL },
    /* 0x0a */ { NULL,                  NULL },
    /* 0x0b */ { NULL,                  NULL },
    /* 0x0c */ { NULL,                  NULL },
    /* 0x0d */ { NULL,                  NULL },
    /* 0x0e */ { NULL,                  NULL },
    /* 0x0f */ { NULL,                  NULL },
    /* 0x10 */ { NULL,                  NULL },
    /* 0x11 */ { NULL,                  NULL },
    /* 0x12 */ { NULL,                  NULL },
    /* 0x13 */ { NULL,                  NULL },
    /* 0x14 */ { NULL,                  NULL },
    /* 0x15 */ { NULL,                  NULL },
    /* 0x16 */ { NULL,                  NULL },
    /* 0x17 */ { NULL,                  NULL },
    /* 0x18 */ { NULL,                  NULL },
    /* 0x19 */ { NULL,                  NULL },
    /* 0x1a */ { NULL,                  NULL },
    /* 0x1b */ { NULL,                  NULL },
    /* 0x1c */ { NULL,                  NULL },
    /* 0x1d */ { NULL,                  NULL },
    /* 0x1e */ { NULL,                  NULL },
    /* 0x1f */ { NULL,                  NULL },
    /* 0x20 */ { NULL,                  NULL },
    /* 0x21 */ { NULL,                  NULL },
    /* 0x22 */ { NULL,                  NULL },
    /* 0x23 */ { NULL,                  NULL },
    /* 0x24 */ { NULL,                  NULL },
    /* 0x25 */ { NULL,                  NULL },
    /* 0x26 */ { NULL,                  NULL },
    /* 0x27 */ { cmd_pm_get_screenerrormode_req, cmd_pm_get_screenerrormode_resp },
    /* 0x28 */ { NULL,                  NULL },
    /* 0x29 */ { NULL,                  NULL },
    /* 0x2a */ { NULL,                  NULL },
    /* 0x2b */ { NULL,                  NULL },
    /* 0x2c */ { NULL,                  NULL },
    /* 0x2d */ { NULL,                  NULL },
    /* 0x2e */ { NULL,                  NULL },
    /* 0x2f */ { NULL,                  NULL },
    /* 0x30 */ { NULL,                  NULL },
    /* 0x31 */ { NULL,                  NULL },
    /* 0x32 */ { NULL,                  NULL },
    /* 0x33 */ { NULL,                  NULL },
    /* 0x34 */ { NULL,                  NULL },
    /* 0x35 */ { NULL,                  NULL },
    /* 0x36 */ { NULL,                  NULL },
    /* 0x37 */ { NULL,                  NULL },
    /* 0x38 */ { NULL,                  NULL },
    /* 0x39 */ { NULL,                  NULL },
    /* 0x3a */ { NULL,                  NULL },
    /* 0x3b */ { NULL,                  NULL },
    /* 0x3c */ { NULL,                  NULL },
    /* 0x3d */ { NULL,                  NULL },
    /* 0x3e */ { NULL,                  NULL },
    /* 0x3f */ { NULL,                  NULL },
    /* 0x40 */ { NULL,                  NULL },
    /* 0x41 */ { NULL,                  NULL },
    /* 0x42 */ { NULL,                  NULL },
    /* 0x43 */ { NULL,                  NULL },
    /* 0x44 */ { NULL,                  NULL },
    /* 0x45 */ { NULL,                  NULL },
    /* 0x46 */ { NULL,                  NULL },
    /* 0x47 */ { NULL,                  NULL },
    /* 0x48 */ { NULL,                  NULL },
    /* 0x49 */ { NULL,                  NULL },
    /* 0x4a */ { NULL,                  NULL },
    /* 0x4b */ { NULL,                  NULL },
    /* 0x4c */ { NULL,                  NULL },
    /* 0x4d */ { NULL,                  NULL },
    /* 0x4e */ { NULL,                  NULL },
    /* 0x4f */ { NULL,                  NULL },
    /* 0x50 */ { NULL,                  NULL },
    /* 0x51 */ { NULL,                  NULL },
    /* 0x52 */ { NULL,                  NULL },
    /* 0x53 */ { NULL,                  NULL },
    /* 0x54 */ { NULL,                  NULL },
    /* 0x55 */ { NULL,                  NULL },
    /* 0x56 */ { NULL,                  NULL },
    /* 0x57 */ { NULL,                  NULL },
    /* 0x58 */ { NULL,                  NULL },
    /* 0x59 */ { NULL,                  NULL },
    /* 0x5a */ { NULL,                  NULL },
    /* 0x5b */ { NULL,                  NULL },
    /* 0x5c */ { NULL,                  NULL },
    /* 0x5d */ { NULL,                  NULL },
    /* 0x5e */ { NULL,                  NULL },
    /* 0x5f */ { NULL,                  NULL },
    /* 0x60 */ { NULL,                  NULL },
    /* 0x61 */ { NULL,                  NULL },
    /* 0x62 */ { NULL,                  NULL },
    /* 0x63 */ { NULL,                  NULL },
    /* 0x64 */ { NULL,                  NULL },
    /* 0x65 */ { NULL,                  NULL },
    /* 0x66 */ { NULL,                  NULL },
    /* 0x67 */ { NULL,                  NULL },
    /* 0x68 */ { NULL,                  NULL },
    /* 0x69 */ { NULL,                  NULL },
    /* 0x6a */ { NULL,                  NULL },
    /* 0x6b */ { cmd_pm_get_forceplotdata_req, cmd_pm_get_forceplotdata_resp },
    /* 0x6c */ { cmd_pm_get_heartbeatdata_req, cmd_pm_get_heartbeatdata_resp },
    /* 0x6d */ { NULL,                  NULL },
    /* 0x6e */ { NULL,                  NULL },
    /* 0x6f */ { NULL,                  NULL },
    /* 0x70 */ { NULL,                  NULL },
    /* 0x71 */ { NULL,                  NULL },
    /* 0x72 */ { NULL,                  NULL },
    /* 0x73 */ { NULL,                  NULL },
    /* 0x74 */ { NULL,                  NULL },
    /* 0x75 */ { NULL,                  NULL },
    /* 0x76 */ { NULL,                  NULL },
    /* 0x77 */ { NULL,                  NULL },
    /* 0x78 */ { NULL,                  NULL },
    /* 0x79 */ { NULL,                  NULL },
    /* 0x7a */ { NULL,                  NULL },
    /* 0x7b */ { NULL,                  NULL },
    /* 0x7c */ { NULL,                  NULL },
    /* 0x7d */ { NULL,                  NULL },
    /* 0x7e */ { NULL,                  NULL },
    /* 0x7f */ { NULL,                  NULL },

    /* 0x80 */ { NULL,                  NULL },
    /* 0x81 */ { NULL,                  NULL },
    /* 0x82 */ { NULL,                  NULL },
    /* 0x83 */ { cmd_pm_get_strokerate_req, cmd_pm_get_strokerate_resp },
    /* 0x84 */ { NULL,                  NULL },
    /* 0x85 */ { NULL,                  NULL },
    /* 0x86 */ { NULL,                  NULL },
    /* 0x87 */ { NULL,                  NULL },
    /* 0x88 */ { NULL,                  NULL },
    /* 0x89 */ { cmd_pm_get_workouttype_req, cmd_pm_get_workouttype_resp },
    /* 0x8a */ { NULL,                  NULL },
    /* 0x8b */ { NULL,                  NULL },
    /* 0x8c */ { NULL,                  NULL },
    /* 0x8d */ { cmd_pm_get_workoutstate_req, cmd_pm_get_workoutstate_resp },
    /* 0x8e */ { cmd_pm_get_intervaltype_req, cmd_pm_get_intervaltype_resp },
    /* 0x8f */ { NULL,                  NULL },
    /* 0x90 */ { NULL,                  NULL },
    /* 0x91 */ { NULL,                  NULL },
    /* 0x92 */ { NULL,                  NULL },
    /* 0x93 */ { cmd_pm_get_rowingstate_req, cmd_pm_get_rowingstate_resp },
    /* 0x94 */ { NULL,                  NULL },
    /* 0x95 */ { NULL,                  NULL },
    /* 0x96 */ { NULL,                  NULL },
    /* 0x97 */ { NULL,                  NULL },
    /* 0x98 */ { NULL,                  NULL },
    /* 0x99 */ { NULL,                  NULL },
    /* 0x9a */ { NULL,                  NULL },
    /* 0x9b */ { NULL,                  NULL },
    /* 0x9c */ { NULL,                  NULL },
    /* 0x9d */ { NULL,                  NULL },
    /* 0x9e */ { NULL,                  NULL },
    /* 0x9f */ { cmd_pm_get_workoutintervalcount_req, cmd_pm_get_workoutintervalcount_resp },
    /* 0xa0 */ { cmd_pm_get_worktime_req, cmd_pm_get_worktime_resp },
    /* 0xa1 */ { NULL,                  NULL },
    /* 0xa2 */ { NULL,                  NULL },
    /* 0xa3 */ { cmd_pm_get_workdistance_req, cmd_pm_get_workdistance_resp },
    /* 0xa4 */ { NULL,                  NULL },
    /* 0xa5 */ { NULL,                  NULL },
    /* 0xa6 */ { NULL,                  NULL },
    /* 0xa7 */ { NULL,                  NULL },
    /* 0xa8 */ { NULL,                  NULL },
    /* 0xa9 */ { NULL,                  NULL },
    /* 0xaa */ { NULL,                  NULL },
    /* 0xab */ { NULL,                  NULL },
    /* 0xac */ { NULL,                  NULL },
    /* 0xad */ { NULL,                  NULL },
    /* 0xae */ { NULL,                  NULL },
    /* 0xaf */ { NULL,                  NULL },
    /* 0xb0 */ { NULL,                  NULL },
    /* 0xb1 */ { NULL,                  NULL },
    /* 0xb2 */ { NULL,                  NULL },
    /* 0xb3 */ { NULL,                  NULL },
    /* 0xb4 */ { NULL,                  NULL },
    /* 0xb5 */ { NULL,                  NULL },
    /* 0xb6 */ { NULL,                  NULL },
    /* 0xb7 */ { NULL,                  NULL },
    /* 0xb8 */ { NULL,                  NULL },
    /* 0xb9 */ { NULL,                  NULL },
    /* 0xba */ { NULL,                  NULL },
    /* 0xbb */ { NULL,                  NULL },
    /* 0xbc */ { NULL,                  NULL },
    /* 0xbd */ { NULL,                  NULL },
    /* 0xbe */ { NULL,                  NULL },
    /* 0xbf */ { cmd_pm_get_strokestate_req, cmd_pm_get_strokestate_resp },
    /* 0xc0 */ { NULL,                  NULL },
    /* 0xc1 */ { cmd_pm_get_dragfactor_req, cmd_pm_get_dragfactor_resp },
    /* 0xc2 */ { NULL,                  NULL },
    /* 0xc3 */ { NULL,                  NULL },
    /* 0xc4 */ { NULL,                  NULL },
    /* 0xc5 */ { NULL,                  NULL },
    /* 0xc6 */ { NULL,                  NULL },
    /* 0xc7 */ { NULL,                  NULL },
    /* 0xc8 */ { NULL,                  NULL },
    /* 0xc9 */ { cmd_pm_get_errorvalue_req, cmd_pm_get_errorvalue_resp },
    /* 0xca */ { NULL,                  NULL },
    /* 0xcb */ { NULL,                  NULL },
    /* 0xcc */ { NULL,                  NULL },
    /* 0xcd */ { NULL,                  NULL },
    /* 0xce */ { NULL,                  NULL },
    /* 0xcf */ { cmd_pm_get_resttime_req, cmd_pm_get_resttime_resp },
    /* 0xd0 */ { NULL,                  NULL },
    /* 0xd1 */ { NULL,                  NULL },
    /* 0xd2 */ { NULL,                  NULL },
    /* 0xd3 */ { NULL,                  NULL },
    /* 0xd4 */ { NULL,                  NULL },
    /* 0xd5 */ { NULL,                  NULL },
    /* 0xd6 */ { NULL,                  NULL },
    /* 0xd7 */ { NULL,                  NULL },
    /* 0xd8 */ { NULL,                  NULL },
    /* 0xd9 */ { NULL,                  NULL },
    /* 0xda */ { NULL,                  NULL },
    /* 0xdb */ { NULL,                  NULL },
    /* 0xdc */ { NULL,                  NULL },
    /* 0xdd */ { NULL,                  NULL },
    /* 0xde */ { NULL,                  NULL },
    /* 0xdf */ { NULL,                  NULL },
    /* 0xe0 */ { NULL,                  NULL },
    /* 0xe1 */ { NULL,                  NULL },
    /* 0xe2 */ { NULL,                  NULL },
    /* 0xe3 */ { NULL,                  NULL },
    /* 0xe4 */ { NULL,                  NULL },
    /* 0xe5 */ { NULL,                  NULL },
    /* 0xe6 */ { NULL,                  NULL },
    /* 0xe7 */ { NULL,                  NULL },
    /* 0xe8 */ { NULL,                  NULL },
    /* 0xe9 */ { NULL,                  NULL },
    /* 0xea */ { NULL,                  NULL },
    /* 0xeb */ { NULL,                  NULL },
    /* 0xec */ { NULL,                  NULL },
    /* 0xed */ { NULL,                  NULL },
    /* 0xee */ { NULL,                  NULL },
    /* 0xef */ { NULL,                  NULL },
    /* 0xf0 */ { NULL,                  NULL },
    /* 0xf1 */ { NULL,                  NULL },
    /* 0xf2 */ { NULL,                  NULL },
    /* 0xf3 */ { NULL,                  NULL },
    /* 0xf4 */ { NULL,                  NULL },
    /* 0xf5 */ { NULL,                  NULL },
    /* 0xf6 */ { NULL,                  NULL },
    /* 0xf7 */ { NULL,                  NULL },
    /* 0xf8 */ { NULL,                  NULL },
    /* 0xf9 */ { NULL,                  NULL },
    /* 0xfa */ { NULL,                  NULL },
    /* 0xfb */ { NULL,                  NULL },
    /* 0xfc */ { NULL,                  NULL },
    /* 0xfd */ { NULL,                  NULL },
    /* 0xfe */ { NULL,                  NULL },
    /* 0xff */ { NULL,                  NULL }
};

static int cmd_getstatus_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETSTATUS);

    return 0;
}
static int cmd_getstatus_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->status);

    return 0;
}
static int cmd_reset_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, RESET);

    return 0;
}
static int cmd_reset_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* No data returned. */

    return 0;
}
static int cmd_goidle_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GOIDLE);

    return 0;
}
static int cmd_goidle_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* No data returned. */
    return 0;
}
static int cmd_gohaveid_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GOHAVEID);

    return 0;
}
static int cmd_gohaveid_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* No data returned. */
    return 0;
}
static int cmd_goinuse_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GOINUSE);

    return 0;
}
static int cmd_goinuse_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* No data returned. */

    return 0;
}
static int cmd_gofinished_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GOFINISHED);

    return 0;
}
static int cmd_gofinished_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;
    /* No data returned. */

    return 0;
}
static int cmd_goready_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GOREADY);

    return 0;
}
static int cmd_goready_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* No data returned. */

    return 0;
}
static int cmd_badid_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, BADID);

    return 0;
}
static int cmd_badid_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* No data returned. */

    return 0;
}
static int cmd_getversion_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETVERSION);

    return 0;
}
static int cmd_getversion_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 7);
    DISCARD_LEN(c);
    POP_BYTE(c, a->getversion_mfg_id);
    POP_BYTE(c, a->getversion_cid);
    POP_BYTE(c, a->getversion_model);
    POP_SHORT(c, a->getversion_hw_version);
    POP_SHORT(c, a->getversion_sw_version);

    return 0;
}
static int cmd_getid_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETID);

    return 0;
}
static int cmd_getid_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 5);
    DISCARD_LEN(c);
    POP_BYTE(c, a->getid_id[0]);
    POP_BYTE(c, a->getid_id[1]);
    POP_BYTE(c, a->getid_id[2]);
    POP_BYTE(c, a->getid_id[3]);
    POP_BYTE(c, a->getid_id[4]);

    return 0;
}
static int cmd_getunits_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETUNITS);

    return 0;
}
static int cmd_getunits_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->getunits_units_type);

    return 0;
}
static int cmd_getserial_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETSERIAL);

    return 0;
}
static int cmd_getserial_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 9);
    DISCARD_LEN(c);
    POP_BYTE(c, a->getserial_serial_number[0]);
    POP_BYTE(c, a->getserial_serial_number[1]);
    POP_BYTE(c, a->getserial_serial_number[2]);
    POP_BYTE(c, a->getserial_serial_number[3]);
    POP_BYTE(c, a->getserial_serial_number[4]);
    POP_BYTE(c, a->getserial_serial_number[5]);
    POP_BYTE(c, a->getserial_serial_number[6]);
    POP_BYTE(c, a->getserial_serial_number[7]);
    POP_BYTE(c, a->getserial_serial_number[8]);

    return 0;
}
static int cmd_getlist_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETLIST); */

    return 0;
}
static int cmd_getlist_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getutilization_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETUTILIZATION); */

    return 0;
}
static int cmd_getutilization_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getmotorcurrent_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETMOTORCURRENT); */

    return 0;
}
static int cmd_getmotorcurrent_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getodometer_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETODOMETER);

    return 0;
}
static int cmd_getodometer_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 5);
    DISCARD_LEN(c);
    POP_INT(c, a->getodometer_odometer);
    POP_BYTE(c, a->getodometer_units_specifier);

    return 0;
}
static int cmd_geterrorcode_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETERRORCODE);

    return 0;
}
static int cmd_geterrorcode_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 3);
    DISCARD_LEN(c);
    POP_NUM(c, a->geterrorcode_error_code, 3);

    return 0;
}
static int cmd_getservicecode_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETSERVICECODE); */

    return 0;
}
static int cmd_getservicecode_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getusercfg1_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETUSERCFG1); */

    return 0;
}
static int cmd_getusercfg1_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getusercfg2_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETUSERCFG2); */

    return 0;
}
static int cmd_getusercfg2_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_gettwork_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETTWORK);

    return 0;
}
static int cmd_gettwork_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 3);
    DISCARD_LEN(c);
    POP_BYTE(c, a->gettwork_hours);
    POP_BYTE(c, a->gettwork_minutes);
    POP_BYTE(c, a->gettwork_seconds);

    return 0;
}
static int cmd_gethorizontal_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETHORIZONTAL);

    return 0;
}
static int cmd_gethorizontal_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 3);
    DISCARD_LEN(c);
    POP_SHORT(c, a->gethorizontal_distance);
    POP_BYTE(c, a->gethorizontal_distance_units_specifier);

    return 0;
}
static int cmd_getvertical_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETVERTICAL); */

    return 0;
}
static int cmd_getvertical_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getcalories_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETCALORIES);

    return 0;
}
static int cmd_getcalories_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 2);
    DISCARD_LEN(c);
    POP_SHORT(c, a->getcalories_total_calories);

    return 0;
}
static int cmd_getprogram_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETPROGRAM);

    return 0;
}
static int cmd_getprogram_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->getprogram_workout_number);

    return 0;
}
static int cmd_getspeed_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETSPEED); */

    return 0;
}
static int cmd_getspeed_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getpace_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETPACE);

    return 0;
}
static int cmd_getpace_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 3);
    DISCARD_LEN(c);
    POP_SHORT(c, a->getpace_stroke_pace);
    POP_BYTE(c, a->getpace_units_specifier);

    return 0;
}
static int cmd_getcadence_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETCADENCE);

    return 0;
}
static int cmd_getcadence_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 3);
    DISCARD_LEN(c);
    POP_SHORT(c, a->getcadence_stroke_rate);
    POP_BYTE(c, a->getcadence_units_specifier);

    return 0;
}
static int cmd_getgrade_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETGRADE); */

    return 0;
}
static int cmd_getgrade_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getgear_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETGEAR); */

    return 0;
}
static int cmd_getgear_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getuplist_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETUPLIST); */

    return 0;
}
static int cmd_getuplist_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getuserinfo_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETUSERINFO);

    return 0;
}
static int cmd_getuserinfo_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 5);
    DISCARD_LEN(c);
    POP_SHORT(c, a->getuserinfo_weight);
    POP_BYTE(c, a->getuserinfo_weight_units_specifier);
    POP_BYTE(c, a->getuserinfo_age);
    POP_BYTE(c, a->getuserinfo_gender);

    return 0;
}
static int cmd_gettorque_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETTORQUE); */

    return 0;
}
static int cmd_gettorque_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_gethrcur_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETHRCUR);
    
    return 0;
}
static int cmd_gethrcur_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 1);
    DISCARD_LEN(c);
    POP_BYTE(c, a->gethrcur_bpm);

    return 0;
}
static int cmd_gethrtzone_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETHRTZONE); */

    return 0;
}
static int cmd_gethrtzone_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getmets_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETMETS); */

    return 0;
}
static int cmd_getmets_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getpower_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    ADD_BYTE(c, GETPOWER);

    return 0;
}
static int cmd_getpower_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    CHK_LEN(c, 3);
    DISCARD_LEN(c);
    POP_SHORT(c, a->getpower_stroke_watts);
    POP_BYTE(c, a->getpower_units_specifier);

    return 0;
}
static int cmd_gethravg_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETHRAVG); */

    return 0;
}
static int cmd_gethravg_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_gethrmax_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETHRMAX); */

    return 0;
}
static int cmd_gethrmax_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getuserdata1_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETUSERDATA1); */

    return 0;
}
static int cmd_getuserdata1_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getuserdata2_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETUSERDATA2); */

    return 0;
}
static int cmd_getuserdata2_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getaudiochannel_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETAUDIOCHANNEL); */

    return 0;
}
static int cmd_getaudiochannel_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getaudiovolume_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETAUDIOVOLUME); */

    return 0;
}
static int cmd_getaudiovolume_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getaudiomute_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETAUDIOMUTE); */

    return 0;
}
static int cmd_getaudiomute_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_endtext_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, ENDTEXT); */

    return 0;
}
static int cmd_endtext_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_displaypopup_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, DISPLAYPOPUP); */

    return 0;
}
static int cmd_displaypopup_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_getpopupstatus_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETPOPUPSTATUS); */

    return 0;
}
static int cmd_getpopupstatus_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}

/* Long commands */
static int cmd_autoupload_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, AUTOUPLOAD);
    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, a->autoupload_configuration);

    return 0;
}
static int cmd_autoupload_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_uplist_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, UPLIST); */

    return 0;
}
static int cmd_uplist_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_upstatussec_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, UPSTATUSSEC); */

    return 0;
}
static int cmd_upstatussec_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_uplistsec_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, UPLISTSEC); */

    return 0;
}
static int cmd_uplistsec_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_iddigits_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, IDDIGITS);
    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, a->iddigits_num);

    return 0;
}
static int cmd_iddigits_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_settime_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG
    ADD_BYTE(c, SETTIME);
    ADD_BYTE(c, 3); /* length */
    ADD_BYTE(c, a->settime_hour);
    ADD_BYTE(c, a->settime_minute);
    ADD_BYTE(c, a->settime_second);
    return 0;
}
static int cmd_settime_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_setdate_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETDATE);
    ADD_BYTE(c, 3); /* length */
    ADD_BYTE(c, a->setdate_year);
    ADD_BYTE(c, a->setdate_month);
    ADD_BYTE(c, a->setdate_day);

    return 0;
}
static int cmd_setdate_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_settimeout_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETTIMEOUT);
    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, a->settimeout_state_timeout);

    return 0;
}
static int cmd_settimeout_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_setusercfg1_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    if (!csafe_pm3_cmds[a->pm3_specific_cmd].req) {
       return -1;
    }

    ADD_BYTE(c, SETUSERCFG1);

    return csafe_pm3_cmds[a->pm3_specific_cmd].req(c, a);
}
static int cmd_setusercfg1_resp(csafe_t *c, csafe_vars_resp_t *a) {
    unsigned char cmd;

#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    DISCARD_LEN(c);
    POP_BYTE(c, cmd);

    if (!csafe_pm3_cmds[cmd].resp) {
        return -1;
    }

    return csafe_pm3_cmds[cmd].resp(c, a);
}
static int cmd_setusercfg2_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETUSERCFG2); */

    return 0;
}
static int cmd_setusercfg2_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_settwork_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETTWORK);
    ADD_BYTE(c, 3);
    ADD_BYTE(c, a->settwork_hours);
    ADD_BYTE(c, a->settwork_minutes);
    ADD_BYTE(c, a->settwork_seconds);

    return 0;
}
static int cmd_settwork_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_sethorizontal_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETHORIZONTAL);
    ADD_BYTE(c, 3);
    ADD_SHORT(c, a->sethorizontal_distance);
    ADD_BYTE(c, a->sethorizontal_distance_units_specifier);

    return 0;
}
static int cmd_sethorizontal_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_setvertical_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETVERTICAL); */

    return 0;
}
static int cmd_setvertical_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_setcalories_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETCALORIES);
    ADD_BYTE(c, 2); /* length */
    ADD_SHORT(c, a->setcalories_total_calories);

    return 0;
}
static int cmd_setcalories_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */

    return 0;
}
static int cmd_setprogram_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETPROGRAM);
    ADD_BYTE(c, 2); /* length */
    ADD_BYTE(c, a->setprogram_program_number);
    ADD_BYTE(c, 0);                     /* spec says don't care */

    return 0;
}
static int cmd_setprogram_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setspeed_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETSPEED); */
    return 0;
}
static int cmd_setspeed_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setgrade_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETGRADE); */
    return 0;
}
static int cmd_setgrade_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setgear_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETGEAR); */
    return 0;
}
static int cmd_setgear_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setuserinfo_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETUSERINFO); */
    return 0;
}
static int cmd_setuserinfo_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_settorque_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETTORQUE); */

    return 0;
}
static int cmd_settorque_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setlevel_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETLEVEL); */
    return 0;
}
static int cmd_setlevel_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_settargethr_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETTARGETHR); */
    return 0;
}
static int cmd_settargethr_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setmets_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETMETS); */
    return 0;
}
static int cmd_setmets_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setpower_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, SETPOWER);
    ADD_BYTE(c, 3); /* length */
    ADD_SHORT(c, a->setpower_stroke_watts);
    ADD_BYTE(c, a->setpower_stroke_watts_units_specifier);

    return 0;
}
static int cmd_setpower_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_sethrzone_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETHRZONE); */
    return 0;
}
static int cmd_sethrzone_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_sethrmax_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETHRMAX); */
    return 0;
}
static int cmd_sethrmax_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setchannelrange_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETCHANNELRANGE); */
    return 0;
}
static int cmd_setchannelrange_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setvolumerange_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETVOLUMERANGE); */
    return 0;
}
static int cmd_setvolumerange_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setaudiomute_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETAUDIOMUTE); */
    return 0;
}
static int cmd_setaudiomute_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setaudiochannel_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETAUDIOCHANNEL); */
    return 0;
}
static int cmd_setaudiochannel_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_setaudiovolume_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETAUDIOVOLUME); */
    return 0;
}
static int cmd_setaudiovolume_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_starttext_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, STARTTEXT); */
    return 0;
}
static int cmd_starttext_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_appendtext_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, APPENDTEXT); */
    return 0;
}
static int cmd_appendtext_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_gettextstatus_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETTEXTSTATUS); */
    return 0;
}
static int cmd_gettextstatus_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* Not implemented. */
    return 0;
}
static int cmd_getcaps_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    ADD_BYTE(c, GETCAPS);
    ADD_BYTE(c, 1); /* length */
    ADD_BYTE(c, a->getcaps_capability_code);

    return 0;
}
static int cmd_getcaps_resp(csafe_t *c, csafe_vars_resp_t *a) {
    unsigned char len;

#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    POP_BYTE(c, len);

    /* XXX we aren't told from the PM what capability code we asked for RARGH
     * This means we have to carry state into this response function, which
     * isn't catered for. Bah.
     * 
     * Probably best to ignore this one for now; there's nothing that we don't
     * already know from this function.  Assume we always ask for cap code
     * zero.
     *
     * XXX currently using length to determine capability code that was asked
     * for.
     */
    if (len == 3) {
        POP_BYTE(c, a->getcaps_max_rx_frame);
        POP_BYTE(c, a->getcaps_max_tx_frame);
        POP_BYTE(c, a->getcaps_min_interframe);
    } else if (len == 4) {
        /*
         * XXX C2 CID spec says 2 bytes (page 52).
         * XXX CSAFE spec says 4 bytes (page 25).
         * Need to test to confirm.
         */
        POP_BYTES(c, a->getcaps_capcode_0x01, 4);
    } else if (len == 11) {
        POP_BYTES(c, a->getcaps_capcode_0x02, 11);
    } else {
        return -1;
    }

    return 0;
}
static int cmd_setpmcfg_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETPMCFG); */
    return 0;
}
static int cmd_setpmcfg_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* C2 CID Page 52: PM3 proprietary commands */
    return 0;
}
static int cmd_setpmdata_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, SETPMDATA); */
    return 0;
}
static int cmd_setpmdata_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* C2 CID Page 52: PM3 proprietary commands */
    return 0;
}
static int cmd_getpmcfg_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETPMCFG); */
    return 0;
}
static int cmd_getpmcfg_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* C2 CID Page 52: PM3 proprietary commands */
    return 0;
}
static int cmd_getpmdata_req(csafe_t *c, csafe_vars_req_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* ADD_BYTE(c, GETPMDATA); */
    return 0;
}
static int cmd_getpmdata_resp(csafe_t *c, csafe_vars_resp_t *a) {
#ifdef CSAFE_DEBUG
    fprintf(stderr, "%s\n", __FUNCTION__);
#endif // CSAFE_DEBUG

    (void)a;
    (void)c;

    /* C2 CID Page 52: PM3 proprietary commands */
    return 0;
}

/* Normal commands. */
static csafe_handler_t csafe_cmds[] = {
    /* Long commands. */
    /* 0x00 */ { NULL,                  NULL },
    /* 0x01 */ { cmd_autoupload_req,    cmd_autoupload_resp },
    /* 0x02 */ { cmd_uplist_req,        cmd_uplist_resp },
    /* 0x03 */ { NULL,                  NULL },
    /* 0x04 */ { cmd_upstatussec_req,   cmd_upstatussec_resp },
    /* 0x05 */ { cmd_uplistsec_req,     cmd_uplistsec_resp },
    /* 0x06 */ { NULL,                  NULL },
    /* 0x07 */ { NULL,                  NULL },
    /* 0x08 */ { NULL,                  NULL },
    /* 0x09 */ { NULL,                  NULL },
    /* 0x0a */ { NULL,                  NULL },
    /* 0x0b */ { NULL,                  NULL },
    /* 0x0c */ { NULL,                  NULL },
    /* 0x0d */ { NULL,                  NULL },
    /* 0x0e */ { NULL,                  NULL },
    /* 0x0f */ { NULL,                  NULL },
    /* 0x10 */ { cmd_iddigits_req,      cmd_iddigits_resp },
    /* 0x11 */ { cmd_settime_req,       cmd_settime_resp },
    /* 0x12 */ { cmd_setdate_req,       cmd_setdate_resp },
    /* 0x13 */ { cmd_settimeout_req,    cmd_settimeout_resp },
    /* 0x14 */ { NULL,                  NULL },
    /* 0x15 */ { NULL,                  NULL },
    /* 0x16 */ { NULL,                  NULL },
    /* 0x17 */ { NULL,                  NULL },
    /* 0x18 */ { NULL,                  NULL },
    /* 0x19 */ { NULL,                  NULL },
    /* 0x1a */ { cmd_setusercfg1_req,   cmd_setusercfg1_resp },
    /* 0x1b */ { cmd_setusercfg2_req,   cmd_setusercfg2_resp },
    /* 0x1c */ { NULL,                  NULL },
    /* 0x1d */ { NULL,                  NULL },
    /* 0x1e */ { NULL,                  NULL },
    /* 0x1f */ { NULL,                  NULL },
    /* 0x20 */ { cmd_settwork_req,      cmd_settwork_resp },
    /* 0x21 */ { cmd_sethorizontal_req, cmd_sethorizontal_resp },
    /* 0x22 */ { cmd_setvertical_req,   cmd_setvertical_resp },
    /* 0x23 */ { cmd_setcalories_req,   cmd_setcalories_resp },
    /* 0x24 */ { cmd_setprogram_req,    cmd_setprogram_resp },
    /* 0x25 */ { cmd_setspeed_req,      cmd_setspeed_resp },
    /* 0x26 */ { NULL,                  NULL },
    /* 0x27 */ { NULL,                  NULL },
    /* 0x28 */ { cmd_setgrade_req,      cmd_setgrade_resp },
    /* 0x29 */ { cmd_setgear_req,       cmd_setgear_resp },
    /* 0x2a */ { NULL,                  NULL },
    /* 0x2b */ { cmd_setuserinfo_req,   cmd_setuserinfo_resp },
    /* 0x2c */ { cmd_settorque_req,     cmd_settorque_resp },
    /* 0x2d */ { cmd_setlevel_req,      cmd_setlevel_resp },
    /* 0x2e */ { NULL,                  NULL },
    /* 0x2f */ { NULL,                  NULL },
    /* 0x30 */ { cmd_settargethr_req,   cmd_settargethr_resp },
    /* 0x31 */ { NULL,                  NULL },
    /* 0x32 */ { NULL,                  NULL },
    /* 0x33 */ { cmd_setmets_req,       cmd_setmets_resp },
    /* 0x34 */ { cmd_setpower_req,      cmd_setpower_resp },
    /* 0x35 */ { cmd_sethrzone_req,     cmd_sethrzone_resp },
    /* 0x36 */ { cmd_sethrmax_req,      cmd_sethrmax_resp },
    /* 0x37 */ { NULL,                  NULL },
    /* 0x38 */ { NULL,                  NULL },
    /* 0x39 */ { NULL,                  NULL },
    /* 0x3a */ { NULL,                  NULL },
    /* 0x3b */ { NULL,                  NULL },
    /* 0x3c */ { NULL,                  NULL },
    /* 0x3d */ { NULL,                  NULL },
    /* 0x3e */ { NULL,                  NULL },
    /* 0x3f */ { NULL,                  NULL },
    /* 0x40 */ { cmd_setchannelrange_req,   cmd_setchannelrange_resp },
    /* 0x41 */ { cmd_setvolumerange_req,cmd_setvolumerange_resp },
    /* 0x42 */ { cmd_setaudiomute_req,  cmd_setaudiomute_resp },
    /* 0x43 */ { cmd_setaudiochannel_req,   cmd_setaudiochannel_resp },
    /* 0x44 */ { cmd_setaudiovolume_req,cmd_setaudiovolume_resp },
    /* 0x45 */ { NULL,                  NULL },
    /* 0x46 */ { NULL,                  NULL },
    /* 0x47 */ { NULL,                  NULL },
    /* 0x48 */ { NULL,                  NULL },
    /* 0x49 */ { NULL,                  NULL },
    /* 0x4a */ { NULL,                  NULL },
    /* 0x4b */ { NULL,                  NULL },
    /* 0x4c */ { NULL,                  NULL },
    /* 0x4d */ { NULL,                  NULL },
    /* 0x4e */ { NULL,                  NULL },
    /* 0x4f */ { NULL,                  NULL },
    /* 0x50 */ { NULL,                  NULL },
    /* 0x51 */ { NULL,                  NULL },
    /* 0x52 */ { NULL,                  NULL },
    /* 0x53 */ { NULL,                  NULL },
    /* 0x54 */ { NULL,                  NULL },
    /* 0x55 */ { NULL,                  NULL },
    /* 0x56 */ { NULL,                  NULL },
    /* 0x57 */ { NULL,                  NULL },
    /* 0x58 */ { NULL,                  NULL },
    /* 0x59 */ { NULL,                  NULL },
    /* 0x5a */ { NULL,                  NULL },
    /* 0x5b */ { NULL,                  NULL },
    /* 0x5c */ { NULL,                  NULL },
    /* 0x5d */ { NULL,                  NULL },
    /* 0x5e */ { NULL,                  NULL },
    /* 0x5f */ { NULL,                  NULL },
    /* 0x60 */ { cmd_starttext_req,     cmd_starttext_resp },
    /* 0x61 */ { cmd_appendtext_req,    cmd_appendtext_resp },
    /* 0x62 */ { NULL,                  NULL },
    /* 0x63 */ { NULL,                  NULL },
    /* 0x64 */ { NULL,                  NULL },
    /* 0x65 */ { cmd_gettextstatus_req, cmd_gettextstatus_resp },
    /* 0x66 */ { NULL,                  NULL },
    /* 0x67 */ { NULL,                  NULL },
    /* 0x68 */ { NULL,                  NULL },
    /* 0x69 */ { NULL,                  NULL },
    /* 0x6a */ { NULL,                  NULL },
    /* 0x6b */ { NULL,                  NULL },
    /* 0x6c */ { NULL,                  NULL },
    /* 0x6d */ { NULL,                  NULL },
    /* 0x6e */ { NULL,                  NULL },
    /* 0x6f */ { NULL,                  NULL },
    /* 0x70 */ { cmd_getcaps_req,       cmd_getcaps_resp },
    /* 0x71 */ { NULL,                  NULL },
    /* 0x72 */ { NULL,                  NULL },
    /* 0x73 */ { NULL,                  NULL },
    /* 0x74 */ { NULL,                  NULL },
    /* 0x75 */ { NULL,                  NULL },
    /* 0x76 */ { cmd_setpmcfg_req,      cmd_setpmcfg_resp },
    /* 0x77 */ { cmd_setpmdata_req,     cmd_setpmdata_resp },
    /* 0x78 */ { NULL,                  NULL },
    /* 0x79 */ { NULL,                  NULL },
    /* 0x7a */ { NULL,                  NULL },
    /* 0x7b */ { NULL,                  NULL },
    /* 0x7c */ { NULL,                  NULL },
    /* 0x7d */ { NULL,                  NULL },
    /* 0x7e */ { cmd_getpmcfg_req,      cmd_getpmcfg_resp },
    /* 0x7f */ { cmd_getpmdata_req,     cmd_getpmdata_resp },

    /* Short commands. */
    /* 0x80 */ { cmd_getstatus_req,     cmd_getstatus_resp },
    /* 0x81 */ { cmd_reset_req,         cmd_reset_resp },
    /* 0x82 */ { cmd_goidle_req,        cmd_goidle_resp },
    /* 0x83 */ { cmd_gohaveid_req,      cmd_gohaveid_resp },
    /* 0x84 */ { cmd_goinuse_req,       cmd_goinuse_resp },
    /* 0x85 */ { NULL,                  NULL },
    /* 0x86 */ { cmd_gofinished_req,    cmd_gofinished_resp },
    /* 0x87 */ { cmd_goready_req,       cmd_goready_resp },
    /* 0x88 */ { cmd_badid_req,         cmd_badid_resp },
    /* 0x89 */ { NULL,                  NULL },
    /* 0x8a */ { NULL,                  NULL },
    /* 0x8b */ { NULL,                  NULL },
    /* 0x8c */ { NULL,                  NULL },
    /* 0x8d */ { NULL,                  NULL },
    /* 0x8e */ { NULL,                  NULL },
    /* 0x8f */ { NULL,                  NULL },
    /* 0x90 */ { NULL,                  NULL },
    /* 0x91 */ { cmd_getversion_req,    cmd_getversion_resp },
    /* 0x92 */ { cmd_getid_req,         cmd_getid_resp },
    /* 0x93 */ { cmd_getunits_req,      cmd_getunits_resp },
    /* 0x94 */ { cmd_getserial_req,     cmd_getserial_resp },
    /* 0x95 */ { NULL,                  NULL },
    /* 0x96 */ { NULL,                  NULL },
    /* 0x97 */ { NULL,                  NULL },
    /* 0x98 */ { cmd_getlist_req,       cmd_getlist_resp },
    /* 0x99 */ { cmd_getutilization_req,    cmd_getutilization_resp },
    /* 0x9a */ { cmd_getmotorcurrent_req,   cmd_getmotorcurrent_resp },
    /* 0x9b */ { cmd_getodometer_req,   cmd_getodometer_resp },
    /* 0x9c */ { cmd_geterrorcode_req,  cmd_geterrorcode_resp },
    /* 0x9d */ { cmd_getservicecode_req,cmd_getservicecode_resp },
    /* 0x9e */ { cmd_getusercfg1_req,   cmd_getusercfg1_resp },
    /* 0x9f */ { cmd_getusercfg2_req,   cmd_getusercfg2_resp },
    /* 0xa0 */ { cmd_gettwork_req,      cmd_gettwork_resp },
    /* 0xa1 */ { cmd_gethorizontal_req, cmd_gethorizontal_resp },
    /* 0xa2 */ { cmd_getvertical_req,   cmd_getvertical_resp },
    /* 0xa3 */ { cmd_getcalories_req,   cmd_getcalories_resp },
    /* 0xa4 */ { cmd_getprogram_req,    cmd_getprogram_resp },
    /* 0xa5 */ { cmd_getspeed_req,      cmd_getspeed_resp },
    /* 0xa6 */ { cmd_getpace_req,       cmd_getpace_resp },
    /* 0xa7 */ { cmd_getcadence_req,    cmd_getcadence_resp },
    /* 0xa8 */ { cmd_getgrade_req,      cmd_getgrade_resp },
    /* 0xa9 */ { cmd_getgear_req,       cmd_getgear_resp },
    /* 0xaa */ { cmd_getuplist_req,     cmd_getuplist_resp },
    /* 0xab */ { cmd_getuserinfo_req,   cmd_getuserinfo_resp },
    /* 0xac */ { cmd_gettorque_req,     cmd_gettorque_resp },
    /* 0xad */ { NULL,                  NULL },
    /* 0xae */ { NULL,                  NULL },
    /* 0xaf */ { NULL,                  NULL },
    /* 0xb0 */ { cmd_gethrcur_req,      cmd_gethrcur_resp },
    /* 0xb1 */ { NULL,                  NULL },
    /* 0xb2 */ { cmd_gethrtzone_req,    cmd_gethrtzone_resp },
    /* 0xb3 */ { cmd_getmets_req,       cmd_getmets_resp },
    /* 0xb4 */ { cmd_getpower_req,      cmd_getpower_resp },
    /* 0xb5 */ { cmd_gethravg_req,      cmd_gethravg_resp },
    /* 0xb6 */ { cmd_gethrmax_req,      cmd_gethrmax_resp },
    /* 0xb7 */ { NULL,                  NULL },
    /* 0xb8 */ { NULL,                  NULL },
    /* 0xb9 */ { NULL,                  NULL },
    /* 0xba */ { NULL,                  NULL },
    /* 0xbb */ { NULL,                  NULL },
    /* 0xbc */ { NULL,                  NULL },
    /* 0xbd */ { NULL,                  NULL },
    /* 0xbe */ { cmd_getuserdata1_req,  cmd_getuserdata1_resp },
    /* 0xbf */ { cmd_getuserdata2_req,  cmd_getuserdata2_resp },
    /* 0xc0 */ { cmd_getaudiochannel_req,cmd_getaudiochannel_resp },
    /* 0xc1 */ { cmd_getaudiovolume_req, cmd_getaudiovolume_resp },
    /* 0xc2 */ { cmd_getaudiomute_req,  cmd_getaudiomute_resp },
    /* 0xc3 */ { NULL,                  NULL },
    /* 0xc4 */ { NULL,                  NULL },
    /* 0xc5 */ { NULL,                  NULL },
    /* 0xc6 */ { NULL,                  NULL },
    /* 0xc7 */ { NULL,                  NULL },
    /* 0xc8 */ { NULL,                  NULL },
    /* 0xc9 */ { NULL,                  NULL },
    /* 0xca */ { NULL,                  NULL },
    /* 0xcb */ { NULL,                  NULL },
    /* 0xcc */ { NULL,                  NULL },
    /* 0xcd */ { NULL,                  NULL },
    /* 0xce */ { NULL,                  NULL },
    /* 0xcf */ { NULL,                  NULL },
    /* 0xd0 */ { NULL,                  NULL },
    /* 0xd1 */ { NULL,                  NULL },
    /* 0xd2 */ { NULL,                  NULL },
    /* 0xd3 */ { NULL,                  NULL },
    /* 0xd4 */ { NULL,                  NULL },
    /* 0xd5 */ { NULL,                  NULL },
    /* 0xd6 */ { NULL,                  NULL },
    /* 0xd7 */ { NULL,                  NULL },
    /* 0xd8 */ { NULL,                  NULL },
    /* 0xd9 */ { NULL,                  NULL },
    /* 0xda */ { NULL,                  NULL },
    /* 0xdb */ { NULL,                  NULL },
    /* 0xdc */ { NULL,                  NULL },
    /* 0xdd */ { NULL,                  NULL },
    /* 0xde */ { NULL,                  NULL },
    /* 0xdf */ { NULL,                  NULL },
    /* 0xe0 */ { cmd_endtext_req,       cmd_endtext_resp },
    /* 0xe1 */ { cmd_displaypopup_req,  cmd_displaypopup_resp },
    /* 0xe2 */ { NULL,                  NULL },
    /* 0xe3 */ { NULL,                  NULL },
    /* 0xe4 */ { NULL,                  NULL },
    /* 0xe5 */ { cmd_getpopupstatus_req,cmd_getpopupstatus_resp },
    /* 0xe6 */ { NULL,                  NULL },
    /* 0xe7 */ { NULL,                  NULL },
    /* 0xe8 */ { NULL,                  NULL },
    /* 0xe9 */ { NULL,                  NULL },
    /* 0xea */ { NULL,                  NULL },
    /* 0xeb */ { NULL,                  NULL },
    /* 0xec */ { NULL,                  NULL },
    /* 0xed */ { NULL,                  NULL },
    /* 0xee */ { NULL,                  NULL },
    /* 0xef */ { NULL,                  NULL },
    /* 0xf0 */ { NULL,                  NULL },
    /* 0xf1 */ { NULL,                  NULL },
    /* 0xf2 */ { NULL,                  NULL },
    /* 0xf3 */ { NULL,                  NULL },
    /* 0xf4 */ { NULL,                  NULL },
    /* 0xf5 */ { NULL,                  NULL },
    /* 0xf6 */ { NULL,                  NULL },
    /* 0xf7 */ { NULL,                  NULL },
    /* 0xf8 */ { NULL,                  NULL },
    /* 0xf9 */ { NULL,                  NULL },
    /* 0xfa */ { NULL,                  NULL },
    /* 0xfb */ { NULL,                  NULL },
    /* 0xfc */ { NULL,                  NULL },
    /* 0xfd */ { NULL,                  NULL },
    /* 0xfe */ { NULL,                  NULL },
    /* 0xff */ { NULL,                  NULL }
};

/*
 * Add command to csafe context.
 */
int csafe_add(csafe_t *c, csafe_cmd_t cmd, csafe_vars_req_t *a) {
    if (!csafe_cmds[cmd].req) {
        return -1;
    }

    return csafe_cmds[cmd].req(c, a);
}

/*
 * Put the in-progress csafe request into buff and len.
 * Properly format with start, stop and checksum.
 *
 * XXX length checks required
 */
int csafe_get_frame(csafe_t *c, unsigned char *buff, unsigned int *len) {
    memcpy(buff, c->buff, c->len);              /* take a copy */
    *len = c->len;

    buff[(*len)++] = checksum(c->buff, c->len); /* add checksum */
    if (byte_stuff(buff, len) < 0) {            /* stuff */
        return -1;
    }

    memmove(buff+1, buff, *len);                /* add start / stop */
    (*len)++;
    buff[0] = CSAFE_START_FLAG;
    buff[(*len)++] = CSAFE_STOP_FLAG;

    return 0;
}

/*
 * Put a buffer buff of size len into a csafe. Strip start, stop
 * and verify checksum before we do this.
 *
 * XXX length checks required?
 */
int csafe_set_frame(csafe_t *c, const unsigned char *buff, const unsigned int len) {
    if (len >= CSAFE_MAX_FRAME_SIZE_HID) {
        return -1;
    }

    if (buff[0] != CSAFE_START_FLAG ||
            buff[len-1] != CSAFE_STOP_FLAG) {
        return -2;
    }

    memcpy(c->buff, buff+1, len-2);             /* copy, stripping start and stop */
    c->len = len - 2;

    byte_unstuff(c->buff, &c->len);             /* unstuff */

    if (checksum(c->buff, c->len-1) != c->buff[c->len-1]) { /* verify */
        return -3;
    }
    c->len--;                                   /* dump checksum */

    return 0;
}

int csafe_unpack(csafe_t *c, csafe_vars_resp_t *a) {
    unsigned char cmd;

    while (c->len > 0) {
        POP_BYTE(c, cmd);

        if (!csafe_cmds[cmd].resp) {
#ifdef CSAFE_DEBUG
            fprintf(stderr, "failed to unpack response %02x", cmd);
#endif // CSAFE_DEBUG

            /*
             * Bail out on an unknown command. We should jump over
             * the length of the response, but if we have bogus data
             * we'll end up in a bad state. Just dump it for now.
             */
            return -1;
        }

        csafe_cmds[cmd].resp(c, a);
    }

    return 0;
}

/*
 *
 */
int csafe_unpack_resp(csafe_t *c, csafe_vars_resp_t *a) {
    if (c->len == 0) {
        return 0;
    }

    /*
     * First byte of a response is always the status. Pull this
     * out manually, as we don't have a command identifier for this
     * data. Note that a request for status (GETSTATUS) will have
     * an identifier.
     */
    POP_BYTE(c, a->status);

    /*
     * CSAFE 2.5.1: we aren't worried about the frame count
     * bit so discard it.
     */
    a->status &= 0x7f;

    return csafe_unpack(c, a);
}
