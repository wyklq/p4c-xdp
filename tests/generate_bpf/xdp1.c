/* Automatically generated by p4c-xdp from xdp1.p4 on Tue Nov  6 05:53:22 2018
 */
#include "xdp1.h"
#define KBUILD_MODNAME "xdptest"
#include "ebpf_xdp.h"

enum ebpf_errorCodes {
    NoError,
    PacketTooShort,
    NoMatch,
    StackOutOfBounds,
    HeaderTooShort,
    ParserTimeout,
};

#define EBPF_MASK(t, w) ((((t)(1)) << (w)) - (t)1)
#define BYTES(w) ((w) / 8)
#define write_partial(a, s, v) do { u8 mask = EBPF_MASK(u8, s); *((u8*)a) = ((*((u8*)a)) & ~mask) | (((v) >> (8 - (s))) & mask); } while (0)
#define write_byte(base, offset, v) do { *(u8*)((base) + (offset)) = (v); } while (0)

void* memcpy(void* dest, const void* src, size_t num);

inline u16 ebpf_ipv4_checksum(u8 version, u8 ihl, u8 diffserv,
                  u16 totalLen, u16 identification, u8 flags,
                  u16 fragOffset, u8 ttl, u8 protocol,
                  u32 srcAddr, u32 dstAddr) {
    u32 checksum = htons(((u16)version << 12) | ((u16)ihl << 8) | (u16)diffserv);
    checksum += htons(totalLen);
    checksum += htons(identification);
    checksum += htons(((u16)flags << 13) | fragOffset);
    checksum += htons(((u16)ttl << 8) | (u16)protocol);
    srcAddr = htonl(srcAddr);
    dstAddr = htonl(dstAddr);
    checksum += (srcAddr >> 16) + (u16)srcAddr;
    checksum += (dstAddr >> 16) + (u16)dstAddr;
    // Fields in 'struct Headers' are host byte order.
    // Deparser converts to network byte-order
    return __constant_ntohs(~((checksum & 0xFFFF) + (checksum >> 16)));
}
inline u16 csum16_add(u16 csum, u16 addend) {
    u16 res = csum;
    res += addend;
    return (res + (res < addend));
}
inline u16 csum16_sub(u16 csum, u16 addend) {
    return csum16_add(csum, ~addend);
}
inline u16 csum_replace2(u16 csum, u16 old, u16 new) {
    return (~csum16_add(csum16_sub(~csum, old), new));
}

inline u16 csum_fold(u32 csum) {
    u32 r = csum << 16 | csum >> 16;
    csum = ~csum;
    csum -= r;
    return (u16)(csum >> 16);
}
inline u32 csum_unfold(u16 csum) {
    return (u32)csum;
}
inline u32 csum32_add(u32 csum, u32 addend) {
    u32 res = csum;
    res += addend;
    return (res + (res < addend));
}
inline u32 csum32_sub(u32 csum, u32 addend) {
    return csum32_add(csum, ~addend);
}
inline u16 csum_replace4(u16 csum, u32 from, u32 to) {
    u32 tmp = csum32_sub(~csum_unfold(csum), from);
    return csum_fold(csum32_add(tmp, to));
}

struct bpf_map_def SEC("maps") perf_event = {
   .type = BPF_MAP_TYPE_PERF_EVENT_ARRAY,
   .key_size = sizeof(u32),
   .value_size = sizeof(u32),
   .pinning = 2,
   .max_entries = 2,
};
#define BPF_PERF_EVENT_OUTPUT() do {\
    int pktsize = (int)(skb->data_end - skb->data);\
    bpf_perf_event_output(skb, &perf_event, ((u64)pktsize << 32), &pktsize, 4);\
} while(0);

#define BPF_KTIME_GET_NS() ({\
   u32 ___ts = (u32)bpf_ktime_get_ns(); ___ts; })\

struct bpf_map_def SEC("maps") ebpf_outTable = {
    .type = BPF_MAP_TYPE_PERCPU_ARRAY,
    .key_size = sizeof(u32),
    .value_size = sizeof(u32),
    .pinning = 2, /* PIN_GLOBAL_NS */
    .max_entries = 1 /* No multicast support */
};

