#ifndef S17_ROSTER_H
#define S17_ROSTER_H

/**
 * S17 Network Roster - Device IDs
 * 
 * Simple macro definitions for all devices in the S17 network.
 * Military-inspired callsign naming.
 * 
 * Usage:
 * - Share this file across all devices in the network
 * - Each device sets S17_THIS_DEVICE_ID to one of these values
 * - Network addresses remain private in each device's config
 */

/* S17 Network Node IDs - Random for security */
#define S17_NODE_HERMES     0x42A7    /* Board A - Primary Controller */
#define S17_NODE_HOMA       0x8D1F    /* Board B - Homa RC Plane*/
#define S17_NODE_BELFHYM    0x3C95    /* Board C - Belfhym Machine */
//#define S17_NODE_DELTA      0x6E4B    /* Board D - Actuator Node */
//#define S17_NODE_ECHO       0x7A83    /* Board E - Relay Node */
//#define S17_NODE_FOXTROT    0x91D6    /* Board F - Reserved */
//#define S17_NODE_GOLF       0x2F54    /* Board G - Reserved */
//#define S17_NODE_HOTEL      0x5B29    /* Board H - Reserved */

/* Special IDs */
#define S17_BROADCAST       0xFFFF    /* Broadcast to all devices */

#endif // S17_ROSTER_H