#include <iostream>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "score_compute.h" //add by Evan
#include <set> //add by Evan
#include <assert.h> //add by Evan
#include "global.h"
//#include "miscell.h" //add by Evan

using namespace std;

extern "C"{
typedef struct FAULT *fptr;
}

extern int tdf_num_of_gate_fault;
//fptr first_fault; // first_fault definition

static vector< vector<fptr> > PO_Fanin_Cone_Fault;
static vector<int> CO;

int get_fault_CO( fptr f ) { return CO[ f->to_swlist ]; }

void tdf_generate_fault_list2()
{   
    int i,j,k;
    wptr w;
    nptr n;
    fptr f;
    int fault_num;  

	////////////////////////////////////////////EVAN////////////////////////////////////////////v
	// Evan: construct a po size vec to collect fanin cone 
	PO_Fanin_Cone_Fault.resize( ncktout );
	
	vector< set<int> > Wire_Fanout_Po_index;
	Wire_Fanout_Po_index.resize( ncktwire );
	// Evan: compute each wire fanout Po, from po to pi
	
	// Evan: fanout will fanout to itself
	for (i = 0; i < ncktout; ++i) {
		(Wire_Fanout_Po_index[ cktout[i]->wlist_index ]).insert( i );
	}	
	// Evan: internal node case, for-loop follows topological order
	set<int> *wire_po_set_ptr;
	for (i = ncktwire - 1; i >= 0; --i) {
		// Evan: declaration
		wptr w_fanout_wire;
		w = sort_wlist[i];

		// Evan: for each fanout node, traverse its fanout wire
		for (j = 0 ; j < w->nout; ++j) {
			// Evan: declaration
            n = w->onode[j];
            
            for(k = 0; k < n->nout; ++k) {
            	w_fanout_wire = n->owire[k];
            	
            	wire_po_set_ptr = &( Wire_Fanout_Po_index[ w_fanout_wire->wlist_index ] );
            	assert( wire_po_set_ptr->size() > 0 );
            	
            	Wire_Fanout_Po_index[i].insert( wire_po_set_ptr->begin(), wire_po_set_ptr->end() );
            	
            	wire_po_set_ptr = NULL;
            }
		}
	}


	set<int>::iterator wire_po_iter;
/*
	for (i = 0; i < ncktwire; ++i) {
        w = sort_wlist[i];
        cout<<"wire_name: "<<w->name<<endl<<"    fanout_po: ";
		wire_po_set_ptr = &( Wire_Fanout_Po_index[i] );
	    for( wire_po_iter = wire_po_set_ptr->begin(); wire_po_iter != wire_po_set_ptr->end(); ++wire_po_iter ){
	    	cout<<cktout[ *wire_po_iter ]->name<<' ';
	    }
        cout<<endl;
    }
*/

	///////////////////////////////////////////EVAN////////////////////////////////////////////^

    first_fault = NIL(struct FAULT);  // start of fault list 
    tdf_num_of_gate_fault = 0; // totle number of faults in the whole circuit

    /* walk through every wire in the circuit*/
    for (i = ncktwire - 1; i >= 0; i--) {
      w = sort_wlist[i]; // each wire w
        n = w->inode[0]; // w is the gate n output wire
	


	/* for each gate, create a gate output slow to rise (STR) fault */
        if (!(f = ALLOC(1,struct FAULT))) assert(0&&"No more room!");
        f->node = n;
        f->io = GO;
        f->fault_type = 0;//STR
        f->to_swlist = i;
        switch (n->type) {   
      	  case   NOT:
      	  case   BUF:
         	f->eqv_fault_num = 1;
        	for (j = 0; j < w->inode[0]->nin; j++) {
          	   if (w->inode[0]->iwire[j]->nout > 1) f->eqv_fault_num++;
        	}
         	break;
        case   AND:
        case   NOR:
        case INPUT:
        case    OR:
        case  NAND: 
        case   EQV:
        case   XOR: f->eqv_fault_num = 1; break;
      }
      	
        f->detect=FALSE;
         f->detected_times = 0;
        tdf_num_of_gate_fault += f->eqv_fault_num; // accumulate total fault count
        f->pnext = first_fault;  // insert into the fault list
        f->pnext_undetect = first_fault; // initial undetected fault list contains all faults
        first_fault = f;

	///////////////////////////////////////////EVAN////////////////////////////////////////////v
		wire_po_set_ptr = &( Wire_Fanout_Po_index[i] );
		// Evan: add this fault to each fan_out po_list of this wire
	    for( wire_po_iter = wire_po_set_ptr->begin(); wire_po_iter != wire_po_set_ptr->end(); ++wire_po_iter ){
	    	PO_Fanin_Cone_Fault[ *wire_po_iter ].push_back( f );
	    }
	///////////////////////////////////////////EVAN////////////////////////////////////////////^

	/* for each gate, create a gate output slow to fall (STF) fault */
        if (!(f = ALLOC(1,struct FAULT))) assert(0&&"No more room!");
        f->node = n;
        f->io = GO;
        f->fault_type = 1;//STF
        f->to_swlist = i;
        switch (n->type) {
      		case   NOT:
      		case   BUF:
        	f->eqv_fault_num = 1;
        	for (j = 0; j < w->inode[0]->nin; j++) {
          		if (w->inode[0]->iwire[j]->nout > 1) f->eqv_fault_num++;
        	}
        	break;
      		case    OR:
      		case  NAND:
      		case INPUT:
      		case   AND:
      		case   NOR: 
      		case   EQV:
      		case   XOR: f->eqv_fault_num = 1; break;
         }	

        f->detect=FALSE;
        f->detected_times = 0;
        tdf_num_of_gate_fault += f->eqv_fault_num;
        f->pnext = first_fault;
        f->pnext_undetect = first_fault;
        first_fault = f;
	///////////////////////////////////////////EVAN////////////////////////////////////////////v
		//wire_po_set_ptr = &( Wire_Fanout_Po_index[i] );
		// Evan: add this fault to each fan_out po_list of this wire
	    for( wire_po_iter = wire_po_set_ptr->begin(); wire_po_iter != wire_po_set_ptr->end(); ++wire_po_iter ){
	    	PO_Fanin_Cone_Fault[ *wire_po_iter ].push_back( f );
	    }
	///////////////////////////////////////////EVAN////////////////////////////////////////////^

	/*if w has multiple fanout branches,   */
        if (w->nout > 1) {
          for (j = 0 ; j < w->nout; j++) {
            n = w->onode[j];
            switch (n->type) {
             case OUTPUT:
             case    OR:
             case   NOR: 
             case   AND:
             case  NAND:
             case   EQV:
             case   XOR:
                /* create STR */
                if (!(f = ALLOC(1,struct FAULT))) assert( 0&&"No more room!");
                f->node = n;
                f->io = GI;
                f->fault_type = 0;//STR
                f->to_swlist = i;
                f->eqv_fault_num = 1;
                for (k = 0; k < n->nin; k++) {  
                   if (n->iwire[k] == w) f->index = k;
                }
                f->detect=FALSE;
                 f->detected_times = 0;
                tdf_num_of_gate_fault++;
                f->pnext = first_fault;
                f->pnext_undetect = first_fault;
                first_fault = f;

				///////////////////////////////////////////EVAN////////////////////////////////////////////v
				//wire_po_set_ptr = &( Wire_Fanout_Po_index[i] );
				// Evan: add this fault to each fan_out po_list of this wire
	    		for( wire_po_iter = wire_po_set_ptr->begin(); wire_po_iter != wire_po_set_ptr->end(); ++wire_po_iter ){
	    			PO_Fanin_Cone_Fault[ *wire_po_iter ].push_back( f );
	    		}
				///////////////////////////////////////////EVAN////////////////////////////////////////////^

                /* create STF */
                if (!(f = ALLOC(1,struct FAULT))) assert( 0 && "Room more room!");
                f->node = n;
                f->io = GI;
                f->fault_type = 1;//STF
                f->to_swlist = i;
                f->eqv_fault_num = 1;
                for (k = 0; k < n->nin; k++) {
                    if (n->iwire[k] == w) f->index = k;
                }
                f->detect=FALSE;
                 f->detected_times = 0;
                tdf_num_of_gate_fault++;
                f->pnext = first_fault;
                f->pnext_undetect = first_fault;
                first_fault = f;
                
                ///////////////////////////////////////////EVAN////////////////////////////////////////////v
				//wire_po_set_ptr = &( Wire_Fanout_Po_index[i] );
				// Evan: add this fault to each fan_out po_list of this wire
	    		for( wire_po_iter = wire_po_set_ptr->begin(); wire_po_iter != wire_po_set_ptr->end(); ++wire_po_iter ){
	    			PO_Fanin_Cone_Fault[ *wire_po_iter ].push_back( f );
	    		}
				///////////////////////////////////////////EVAN////////////////////////////////////////////^
                
                
                break;
              }  
            }
        }
    }

    /*walk through all fautls, assign fault_no one by one  */
    for (f = first_fault, fault_num = 0; f; f = f->pnext, fault_num++) {
        f->fault_no = fault_num;
    }  		
/*
	for (i = 0; i < ncktout; ++i) {
        w = cktout[i];
        cout<<"PO name: "<<w->name<<endl<<"    fault_wire: ";
	    for( j = 0; j < PO_Fanin_Cone_Fault[i].size(); ++j ){
	    	cout<<sort_wlist[ PO_Fanin_Cone_Fault[i][j]->to_swlist ]->name<<' ';
	    }
        cout<<endl;
    }
*/
    //fprintf(stdout,"#number of equivalent faults = %d\n", fault_num);
    return;  
}/* end of generate_fault_list */


