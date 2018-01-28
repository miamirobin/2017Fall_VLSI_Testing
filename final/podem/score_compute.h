#ifndef _SCORE_COMPUTE_H_
#define _SCORE_COMPUTE_H_


#ifdef __cplusplus
extern "C" {
#endif
typedef struct FAULT *fptr;
void tdf_generate_fault_list2();
void calculate_observability();
int get_fault_CO( fptr );
#ifdef __cplusplus
}
#endif

#endif //_SCORE_COMPUTE_H_
