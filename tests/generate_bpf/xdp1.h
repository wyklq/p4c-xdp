/* Automatically generated by p4c-xdp from xdp1.p4 on Tue Nov  6 05:53:22 2018
 */
#ifndef _P4_GEN_HEADER_
#define _P4_GEN_HEADER_
#define KBUILD_MODNAME "xdptest"
#include "ebpf_xdp.h"

#define MAP_PATH "/sys/fs/bpf/xdp/globals"
struct xdp_input {
    u32 input_port; /* bit<32> */
};

struct xdp_output {
    enum xdp_action output_action; /* xdp_action */
    u32 output_port; /* bit<32> */
};

struct Ethernet {
    u64 destination; /* bit<48> */
    u64 source; /* bit<48> */
    u16 protocol; /* bit<16> */
    u8 ebpf_valid;
};

struct IPv4 {
    u8 version; /* bit<4> */
    u8 ihl; /* bit<4> */
    u8 diffserv; /* bit<8> */
    u16 totalLen; /* bit<16> */
    u16 identification; /* bit<16> */
    u8 flags; /* bit<3> */
    u16 fragOffset; /* bit<13> */
    u8 ttl; /* bit<8> */
    u8 protocol; /* bit<8> */
    u16 hdrChecksum; /* bit<16> */
    u32 srcAddr; /* bit<32> */
    u32 dstAddr; /* bit<32> */
    u8 ebpf_valid;
};

struct Headers {
    struct Ethernet ethernet; /* Ethernet */
    struct IPv4 ipv4; /* IPv4 */
};

#if CONTROL_PLANE
static void init_tables() 
{
    u32 ebpf_zero = 0;
}
#endif
#endif
