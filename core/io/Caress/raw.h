#include "car_datatypes.h"
#include "car_globals.h"

typedef char modname_t[MAXNAMELENGTH + 1];

struct raw_infovars /* storage type for persistent variables */
    {
    char* m_file_name_save;
    FILE* m_raw_data_file;
    char m_buf[512 /*BLOCK_LENGTH*/];                          /* length of file block */
    char m_big_buf[130 /*MAX_BLOCKS*/ * 512 /*BLOCK_LENGTH*/]; /* largest data block */
    char* m_big_buf_poi;
    int64 m_index1;
    int64 m_saved_index;
    char m_token;
    int32 m_data_unit_type;

    struct raw_list_element_t {
        char d_type;
        int64 number;
    } m_list[4 * 32 /*LIST_LENGTH*/]; /* a node may have max 32 data groups */
    int32 m_list_index;

    struct descr_element_t {
        int32 kind;
        int32 key_num;
        modname_t key_name[2 * 64 /*MAX_KEYS*/]; /* an element may have max 64 nodes */
    } m_descr_list[256 /*DESCR_LIST_LENGTH*/];   /* max number of different element structures*/
    int32 m_descr_index;

    int32 m_actual_descr_index;
    int32 m_actual_key_index;
    int32 m_actual_list_index;
    int32 m_data_end;
    int32 m_elementtype;
    modname_t m_command;
    modname_t m_key;
    modname_t m_detname;
    int32 m_datatype;
    int64 m_datanumber;
    char* m_dataaddr;
    int32 m_element_number;
    int64 m_block_number;
    int64 m_saved_block_number;
    int32 m_data_blocks;
    int32 m_prelim_eof_seen;
    int32 m_first_block_read;
    modname_t m_blanks;
    int32 m_last_data;
#ifndef IDL
    int32 m_pv_flag;
#endif
    int32 m_bConvertInt64ToDouble;
    int32 m_flag;
#ifdef RAW_ONLINE
    int32 m_re_read_flag;
#endif

/* local number representation: 3 definitions to force compile error, if more than 1 is defined */
#if defined(BIG_ENDIAN)
    int32 m_local_int16;
    int32 m_local_int32;
    int32 m_local_int64;
    int32 m_local_ieee32;
    int32 m_local_ieee64;
#elif defined(__vms)
    int32 m_local_int16;
    int32 m_local_int32;
    int32 m_local_int64;
    int32 m_local_ieee32;
    int32 m_local_ieee64;
#elif defined(LITTLE_ENDIAN)
    int32 m_local_int16;
    int32 m_local_int32;
    int32 m_local_int64;
    int32 m_local_ieee32;
    int32 m_local_ieee64;
#endif
};

/* reentrant interface */
struct raw_infovars* allocate_raw_infovars(void);
void free_raw_infovars(struct raw_infovars* pVars);
int32 open_data_file_r(struct raw_infovars* pVars, const char* file_name);
int32 close_data_file_r(struct raw_infovars* pVars);
#ifdef RAW_ONLINE
void re_read_r(struct raw_infovars* pVars);
#endif
int32 next_data_unit_r(struct raw_infovars* pVars, int32* e_number, int32* e_type,
                       modname_t element, modname_t node, int32* d_type, int64* number);
int32 get_data_unit_r(struct raw_infovars* pVars, void* addr);
int32 get_data_partition_r(struct raw_infovars* pVars, void* addr, int32 section,
                           int64 starting_item, int64 number_of_items, int32 type_of_items);
int32 last_data_of_element_r(struct raw_infovars* pVars);

/* non-reentrant (old) interface */
int32 open_data_file(const char* file_name, int32* mode);
int32 close_data_file(void);
#ifdef RAW_ONLINE
void re_read(void);
#endif
int32 next_data_unit(int32* e_number, int32* e_type, modname_t element, modname_t node,
                     int32* d_type, int32* number);
int32 get_data_unit(void* addr);
int32 get_data_partition(void* addr, int32* section, int32* starting_item, int32* number_of_items,
                         int32* type_of_items);
int32 last_data_of_element(void);

/* IDL/PV-Wave interface */
int genwave(int argc, char* argv[]);
int32 open_data_file_pv(int32 argc, char* argv[]);
int32 close_data_file_pv(void);
#ifdef RAW_ONLINE
void re_read_pv(void);
#endif
int32 next_data_unit_pv(int32 argc, char* argv[]);
int32 get_data_unit_pv(int32 argc, char* argv[]);
int32 get_data_partition_pv(int32 argc, char* argv[]);
int32 last_data_of_element_pv(void);
