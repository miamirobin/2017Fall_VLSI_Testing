/**********************************************************************/
/*           Parallel-Fault Event-Driven Fault Simulator              */
/*                                                                    */
/*           Author: CCM                                              */
/*           last update : 22/10/2016                                 */
/**********************************************************************/

#include <stdio.h>
#include "global.h"
#include "miscell.h"

//DO YOUR PA#3 CODE HERE


#define STR         0
#define STF         1

int tdf_num_of_gate_fault;
extern int debug;



transition_delay_fault_simulation()
{   
    /* announce the variales and functions */
    void tdf_generate_fault_list();
    void tdf_simulate_vectors();
    int total_detect_num;
    fptr flist,f;

    tdf_generate_fault_list(); //generate the fault list with no fault collapsing
    total_detect_num = 0;      // initialize the counter to zero
    flist = first_fault;       //use a ptr to point to the address of first fault

    /* simulate the circuit with given test vectors to detect faults */
    tdf_simulate_vectors(vectors,sim_vectors,flist,&total_detect_num); 

    /*Print out the result */
    printf("\n# Result:\n");
    printf("----------------------------\n");
    printf("# Total transition delay faults: %2d\n", tdf_num_of_gate_fault);
    printf("# Total detected faults: %2d\n", total_detect_num);
    printf("# fault coverage: %5.6f %\n",((double)total_detect_num)/tdf_num_of_gate_fault*100);


}



/* modify from generate_fault_list() in init_flist.c 
   remove the part of fault collapsing */
tdf_generate_fault_list()
{   
    int i,j,k;
    wptr w;
    nptr n;
    fptr f;
    int fault_num;  

    first_fault = NIL(struct FAULT);  // start of fault list 
    tdf_num_of_gate_fault = 0; // totle number of faults in the whole circuit

    /* walk through every wire in the circuit*/
    for (i = ncktwire - 1; i >= 0; i--) {
      w = sort_wlist[i]; // each wire w
        n = w->inode[0]; // w is the gate n output wire

	/* for each gate, create a gate output slow to rise (STR) fault */
        if (!(f = ALLOC(1,struct FAULT))) error("No more room!");
        f->node = n;
        f->io = GO;
        f->fault_type = STR;
        f->to_swlist = i;
      	f->eqv_fault_num = 1;
        f->detect=FALSE;
        tdf_num_of_gate_fault += f->eqv_fault_num; // accumulate total fault count
        f->pnext = first_fault;  // insert into the fault list
        f->pnext_undetect = first_fault; // initial undetected fault list contains all faults
        first_fault = f;

	/* for each gate, create a gate output slow to fall (STF) fault */
        if (!(f = ALLOC(1,struct FAULT))) error("No more room!");
        f->node = n;
        f->io = GO;
        f->fault_type = STF;
        f->to_swlist = i;
	f->eqv_fault_num = 1; 
        f->detect=FALSE;
        tdf_num_of_gate_fault += f->eqv_fault_num;
        f->pnext = first_fault;
        f->pnext_undetect = first_fault;
        first_fault = f;

	/*if w has multiple fanout branches,   */
        if (w->nout > 1) {
            for (j = 0 ; j < w->nout; j++) {
                n = w->onode[j];
		
                /* create STR */
                if (!(f = ALLOC(1,struct FAULT))) error("No more room!");
                f->node = n;
                f->io = GI;
                f->fault_type = STR;
                f->to_swlist = i;
                f->eqv_fault_num = 1;
                for (k = 0; k < n->nin; k++) {  
                   if (n->iwire[k] == w) f->index = k;
                }
                f->detect=FALSE;
                tdf_num_of_gate_fault++;
                f->pnext = first_fault;
                f->pnext_undetect = first_fault;
                first_fault = f;

                /* create STF */
                if (!(f = ALLOC(1,struct FAULT))) error("Room more room!");
                f->node = n;
                f->io = GI;
                f->fault_type = STF;
                f->to_swlist = i;
                f->eqv_fault_num = 1;
                for (k = 0; k < n->nin; k++) {
                    if (n->iwire[k] == w) f->index = k;
                }
                f->detect=FALSE;
                tdf_num_of_gate_fault++;
                f->pnext = first_fault;
                f->pnext_undetect = first_fault;
                first_fault = f;
                
                
            }
        }
    }

    /*walk through all fautls, assign fault_no one by one  */
    for (f = first_fault, fault_num = 0; f; f = f->pnext, fault_num++) {
        f->fault_no = fault_num;
    }

    //fprintf(stdout,"#number of equivalent faults = %d\n", fault_num);
    return;  
}/* end of generate_fault_list */






/* fault simulate a set of test vectors 
   transition delay fault             */
tdf_simulate_vectors(vectors,num_vectors,flist,total_detect_num)
char *vectors[];
int num_vectors;
fptr flist;
int *total_detect_num;
{
    int i,j,nv,current_detect_num;
    fptr f,tdf_sim_a_vector();
 
    /* for every vector */
   for(j=num_vectors-1; j>=0;j--){
        /* for every input, set its value to the vector V1 */ 
        for (i = 0; i < ncktin; i++) {
           nv = ctoi(vectors[j][i]);
           sort_wlist[i]->value = nv;
         
        }
        
        /* initialize the circuit - mark all inputs as changed and all other nodes as unknown (2) */
        for (i = 0; i < ncktwire; i++) {
            if (i < ncktin) {
               sort_wlist[i]->flag |= CHANGED;
            }
            else {
               sort_wlist[i]->value = 2;
            }
         }
         /* simulate the circuit with input vector V1 
            
            to activate the STR fault, the wire value should be 0 first (0->1).
            we have defined that STR =0, so wire value should be equal to fault type.

            to activate the STF fault, the wire value should be 1 first (1->0) 
            we have defined that STF =1, so wire value should be equal to fault type.  */

         sim();/* do a fault-free simulation, see sim.c */
      
         for (f = flist; f; f = f->pnext_undetect) {
            if (f->fault_type == sort_wlist[f->to_swlist]->value){
               f->activate = TRUE;
            }
            else{
               f->activate = FALSE;
            }
         }

         /* for every input, set its value to the vector V2 with launch on shift technique  */       
     
        for (i = 0; i < ncktin; i++) {
           

            if (i == 0){
               nv = ctoi(vectors[j][ncktin]);
            }
            else{
               nv = ctoi(vectors[j][i-1]);
            }
            sort_wlist[i]->value = nv;
            
         }
            
        /* under vector V2, simulate the circuit to detect fault just like SSF.
           use the function fault_sim_a_vector() in faultsim.c
           STR = SA0 
           STF = SA1                                                        */
        flist=fault_sim_a_vector(flist,&current_detect_num); //update the undetected fault list
        // total detect number is the sum of the detect number of each test pattern.
        *total_detect_num += current_detect_num;  
        //fprintf(stderr,"vector[%d] detects %d faults (%d)\n",j,current_detect_num,*total_detect_num);
    }
    

  
    

}// tdf_simulate_vectors

