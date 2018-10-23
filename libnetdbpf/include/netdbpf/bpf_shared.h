/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// const values shared by bpf kernel program bpfloader and netd


// Since we cannot garbage collect the stats map since device boot, we need to make these maps as
// large as possible. The maximum size of number of map entries we can have is depend on the rlimit
// of MEM_LOCK granted to netd. The memory space needed by each map can be calculated by the
// following fomula:
//      elem_size = 40 + roundup(key_size, 8) + roundup(value_size, 8)
//      cost = roundup_pow_of_two(max_entries) * 16 + elem_size * max_entries +
//              elem_size * number_of_CPU
// And the cost of each map currently used is(assume the device have 8 CPUs):
// cookie_tag_map:      key:  8 bytes, value:  8 bytes, cost:  822592 bytes    =   823Kbytes
// uid_counter_set_map: key:  4 bytes, value:  1 bytes, cost:  145216 bytes    =   145Kbytes
// app_uid_stats_map:   key:  4 bytes, value: 32 bytes, cost: 1062784 bytes    =  1063Kbytes
// uid_stats_map:       key: 16 bytes, value: 32 bytes, cost: 1142848 bytes    =  1143Kbytes
// tag_stats_map:       key: 16 bytes, value: 32 bytes, cost: 1142848 bytes    =  1143Kbytes
// iface_index_name_map:key:  4 bytes, value: 16 bytes, cost:   80896 bytes    =    81Kbytes
// iface_stats_map:     key:  4 bytes, value: 32 bytes, cost:   97024 bytes    =    97Kbytes
// dozable_uid_map:     key:  4 bytes, value:  1 bytes, cost:  145216 bytes    =   145Kbytes
// standby_uid_map:     key:  4 bytes, value:  1 bytes, cost:  145216 bytes    =   145Kbytes
// powersave_uid_map:   key:  4 bytes, value:  1 bytes, cost:  145216 bytes    =   145Kbytes
// total:                                                                         4930Kbytes
// It takes maximum 4.9MB kernel memory space if all maps are full, which requires any devices
// running this module to have a memlock rlimit to be larger then 5MB. In the old qtaguid module,
// we don't have a total limit for data entries but only have limitation of tags each uid can have.
// (default is 1024 in kernel);

const int COOKIE_UID_MAP_SIZE = 10000;
const int UID_COUNTERSET_MAP_SIZE = 2000;
const int UID_STATS_MAP_SIZE = 10000;
const int TAG_STATS_MAP_SIZE = 10000;
const int IFACE_INDEX_NAME_MAP_SIZE = 1000;
const int IFACE_STATS_MAP_SIZE = 1000;
const int CONFIGURATION_MAP_SIZE = 1;
const int UID_OWNER_MAP_SIZE = 2000;

#define BPF_PATH "/sys/fs/bpf"

#define BPF_EGRESS_PROG_PATH BPF_PATH "/egress_prog"
#define BPF_INGRESS_PROG_PATH BPF_PATH "/ingress_prog"
#define XT_BPF_INGRESS_PROG_PATH BPF_PATH "/xt_bpf_ingress_prog"
#define XT_BPF_EGRESS_PROG_PATH BPF_PATH "/xt_bpf_egress_prog"
#define XT_BPF_WHITELIST_PROG_PATH BPF_PATH "/xt_bpf_whitelist_prog"
#define XT_BPF_BLACKLIST_PROG_PATH BPF_PATH "/xt_bpf_blacklist_prog"

#define COOKIE_TAG_MAP_PATH BPF_PATH "/traffic_cookie_tag_map"
#define UID_COUNTERSET_MAP_PATH BPF_PATH "/traffic_uid_counterset_map"
#define APP_UID_STATS_MAP_PATH BPF_PATH "/traffic_app_uid_stats_map"
#define UID_STATS_MAP_PATH BPF_PATH "/traffic_uid_stats_map"
#define TAG_STATS_MAP_PATH BPF_PATH "/traffic_tag_stats_map"
#define IFACE_INDEX_NAME_MAP_PATH BPF_PATH "/traffic_iface_index_name_map"
#define IFACE_STATS_MAP_PATH BPF_PATH "/traffic_iface_stats_map"
#define CONFIGURATION_MAP_PATH BPF_PATH "/traffic_configuration_map"
#define UID_OWNER_MAP_PATH BPF_PATH "/traffic_uid_owner_map"

#define BPF_CGROUP_INGRESS_PROG_NAME "cgroup_ingress_prog"
#define BPF_CGROUP_EGRESS_PROG_NAME "cgroup_egress_prog"
#define XT_BPF_INGRESS_PROG_NAME "xt_ingress_prog"
#define XT_BPF_EGRESS_PROG_NAME "xt_egress_prog"
#define XT_BPF_WHITELIST_PROG_NAME "xt_whitelist_prog"
#define XT_BPF_BLACKLIST_PROG_NAME "xt_blacklist_prog"

#define COOKIE_TAG_MAP 0xbfceaaffffffffff
#define UID_COUNTERSET_MAP 0xbfdceeafffffffff
#define APP_UID_STATS_MAP 0xbfa1daafffffffff
#define UID_STATS_MAP 0xbfdaafffffffffff
#define TAG_STATS_MAP 0xbfaaafffffffffff
#define IFACE_STATS_MAP 0xbf1faceaafffffff
#define CONFIGURATION_MAP 0Xbfc0fa0affffffff
#define UID_OWNER_MAP 0xbfbad1d1daffffff

enum UidOwnerMatchType {
    NO_MATCH,
    HAPPY_BOX_MATCH = (1 << 0),
    PENALTY_BOX_MATCH = (1 << 1),
    DOZABLE_MATCH = (1 << 2),
    STANDBY_MATCH = (1 << 3),
    POWERSAVE_MATCH = (1 << 4),
};

// TODO: change the configuration object from an 8-bit bitmask to an object with clearer
// semantics, like a struct.
typedef uint8_t BpfConfig;
const BpfConfig DEFAULT_CONFIG = 0;

// These are also defined in NetdConstants.h, but we want to minimize the number of headers
// included by the BPF kernel program.
// TODO: refactor the the following constant into a seperate file so
// NetdConstants.h can also include it from there.
#define MIN_SYSTEM_UID 0
#define MAX_SYSTEM_UID 9999
#define CONFIGURATION_KEY 1