/*!\file star_demo_master_node.h
 * \brief Star demo code on top of the EZMacPRO stack.
 *
 *
 * \n This software must be used in accordance with the End User License
 * \n Agreement.
 *
 * \b COPYRIGHT
 * \n Copyright 2012 Silicon Laboratories, Inc.
 * \n http://www.silabs.com
 */

#ifndef _STAR_DEMO_MASTER_NODE_H_
#define _STAR_DEMO_MASTER_NODE_H_


                /* ======================================= *
                 *          D E F I N I T I O N S          *
                 * ======================================= */

/*!
 * Application Firmware version.
 */
#define APP_FW_VERSION    "1.0.0b"


/*!
 * If 0x08, Listen Before Talk will be performed before each frame transmission.
 */
//#define LBT_SWITCH                          (0x08)
#define LBT_SWITCH                          (0x00)


/*!
 * Number of slaves the demo supports.
 */
#define MAX_NMBR_OF_SLAVES                  (4)


/*!
 * Addresses used in this demo.
 */
#define DEMO_MASTER_CID                     (0xCD)
#define DEMO_MASTER_SFID                    (0x00)
#define DEMO_MASTER_MCAST                   (0xFE)


/*!
 * States of the Star demo.
 */
#define DEMO_STATE_MASK                     (0xF0)
#define DEMO_BOOT                           (0x00)
#define DEMO_BOOT_INIT                      (0x01)
#define DEMO_BOOT_RND_ADDR                  (0x02)
#define DEMO_ASSOC                          (0x10)
#define DEMO_ASSOC_BEACON_TX                (0x11)
#define DEMO_ASSOC_BEACON_RX                (0x12)
#define DEMO_ASSOC_BEACON_RXD               (0x13)
#define DEMO_ASSOC_REQ_TX                   (0x14)
#define DEMO_ASSOC_REQ_RX                   (0x15)
#define DEMO_ASSOC_REQ_RXD                  (0x16)
#define DEMO_ASSOC_RESP_TX                  (0x17)
#define DEMO_ASSOC_RESP_RX                  (0x18)
#define DEMO_ASSOC_RESP_RXD                 (0x19)
#define DEMO_ASSOC_RESP_ACK_TX              (0x1A)
#define DEMO_ASSOC_RESP_ACK_RX              (0x1B)
#define DEMO_ASSOC_RESP_ACK_RXD             (0x1C)
#define DEMO_SU                             (0x20)
#define DEMO_SU_REQ_TX                      (0x21)
#define DEMO_SU_REQ_RX                      (0x22)
#define DEMO_SU_REQ_RXD                     (0x23)
#define DEMO_SU_RESP_TX                     (0x24)
#define DEMO_SU_RESP_RX                     (0x25)
#define DEMO_SU_RESP_RXD                    (0x26)
#define DEMO_SU_SLEEP                       (0x27)
#define DEMO_SLEEP                          (0x30)
#define DEMO_SLEEP_INIT                     (0x31)
#define DEMO_SLEEP_LOOP                     (0x32)


/*!
 * Frame type identifiers used in the Star demo.
 */
#define FRAME_BEACON                        (0x33)
#define FRAME_ASSOC_REQ                     (0x44)
#define FRAME_ASSOC_RESP                    (0x45)
#define FRAME_ASSOC_RESP_ACK                (0x46)
#define FRAME_SU_REQ                        (0x55)
#define FRAME_SU_RESP                       (0x56)


/*!
 * Defines used for the initialisation of slave info table.
 */
#define ASSOCIATED                          (1)
#define NOT_ASSOCIATED                      (0)
#define TIMEOUT_INITIAL_VALUE               (3)
#define TIMEOUT_ZERO_VALUE                  (0)
#define TEMP_INVALID_VALUE                  (0xFF)
#define TEMP_ZERO_VALUE                     (0x80)
#define VOLTAGE_INVALID_VALUE               (0)
#define VOLTAGE_ZERO_VALUE                  (1)
#define RSSI_INITIAL_VALUE                  (0)
#define RSSI_ZERO_VALUE                     (0)


