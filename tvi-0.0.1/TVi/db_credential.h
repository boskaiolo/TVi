#ifndef DB_CREDENTIAL_H
#define DB_CREDENTIAL_H




#define DARPA2
//#define UNIBS_60_seconds


#define WIRESHARK_COMMAND "/Users/AlbertoBoschetti/Develop/Wireshark_command_line/wireshark"
#define ETHERAPE_COMMAND "/usr/local/bin/etherape"
#define MERGECAP_COMMAND "/Users/AlbertoBoschetti/Develop/Wireshark_command_line/mergecap"




#define DB_ip2coord "hostip"



#ifdef UNIBS

#define DB_host "localhost"
#define DB_name "UNIBS_db"
#define DB_user "root"
#define DB_pass ""
#define DB_PCAP_TABLE "pcap_unibs"
#define DB_DELTA_HIST 300

#define GPS_LATITUDE 45.55
#define GPS_LONGITUDE 10.25

#define PCAP_PATH "/Volumes/bsk/Boschetti/unibs_traces/split_300/"

#endif



#ifdef UNIBS_60_seconds

#define DB_host "localhost"
#define DB_name "UNIBS_2_db"
#define DB_user "root"
#define DB_pass ""
#define DB_PCAP_TABLE "pcap_unibs"
#define DB_DELTA_HIST 60

#define GPS_LATITUDE 45.55
#define GPS_LONGITUDE 10.25

#define PCAP_PATH "/Volumes/bsk/Boschetti/unibs_traces/split_60/"

#endif




#ifdef DARPA2

#define DB_host "localhost"
#define DB_name "darpa_2"
#define DB_user "root"
#define DB_pass ""
#define DB_PCAP_TABLE "pcap_darpa"
#define DB_DELTA_HIST 600

#define GPS_LATITUDE 42.4574
#define GPS_LONGITUDE -71.2178

#define PCAP_PATH "/Volumes/bsk/Boschetti/darpa_traces/split_600"

#endif



#endif // DB_CREDENTIAL_H
