




#ifdef __cplusplus
extern "C" {
#endif
typedef struct FAULT *fptr;
void store_pattern();
void sim_patterns();
void display_patterns();
void v1_activate(int ,fptr );  
void delete_pattern();
void erase_pattern(int num);
void random_switch_pattern();
int pattern_size();
void reorder_fault_list();
#ifdef __cplusplus
}
#endif