/*!
 * Timings of the demo states.
 *
 * -----------------------------------------------------------------------------
 * |ASSOC|SLAVE1|SLAVE2|SLAVE3|SLAVE4|                  SLEEP                  |
 * -----------------------------------------------------------------------------
 * |200ms|200ms |200ms |200ms |200ms |                  3000ms                 |
 * -----------------------------------------------------------------------------
 * |             1000ms              |                  3000ms                 |
 * -----------------------------------------------------------------------------
 */
#define STARTUP_TIMEOUT                     (3)         // sec
#define TIMEFRAME_ASSOC                     (200)       // msec
#define TIMEFRAME_SU_PER_SLAVE              (200)       // msec
#define TIMEFRAME_SLEEP                     (3)         // sec


/*!
 * Return value of the SearchFreeSlotInAssocTable() function if the association
 * table is full.
 */
#define ASSOC_TABLE_FULL                    (0xFF)


/*!
 * LFT timeout macros used exclusively with LFTMR registers.
 */
#define LFTMR2_TIMEOUT_MSEC(n)              (0x44)      // Internal Time Base, WTR=0x04
#define LFTMR1_TIMEOUT_MSEC(n)              (U8)(((U16)((n*32.768F)/64)&0xFF00)>>8) // WTR=0x04 assumed
#define LFTMR0_TIMEOUT_MSEC(n)              (U8)((U16)((n*32.768F)/64)&0x00FF)      // WTR=0x04 assumed
#define LFTMR2_TIMEOUT_SEC(n)               (0x44)      // WTR=0x04
#define LFTMR1_TIMEOUT_SEC(n)               (U8)(((U16)((n*1000*32.768F)/64)&0xFF00)>>8) // WTR=0x04 assumed
#define LFTMR0_TIMEOUT_SEC(n)               (U8)((U16)((n*1000*32.768F)/64)&0x00FF)      // WTR=0x04 assumed


/*!
 * Address type structures.
 */
typedef struct
{
    U8  cid;
    U8  sfid;
} Addr_t;

/*!
 * Slave info table structure type.
 */
typedef struct
{
    U8      associated;
    Addr_t  address;
    U8      timeout;
    U8      temperature;
    U8      voltage;
    U8      rssi;
} SlaveInfoTable_t;

/*!
 * Below are the defined frames of the demo.
 */
typedef struct
{
    U8  type;
} FrameBeacon_t;

typedef struct
{
    U8  type;
} FrameAssocReq_t;

typedef struct
{
    U8      type;
    Addr_t  newAddr;
} FrameAssocResp_t;

typedef struct
{
    U8      type;
    Addr_t  rndAddr;
} FrameAssocRespAck_t;

typedef struct
{
    U8  type;
} FrameStatusUpdateReq_t;

typedef struct
{
    U8  type;
    U8  temperature;
    U8  voltage;
    U8  rssi;
} FrameStatusUpdateResp_t;

typedef union
{
    FrameBeacon_t           beacon;
    FrameAssocReq_t         assocReq;
    FrameAssocResp_t        assocResp;
    FrameAssocRespAck_t     assocRespAck;
    FrameStatusUpdateReq_t  statusUpdateReq;
    FrameStatusUpdateResp_t statusUpdateResp;
} Frame_u;

typedef union
{
    Frame_u frameUnion;
    U8      frameRaw[sizeof(Frame_u)];
} Frame_uu;


                /* ======================================= *
                 *     G L O B A L   V A R I A B L E S     *
                 * ======================================= */


extern SEGMENT_VARIABLE(SlaveInfoTable[MAX_NMBR_OF_SLAVES], SlaveInfoTable_t, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(rfPayload, Frame_uu, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(packetLength, U8, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(rndCounter, U16, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(masterAddr, Addr_t, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(rndAddr, Addr_t, APPLICATION_MSPACE);
extern SEGMENT_VARIABLE(DEMO_SR, U8, APPLICATION_MSPACE);


                /* ======================================= *
                 *  F U N C T I O N   P R O T O T Y P E S  *
                 * ======================================= */

/*!
 * Local function prototypes.
 */
void StateMachine_Init(void);
void StateMachine(void);

void MasterNodeBoot(void);
void MasterNodeAssociate(void);
void MasterNodeStatusUpdate(void);
void MasterNodeSleep(void);

U8 SearchFreeSlotInAssocTable(void);
void PrintSlaveInfo(void);


#endif //_STAR_DEMO_MASTER_NODE_H_

