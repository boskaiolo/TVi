#include "hist_generator.h"


#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>


using namespace std;
using namespace mysqlpp;

// Initial and final timestamp
int t0;
int t1;

#define DELTA_T 60


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
		
		SimpleResult res;
		
		int t_init = t0;
		int t_end = t0+DELTA_T;
		
		
		while (1) {
			
			
			float percent = ((float)(t_init-t0)/(float)DELTA_T)/((float)(t1-t0)/(float) DELTA_T);
			
			printf("Time = %d -> %d [%.1f %%]\n", t_init, t_end, 100.0 * percent);
			
			
			
			query << "INSERT INTO timestamp_list VALUES (" << t_init << ");";
			res = query.execute();
			
			
			
			cout << query.error() << endl;
			
			
			query << "CREATE TABLE hist_" << t_init << " like hist_template;";
			res = query.execute();
			
			
			query << "INSERT INTO hist_" << t_init << " (SELECT "<< t_init<<", proto, 1, sip, count(0) as counter from pcap_darpa where timestamp>="<<t_init \
				<<" AND timestamp<"<<t_end<<" GROUP BY proto, sip) ORDER BY counter DESC;";

			res = query.execute();
			
			
			query << "INSERT INTO hist_" << t_init << " (SELECT "<< t_init<<", proto, 3, dip, count(0) as counter from pcap_darpa where timestamp>="<<t_init \
				<<" AND timestamp<"<<t_end<<" GROUP BY proto, dip) ORDER BY counter DESC;";
			res = query.execute();
			
			
			
			
			query << "INSERT INTO hist_" << t_init << " (SELECT "<< t_init<<", proto, 2, sport, count(0) as counter from pcap_darpa where timestamp>="<<t_init \
				<<" AND timestamp<"<<t_end<<" AND proto<>1 GROUP BY proto, sport) ORDER BY counter DESC;";
			res = query.execute();
			
			query << "INSERT INTO hist_" << t_init << " (SELECT "<< t_init<<", proto, 4, dport, count(0) as counter from pcap_darpa where timestamp>="<<t_init \
				<<" AND timestamp<"<<t_end<<" AND proto<>1 GROUP BY proto, dport) ORDER BY counter DESC;";
			res = query.execute();
			
			
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