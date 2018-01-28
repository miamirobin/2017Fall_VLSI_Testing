#include <stdio.h>
#include "global.h"
#include "miscell.h"
#include "tdfatpg.h"

extern int total_attempt_num;

test()
{
    register fptr undetect_fault;
    register fptr f,fault_under_test, ftemp;
    fptr fault_sim_a_vector(), fault_simulate_vectors();
    void transition_delay_fault_simulation();
    int podem();
    int current_detect_num,total_detect_num, i;
    int total_no_of_backtracks = 0;  // accumulative number of backtracks
    int current_backtracks;
    register int no_of_aborted_faults = 0, no_of_redundant_faults = 0;
    register int no_of_calls = 0;
    int display_undetect();
    reorder_fault_list();
    in_vector_no = 0;
    total_detect_num = 0;
    undetect_fault = first_fault;
    fault_under_test = first_fault;
    pattern_num=0;    
    undet_fault=first_fault;
    /* Fsim only mode */
    if(fsim_only)
    {
        undetect_fault=fault_simulate_vectors(vectors,sim_vectors,undetect_fault,&total_detect_num);
        fault_under_test = undetect_fault;
        in_vector_no+=sim_vectors;
        display_undetect(undetect_fault);
        fprintf(stdout,"\n");
        return;
    }// if fsim only
	
	/* tdFsim only mode */
	if(tdfsim_only)
    {   ndet=total_n;
        transition_delay_fault_simulation();
        return;
    }

    /* ATPG mode */
    /* Figure 5 in the PODEM paper */
    doing_podem=1;
    ndet=total_n;                     
    
  


    while(fault_under_test) {    
        switch(podem(fault_under_test,&current_backtracks,0 )) {
            case MAYBE:  
                
                no_of_aborted_faults+=fault_under_test->eqv_fault_num;
                break;
           
            case TRUE:                                      
	      /*by defect, we want only one pattern per fault */
	      /*run a fault simulation, drop ALL detected faults */
                if (total_attempt_num == 1) {  
                       undetect_fault=undet_fault;
                     /*
                     v1_activate(pattern_num, undetect_fault);
                     undetect_fault = fault_sim_a_vector(undetect_fault,&current_detect_num);  
                     total_detect_num += current_detect_num;
                  
                     if (detect_something==0){         
                           
                           delete_pattern();
                     }
                     else {pattern_num++;    }*/
                    // pattern_num++; 
                    // total_detect_num+=fault_under_test->eqv_fault_num;
                  
                }
                /* If we want mutiple petterns per fault, 
		 * NO fault simulation.  drop ONLY the fault under test */ 
                else {
                    fault_under_test->detect = TRUE;
		    /* walk through the undetected fault list */
                    for (f = undetect_fault; f; f = f->pnext_undetect) {
                        if (f == fault_under_test) {
			  /* drop fault_under_test */
                            if (f == undetect_fault)
                                undetect_fault = undetect_fault->pnext_undetect;
                            else {  
                                ftemp->pnext_undetect = f->pnext_undetect;
                            }
                            break;
                        }
                        ftemp = f;
                    }
                }
                in_vector_no++;
                break;
	    case FALSE:  // 
                fault_under_test->detect = REDUNDANT; 
                no_of_redundant_faults+=fault_under_test->eqv_fault_num;
                break;

	  
        }
        fault_under_test->test_tried = TRUE;
        fault_under_test = NULL;
        for (f = undetect_fault; f; f = f->pnext_undetect) {
            if (!f->test_tried) {
                fault_under_test = f;
                break;
            }
        }
        total_no_of_backtracks += current_backtracks; // accumulate number of backtracks
        no_of_calls++;
    }


   printf("\n");
   doing_podem=0;


    if (total_n==1){
         if (compress) {
        
         int pn=pattern_num+1;
         while (pn!=pattern_num){
            pn=pattern_num;
            undetect_fault =  generate_detected_fault_list();
            for (int i=pattern_num-1;i>=0;i--){
               v1_activate(i, undetect_fault);
               undetect_fault = fault_sim_a_vector(undetect_fault,&current_detect_num);  
               total_detect_num += current_detect_num;
                 
               if (detect_something==0){         
                   
                    erase_pattern(i);
               }
             
            }  
            random_switch_pattern();
            
         }
     
      

     }
         display_patterns(); 
 
         return;
    }
    
    sim_patterns();

    pattern_num=pattern_size();

    if (compress) {
      
       int pn=pattern_num+1;
       while (pn!=pattern_num){
          pn=pattern_num;
          undetect_fault =  generate_detected_fault_list();
          for (int i=pattern_num-1;i>=0;i--){
             v1_activate(i, undetect_fault);
             undetect_fault = fault_sim_a_vector(undetect_fault,&current_detect_num);  
             total_detect_num += current_detect_num;
                 
             if (detect_something==0){         
                   
                  erase_pattern(i);
             }
             
          }  
          random_switch_pattern();
       }     
    }


    display_patterns();
    
    /*
    display_undetect(undetect_fault);

    fprintf(stdout,"\n");
    fprintf(stdout,"#number of aborted faults = %d\n",no_of_aborted_faults);
    fprintf(stdout,"\n");
    fprintf(stdout,"#number of redundant faults = %d\n",no_of_redundant_faults);
    fprintf(stdout,"\n");
    fprintf(stdout,"#number of calling podem1 = %d\n",no_of_calls);
    fprintf(stdout,"\n");
    fprintf(stdout,"#total number of backtracks = %d\n",total_no_of_backtracks);
    */
    return;
}/* end of test */









 






