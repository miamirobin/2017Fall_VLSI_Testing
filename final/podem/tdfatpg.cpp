/**********************************************************************/
/*           automatic test pattern generation                        */
/*                                                                    */
/*                                                                    */
/*           Author: yochi                                            */
/*           last update : 01/26/2018                                 */
/**********************************************************************/


#include <iostream>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <stdlib.h>
#include "tdfatpg.h"
#include "global.h"
#include "miscell.h"
#include <assert.h>
#include <algorithm>


using namespace std;


extern "C" {
    typedef struct FAULT *fptr;
    typedef char *string;
    fptr fault_sim_a_vector(fptr undetect_fault,int *current_detect_num);
    fptr generate_detect_fault_list();
    fptr generate_detected_fault_list();
    void sim();

}



vector<vector<int> > test_pattern;
vector<int> vec_temp;

int current_detect_num;
int  total_detect_num=0;
static vector< vector<fptr> > PO_Fanin_Cone_Fault;
static vector<int> CO;

int get_fault_CO( fptr f ) { return CO[ f->to_swlist ]; }

fptr undetect_fault = first_fault;

int pat_num;

void calculate_observability()
{     
	int i, j, k,temp;
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
	//printf("n:%2d\n",n->type);
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
					for( k = 0; k < n->nin; ++k){
						if(n->iwire[k] == w) continue;
						temp += CC0[ n->iwire[k]->wlist_index ];
					}
					break;
				case  NAND:
				case   AND:
					temp = CO[n->owire[0]->wlist_index] + 1;
					for( k = 0; k < n->nin; ++k){
						if(n->iwire[k] == w) continue;
						temp += CC1[ n->iwire[k]->wlist_index ];
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

vector<fptr> fault_array;

bool myrule(fptr f1,fptr f2){if (get_fault_CO( f1)< get_fault_CO( f2)){return true;}  }

void reorder_fault_list(){
  
   calculate_observability();
   for (fptr f = first_fault; f; f = f->pnext_undetect) {
       
        fault_array.push_back(f);        

   }
   sort(fault_array.begin(),fault_array.end(),myrule);
  
   fptr f;
   first_fault=NULL;
   for (int i=0;i<fault_array.size();i++) {
         f =fault_array[i];
         f->pnext = first_fault;
         f->pnext_undetect = first_fault;
         first_fault = f;
        
    }
  
}








void store_pattern(){
  vec_temp.clear();
  for (int i = 1; i < ncktin; i++) {
    
    vec_temp.push_back(cktin[i]->value);
     
    }
    vec_temp.push_back(cktin[ncktin-1]->value0);
     //vec_temp.push_back( rand()&01 );
     vec_temp.push_back(cktin[0]->value);
    test_pattern.push_back(vec_temp);



 // if (pattern_num==0){for (int i=0;i<ncktin+1;i++){cout<<vec_temp[i];}}

}

void erase_pattern(int num){

   test_pattern.erase( test_pattern.begin()+num);

}
void delete_pattern(){
    test_pattern.pop_back();


}


void v1_activate(int num,fptr flist){

    /* for every input, set its value to the vector V1 */ 
       
        for (int j = 0; j < ncktin; j++) {
                sort_wlist[j]->value =test_pattern[num][j] ;
        }
        
       /* initialize the circuit - mark all inputs as changed and all other nodes as unknown (2) */
        for ( int i = 0; i < ncktwire; i++) {
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
      
         for (fptr f = flist; f; f = f->pnext_undetect) {
            if (f->fault_type == sort_wlist[f->to_swlist]->value){
               f->activate = TRUE;
            }
            else{
               f->activate = FALSE;
            }
         }
          /* for every input, set its value to the vector V2 with launch on shift technique  */       
     
        for (int i = 0; i < ncktin; i++) {
           

            if (i == 0){
               sort_wlist[i]->value =test_pattern[num][ncktin];
            }
            else{
               sort_wlist[i]->value =test_pattern[num][i-1];
            }
           
            
         }



}
int pattern_size(){

  return test_pattern.size();
}


void sim_patterns(){

   undetect_fault=generate_detect_fault_list();
   /*for (int num=0;num<pattern_num;num++){
         v1_activate(num,undetect_fault);
   
         undetect_fault = fault_sim_a_vector(undetect_fault,&current_detect_num);
         total_detect_num += current_detect_num;
   }*/
   
   
   while (undetect_fault){
        vec_temp.clear();
        pat_num= undetect_fault->patterns;
        vec_temp= test_pattern[ pat_num];
        v1_activate(pat_num,undetect_fault);
        test_pattern.push_back(  vec_temp);
         undetect_fault = fault_sim_a_vector(undetect_fault,&current_detect_num);
        total_detect_num += current_detect_num;
       


   }
}


void display_patterns(){


  
  for (int i = 0; i < test_pattern.size(); i++) {
    fprintf(stdout,"T\'");
    for (int j = 0; j < ncktin; j++) {
        switch (test_pattern[i][j]) {
            case 0: fprintf(stdout,"0"); break;
            case 1: fprintf(stdout,"1"); break;
            case 2: fprintf(stdout,"%1d",rand()%2); break;
            case 3: fprintf(stdout,"1"); break;
            case 4: fprintf(stdout,"0"); break;
        }
    }
    
    printf("%2d",test_pattern[i][ncktin]);
    fprintf(stdout,"'");
    fprintf(stdout,"\n");
    

 }  
    
    fprintf(stderr,"\n# time: %5.6fs",(double)clock() / CLOCKS_PER_SEC);
    fprintf(stderr,"\n# test length:%2d",test_pattern.size());
    
    cout<<"#test length: "<<test_pattern.size()<<endl;
    cout<<"#time: "<<(double)clock() / CLOCKS_PER_SEC<<" s"<<endl;
   return;


}





void random_switch_pattern(){
  pattern_num=test_pattern.size();
  int num= pattern_num;
  int i, j;
  vector<int>tmp;
  for (i = 0; i < num; i++) {
    j = rand() % (num);
    tmp = test_pattern[i];
    test_pattern[i] = test_pattern[j];
    test_pattern[j] = tmp;
  }
}