void calculate_observability()
{
	int i, j, temp;
	vector<int> CC0, CC1;
	wptr w;
    nptr n;
	
	CC0.resize( ncktwire );
	CC1.resize( ncktwire );
	CO.resize( ncktwire );

	//compute controllability
	//from Pi to Po
	
	for (i = 0; i < ncktwire; ++i) {
		w = sort_wlist[i];

		assert( w->nin == 1 );
		n = w->inode[0];
		
        switch (n->type) { 
        	case   INPUT:
        		CC1[i] = 1;
				CC0[i] = 1;
				break;
      		case   NOT:
      			assert( n->nin == 1 );
      			CC0[i] = CC1[n->iwire[0]->wlist_index] + 1;
      			CC1[i] = CC0[n->iwire[0]->wlist_index] + 1;
      			break;
      		case   BUF:
      			assert( n->nin == 1 );
      			CC1[i] = CC1[n->iwire[0]->wlist_index] + 1;
      			CC0[i] = CC0[n->iwire[0]->wlist_index] + 1;
      			break;
      		case    OR:
      			//base case
      			//CC0: summation
      			CC0[i] = CC0[n->iwire[0]->wlist_index];
      			//CC1: minimum
      			CC1[i] = CC1[n->iwire[0]->wlist_index];
      			//induction case
      			for( j = 1; j < n->nin; ++j){
      				//CC0: summation
      				CC0[i] += CC0[n->iwire[j]->wlist_index];
      				//CC1: minimum
      				if( CC1[i] > CC1[n->iwire[j]->wlist_index] )
      					CC1[i] = CC1[n->iwire[j]->wlist_index];
      			}
      			//add 1 after traverse all gate input
      			CC0[i] += 1;
      			CC1[i] += 1;
      			break;
      		case  NAND:
      			//base case
      			//CC1: minimum
      			CC1[i] = CC0[n->iwire[0]->wlist_index];
      			//CC0: summation
      			CC0[i] = CC1[n->iwire[0]->wlist_index];
      			//induction case
      			for( j = 1; j < n->nin; ++j){
      				//CC1: minimum
      				if( CC1[i] > CC0[n->iwire[j]->wlist_index] )
      					CC1[i] = CC0[n->iwire[j]->wlist_index];
      				//CC0: summation
      				CC0[i] += CC1[n->iwire[j]->wlist_index];
      			}
      			CC0[i] += 1;
      			CC1[i] += 1;
      			break;
      		case   AND:
      			//base case
      			//CC0: minimum
      			CC0[i] = CC0[n->iwire[0]->wlist_index];
      			//CC1: summation
      			CC1[i] = CC1[n->iwire[0]->wlist_index];
      			//induction case
      			for( j = 1; j < n->nin; ++j){
      				//CC0: minimum
      				if( CC0[i] > CC0[n->iwire[j]->wlist_index] )
      					CC0[i] = CC0[n->iwire[j]->wlist_index];
      				//CC1: summation
      				CC1[i] += CC1[n->iwire[j]->wlist_index];
      			}
      			CC0[i] += 1;
      			CC1[i] += 1;
      			break;
      		case   NOR: 
      			//base case
      			//CC1: summation
      			CC1[i] = CC0[n->iwire[0]->wlist_index];
      			//CC0: minimum
      			CC0[i] = CC1[n->iwire[0]->wlist_index];
      			//induction case
      			for( j = 1; j < n->nin; ++j){
      				//CC1: summation
      				CC1[i] += CC0[n->iwire[j]->wlist_index];
      				//CC0: minimum
      				if( CC0[i] > CC1[n->iwire[j]->wlist_index] )
      					CC0[i] = CC1[n->iwire[j]->wlist_index];
      			}
      			CC0[i] += 1;
      			CC1[i] += 1;
      			break;
      		//default:
      			//cout<<"n->type: "<<n->type<<endl;
      			//cout<<"n: "<<n<<endl;
         }
         
         //cout<<"wire: "<<w->name<<", CC0: "<<CC0[w->wlist_index]<<", CC1: "<<CC1[w->wlist_index]<<endl;
	}
    //compute observability
    // from Po to Pi
    for (i = ncktwire - 1; i >= 0; --i) {
		w = sort_wlist[i];

		n = w->onode[0];
		//for each fanout gate
		//first fanout wire
		switch (n->type) {
        	case   OUTPUT:
        		CO[i] = 0;
				break;
      		case   NOT:
      		case   BUF:
      			CO[i] = CO[n->owire[0]->wlist_index] + 1;
      			break;
      		case    OR:
      		case   NOR:
      			CO[i] = CO[n->owire[0]->wlist_index] + 1;
      			for( j = 0; j < n->nin; ++j){
      				if(n->iwire[j] == w) continue;
      				CO[i] += CC0[ n->iwire[j]->wlist_index ];
      			}
      			break;
      		case  NAND:
      		case   AND:
      			CO[i] = CO[n->owire[0]->wlist_index] + 1;
      			for( j = 0; j < n->nin; ++j){
      				if(n->iwire[j] == w) continue;
      				CO[i] += CC1[ n->iwire[j]->wlist_index ];
      			}
      			break;
			//default:
      			//cout<<"n->type: "<<n->type<<endl;
      			//cout<<"n: "<<n<<endl;
         }
		//multi-fanout case: other fanout wire, choose small CO
		for (j = 1; j < w->nout ; ++j) {
			n = w->onode[j];
			switch (n->type) {
        		case   OUTPUT:
        			CO[i] = 0;
					break;
				case   NOT:
				case   BUF:
					temp = CO[n->owire[0]->wlist_index] + 1;
					break;
				case    OR:
				case   NOR:
					temp = CO[n->owire[0]->wlist_index] + 1;
					for( j = 0; j < n->nin; ++j){
						if(n->iwire[j] == w) continue;
						temp += CC0[ n->iwire[j]->wlist_index ];
					}
					break;
				case  NAND:
				case   AND:
					temp = CO[n->owire[0]->wlist_index] + 1;
					for( j = 0; j < n->nin; ++j){
						if(n->iwire[j] == w) continue;
						temp += CC1[ n->iwire[j]->wlist_index ];
					}
					break;
				//default:
      				//cout<<"n->type: "<<n->type<<endl;
      				//cout<<"n: "<<n<<endl;
			 }
			
			if( temp < CO[i] )
				CO[i] = temp;
		}		
		//cout<<"wire: "<<w->name<<", CO: "<<CO[w->wlist_index]<<endl;
	}
}
