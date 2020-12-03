#include "entropy_computer.h"


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>


using namespace std;
using namespace mysqlpp;


int t0, t1;
#define DELTA_T 60






float hist_to_entropy (mysqlpp::StoreQueryResult * resl) {
	
	float entropy = 0;
	float num_el = 0;
	
	Row row;
	mysqlpp::StoreQueryResult::iterator Iter;
	
	
	for ( Iter = resl->begin(); Iter != resl->end(); Iter++ ) {
		row = *Iter;
		num_el += (float) row[0];
	}
	
	
	for ( Iter = resl->begin(); Iter != resl->end(); Iter++ ) {
		row = *Iter;
			
		
		float val = (float) row[0];
		float p = val/num_el;
		
		//printf("%d [%f]\n", (int) row[0], p);
		
		entropy -= (p * log2( p) );
	}
		
	//printf("ENTROPY: %f\n", entropy);
	
	
	return entropy;
}







int main (int argc, char ** argv) {
	
	
	
	try {
		Connection conn(false);
		conn.connect("darpa_2_db", "127.0.0.1", "root");
		
		Query query = conn.query();
		
		
		query << "SELECT min(timestamp), max(timestamp) FROM pcap_darpa;";
	    mysqlpp::StoreQueryResult res = query.store();
	    Row row;
	    mysqlpp::StoreQueryResult::iterator Iter; // Random Access Iterator

    	
	    for ( Iter = res.begin(); Iter != res.end(); Iter++ ) {
			row = *Iter;
			t0 = row[0];
			t1 = row[1];
	    }
	    
	    cout << "t0 = " << t0 << endl;
	    cout << "t1 = " << t1 << endl;
	    
	}		
		
		
	catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
			"\tretrieved data size: " << er.retrieved <<
			", actual size: " << er.actual_size << endl;
		return -2;
	}
	catch (const Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -3;
	}
	
	
	
	
	try {
		Connection conn(false);
		conn.connect("darpa_2_db", "127.0.0.1", "root");
		
		Query query = conn.query();
		
		int t_init = t0;
		int t_end = t0+DELTA_T;
		
		
		while (1) {
			
			
			float percent = ((float)(t_init-t0)/(float)DELTA_T)/((float)(t1-t0)/(float) DELTA_T);
			
			printf("Time = %d -> %d [%.1f %%]\n", t_init, t_end, 100.0 * percent);
			

			
			
			StoreQueryResult res1;
			float entropy;
			SimpleResult res2;
		
			// IP_SRC (ICMP, TCP, UDP)
			
			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"1\" and type_of_hist=\"1\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"1\",\"1\",\"" << entropy << "\");" ;
			res2 = query.execute();
			
			
			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"6\" and type_of_hist=\"1\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"6\",\"1\",\"" << entropy << "\");" ;
			res2 = query.execute();


			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"17\" and type_of_hist=\"1\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"17\",\"1\",\"" << entropy << "\");" ;
			res2 = query.execute();

			
			
			
			// SPORT (TCP, UDP)
			
			
			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"6\" and type_of_hist=\"2\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"6\",\"2\",\"" << entropy << "\");" ;
			res2 = query.execute();


			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"17\" and type_of_hist=\"2\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"17\",\"2\",\"" << entropy << "\");" ;
			res2 = query.execute();


			// IP_DST (ICMP, TCP, UDP)
			
			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"1\" and type_of_hist=\"3\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"1\",\"3\",\"" << entropy << "\");" ;
			res2 = query.execute();
			
			
			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"6\" and type_of_hist=\"3\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"6\",\"3\",\"" << entropy << "\");" ;
			res2 = query.execute();


			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"17\" and type_of_hist=\"3\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"17\",\"3\",\"" << entropy << "\");" ;
			res2 = query.execute();

			
			
			
			// DPORT (TCP, UDP)
			
			
			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"6\" and type_of_hist=\"4\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"6\",\"4\",\"" << entropy << "\");" ;
			res2 = query.execute();


			query << "SELECT counter FROM hist_" << t_init << " WHERE proto=\"17\" and type_of_hist=\"4\";" ;
			res1 = query.store();
			entropy = hist_to_entropy(&res1);
			query << "INSERT INTO entropies VALUES (\"" << t_init << "\",\"17\",\"4\",\"" << entropy << "\");" ;
			res2 = query.execute();

			
			
			
			t_init += DELTA_T;
			t_end += DELTA_T;
			
			
			if (t_init >= t1)
				break;
			
    
		}
			
				
	}	
		
		
		
	catch (const BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const BadConversion& er) {
		cerr << "Conversion error: " << er.what() << endl <<
			"\tretrieved data size: " << er.retrieved <<
			", actual size: " << er.actual_size << endl;
		return -2;
	}
	catch (const Exception& er) {
		cerr << "Error: " << er.what() << endl;
		return -3;
	}
	
	
	return 0;

}