SEC("prog")
int ebpf_filter(SK_BUFF *skb){
    struct Headers hd = {
        .ethernet = {
            .ebpf_valid = 0
        },
        .ipv4 = {
            .ebpf_valid = 0
        },
    };
    unsigned ebpf_packetOffsetInBits = 0;
    enum ebpf_errorCodes ebpf_errorCode = NoError;
    void* ebpf_packetStart = ((void*)(long)skb->data);
    void* ebpf_packetEnd = ((void*)(long)skb->data_end);
    u32 ebpf_zero = 0;
    u8 ebpf_byte = 0;
    u32 ebpf_outHeaderLength = 0;
    struct xdp_output xout;
    /* TODO: this should be initialized by the environment. HOW? */
    struct xdp_input xin;

    goto start;
    start: {
/* extract(hd.ethernet)*/
        if (ebpf_packetEnd < ebpf_packetStart + BYTES(ebpf_packetOffsetInBits + 112)) {
            ebpf_errorCode = PacketTooShort;
            goto reject;
        }
        hd.ethernet.destination = (u64)((load_dword(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits)) >> 16) & EBPF_MASK(u64, 48));
        ebpf_packetOffsetInBits += 48;

        hd.ethernet.source = (u64)((load_dword(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits)) >> 16) & EBPF_MASK(u64, 48));
        ebpf_packetOffsetInBits += 48;

        hd.ethernet.protocol = (u16)((load_half(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 16;

        hd.ethernet.ebpf_valid = 1;
        switch (hd.ethernet.protocol) {
            case 2048: goto parse_ipv4;
            default: goto accept;
        }
    }
    parse_ipv4: {
/* extract(hd.ipv4)*/
        if (ebpf_packetEnd < ebpf_packetStart + BYTES(ebpf_packetOffsetInBits + 160)) {
            ebpf_errorCode = PacketTooShort;
            goto reject;
        }
        hd.ipv4.version = (u8)((load_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits)) >> 4) & EBPF_MASK(u8, 4));
        ebpf_packetOffsetInBits += 4;

        hd.ipv4.ihl = (u8)((load_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))) & EBPF_MASK(u8, 4));
        ebpf_packetOffsetInBits += 4;

        hd.ipv4.diffserv = (u8)((load_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 8;

        hd.ipv4.totalLen = (u16)((load_half(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 16;

        hd.ipv4.identification = (u16)((load_half(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 16;

        hd.ipv4.flags = (u8)((load_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits)) >> 5) & EBPF_MASK(u8, 3));
        ebpf_packetOffsetInBits += 3;

        hd.ipv4.fragOffset = (u16)((load_half(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))) & EBPF_MASK(u16, 13));
        ebpf_packetOffsetInBits += 13;

        hd.ipv4.ttl = (u8)((load_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 8;

        hd.ipv4.protocol = (u8)((load_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 8;

        hd.ipv4.hdrChecksum = (u16)((load_half(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 16;

        hd.ipv4.srcAddr = (u32)((load_word(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 32;

        hd.ipv4.dstAddr = (u32)((load_word(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits))));
        ebpf_packetOffsetInBits += 32;

        hd.ipv4.ebpf_valid = 1;
        goto accept;
    }

    reject: { return XDP_ABORTED; }

    accept:
    {
        u8 hit;
        enum xdp_action tmp_0;
        {
xout.output_port = 0;
            if ((hd.ethernet.protocol != 2048)) 
                tmp_0 = XDP_DROP;
            else 
                tmp_0 = XDP_PASS;
            xout.output_action = tmp_0;
        }
    }
    /* deparser */
    {
        {
if (hd.ethernet.ebpf_valid) ebpf_outHeaderLength += 112;
            if (hd.ipv4.ebpf_valid) ebpf_outHeaderLength += 160;
        }
        bpf_xdp_adjust_head(skb, BYTES(ebpf_packetOffsetInBits) - BYTES(ebpf_outHeaderLength));
        ebpf_packetStart = ((void*)(long)skb->data);
        ebpf_packetEnd = ((void*)(long)skb->data_end);
        ebpf_packetOffsetInBits = 0;
        u8 hit_0;
        {
/* packet.emit(hd.ethernet)*/
            if (hd.ethernet.ebpf_valid) {
                if (ebpf_packetEnd < ebpf_packetStart + BYTES(ebpf_packetOffsetInBits + 112)) {
                    ebpf_errorCode = PacketTooShort;
                    return XDP_ABORTED;
                }
                ebpf_byte = ((char*)(&hd.ethernet.destination))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.destination))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.destination))[2];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 2, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.destination))[3];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 3, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.destination))[4];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 4, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.destination))[5];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 5, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 48;
                ebpf_byte = ((char*)(&hd.ethernet.source))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.source))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.source))[2];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 2, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.source))[3];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 3, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.source))[4];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 4, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.source))[5];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 5, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 48;
                hd.ethernet.protocol = htons(hd.ethernet.protocol);
                ebpf_byte = ((char*)(&hd.ethernet.protocol))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ethernet.protocol))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 16;
            }
;
            /* packet.emit(hd.ipv4)*/
            if (hd.ipv4.ebpf_valid) {
                if (ebpf_packetEnd < ebpf_packetStart + BYTES(ebpf_packetOffsetInBits + 160)) {
                    ebpf_errorCode = PacketTooShort;
                    return XDP_ABORTED;
                }
                ebpf_byte = ((char*)(&hd.ipv4.version))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 4);
                ebpf_packetOffsetInBits += 4;
                ebpf_byte = ((char*)(&hd.ipv4.ihl))[0];
                write_partial(ebpf_packetStart + BYTES(ebpf_packetOffsetInBits) + 0, 4, (ebpf_byte) << 4);
                ebpf_packetOffsetInBits += 4;
                ebpf_byte = ((char*)(&hd.ipv4.diffserv))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 8;
                hd.ipv4.totalLen = htons(hd.ipv4.totalLen);
                ebpf_byte = ((char*)(&hd.ipv4.totalLen))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.totalLen))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 16;
                hd.ipv4.identification = htons(hd.ipv4.identification);
                ebpf_byte = ((char*)(&hd.ipv4.identification))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.identification))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 16;
                ebpf_byte = ((char*)(&hd.ipv4.flags))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 5);
                ebpf_packetOffsetInBits += 3;
                ebpf_byte = ((char*)(&hd.ipv4.fragOffset))[0];
                write_partial(ebpf_packetStart + BYTES(ebpf_packetOffsetInBits) + 0, 3, (ebpf_byte) << 3);
                ebpf_byte = ((char*)(&hd.ipv4.fragOffset))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 13;
                ebpf_byte = ((char*)(&hd.ipv4.ttl))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 8;
                ebpf_byte = ((char*)(&hd.ipv4.protocol))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 8;
                hd.ipv4.hdrChecksum = htons(hd.ipv4.hdrChecksum);
                ebpf_byte = ((char*)(&hd.ipv4.hdrChecksum))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.hdrChecksum))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 16;
                hd.ipv4.srcAddr = htonl(hd.ipv4.srcAddr);
                ebpf_byte = ((char*)(&hd.ipv4.srcAddr))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.srcAddr))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.srcAddr))[2];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 2, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.srcAddr))[3];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 3, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 32;
                hd.ipv4.dstAddr = htonl(hd.ipv4.dstAddr);
                ebpf_byte = ((char*)(&hd.ipv4.dstAddr))[0];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 0, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.dstAddr))[1];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 1, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.dstAddr))[2];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 2, (ebpf_byte) << 0);
                ebpf_byte = ((char*)(&hd.ipv4.dstAddr))[3];
                write_byte(ebpf_packetStart, BYTES(ebpf_packetOffsetInBits) + 3, (ebpf_byte) << 0);
                ebpf_packetOffsetInBits += 32;
            }
;
        }
    }
    ebpf_end:
    bpf_map_update_elem(&ebpf_outTable, &ebpf_zero, &xout.output_port, BPF_ANY);
    return xout.output_action;
}
char _license[] SEC("license") = "GPL";
