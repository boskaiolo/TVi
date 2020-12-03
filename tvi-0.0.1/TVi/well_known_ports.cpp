#include "well_known_ports.h"

#include <iostream>
#include <string>


std::string port2service(int port)
{

    switch(port)
    {
        case 0 : return "		UDP	Reserved	Official";
        case 1 : return "	TCP	UDP	TCP Port Service Multiplexer	Official";
        case 2 : return "	TCP	UDP	Management Utility	Official";
        case 3 : return "	TCP	UDP	Compression Process	Official";
        case 4 : return "	TCP	UDP	Unassigned	Official";
        case 5 : return "	TCP	UDP	Remote Job Entry	Official";
        case 6 : return "	TCP	UDP	Unassigned	Official";
        case 7 : return "	TCP	UDP	Echo	Official";
        case 8 : return "	TCP	UDP	Unassigned	Official";
        case 9 : return "	TCP	UDP	Discard	Official";
        case 10 : return "	TCP	UDP	Unassigned	Official";
        case 11 : return "	TCP	UDP	Active Users	Official";
        case 12 : return "	TCP	UDP	Unassigned	Official";
        case 13 : return "	TCP	UDP	DAYTIME – (RFC 867)	Official";
        case 14 : return "	TCP	UDP	Unassigned	Official";
        case 16 : return "	TCP	UDP	Unassigned	Official";
        case 17 : return "	TCP	UDP	Quote of the Day	Official";
        case 18 : return "	TCP	UDP	Message Send Protocol	Official";
        case 19 : return "	TCP	UDP	Character Generator	Official";
        case 20 : return "	TCP		FTP – data transfer	Official";
        case 21 : return "	TCP		FTP – control (command)	Official";
        case 22 : return "	TCP	UDP	Secure Shell (SSH)—used for secure logins, file transfers (scp, sftp) and port forwarding	Official";
        case 23 : return "	TCP		Telnet protocol—unencrypted text communications	Official USA only";
        case 24 : return "	TCP	UDP	Priv-mail  : return any private mail system.	Official";
        case 25 : return "	TCP		Simple Mail Transfer Protocol (SMTP)—used for e-mail routing between mail servers	Official";
        case 34 : return "	TCP	UDP	Remote File (RF)—used to transfer files between machines	Unofficial";
        case 35 : return "	TCP	UDP	Any private printer server protocol	Official";
        case 37 : return "	TCP	UDP	TIME protocol	Official";
        case 39 : return "	TCP	UDP	Resource Location Protocol[2] (RLP)—used for determining the location of higher level services from hosts on a network	Official";
        case 41 : return "	TCP	UDP	Graphics	Official";
        case 42 : return "	TCP	UDP	nameserver, ARPA Host Name Server Protocol	Official";
        case 43 : return "	TCP		WHOIS protocol	Official";
        case 47 : return "	TCP		GRE protocol	Official";
        case 49 : return "	TCP	UDP	TACACS Login Host protocol	Official";
        case 50 : return "	TCP	UDP	Encapsulating Security Payload (ESP)	Official";
        case 51 : return "	TCP	UDP	Authentication Header (AH)	Official";
        case 52 : return "	TCP	UDP	XNS (Xerox Network Systems) Time Protocol	Official";
        case 53 : return "	TCP	UDP	Domain Name System (DNS)	Official";
        case 54 : return "	TCP	UDP	XNS (Xerox Network Systems) Clearinghouse	Official";
        case 55 : return "	TCP	UDP	ISI Graphics Language (ISI-GL)	Unofficial";
        case 56 : return "	TCP	UDP	XNS (Xerox Network Systems) Authentication	Official";
        case 57 : return "	TCP		Mail Transfer Protocol (MTP)	Unofficial";
        case 58 : return "	TCP	UDP	XNS (Xerox Network Systems) Mail	Official";
        case 67 : return "		UDP	Bootstrap Protocol (BOOTP) Server; also used by Dynamic Host Configuration Protocol (DHCP)	Official";
        case 68 : return "		UDP	Bootstrap Protocol (BOOTP) Client; also used by Dynamic Host Configuration Protocol (DHCP)	Official";
        case 69 : return "		UDP	Trivial File Transfer Protocol (TFTP)	Official";
        case 70 : return "	TCP		Gopher protocol	Official";
        case 79 : return "	TCP		Finger protocol	Official";
        case 80 : return "	TCP	UDP	Hypertext Transfer Protocol (HTTP)	Official";
        case 81 : return "	TCP		Torpark—Onion routing	Unofficial";
        case 82 : return "		UDP	Torpark—Control	Unofficial";
        case 83 : return "	TCP		MIT ML Device	Official";
        case 88 : return "	TCP	UDP	Kerberos—authentication system	Official";
        case 90 : return "	TCP	UDP	dnsix (DoD Network Security for Information Exchange) Securit Attribute Token Map	Official";
        case 99 : return "	TCP		WIP Message Protocol	Unofficial";
        case 101 : return "	TCP		NIC host name	Official";
        case 102 : return "	TCP		ISO-TSAP (Transport Service Access Point) Class 0 protocol[4]	Official";
        case 104 : return "	TCP	UDP	ACR/NEMA Digital Imaging and Communications in Medicine	Official";
        case 105 : return "	TCP	UDP	CCSO Nameserver Protocol (Qi/Ph)	Official";
        case 107 : return "	TCP		Remote TELNET Service[5] protocol	Official";
        case 108 : return "	TCP	UDP	SNA Gateway Access Server [6]	Official";
        case 109 : return "	TCP		Post Office Protocol 2 (POP2)	Official";
        case 110 : return "	TCP		Post Office Protocol 3 (POP3)	Official";
        case 111 : return "	TCP	UDP	ONC RPC (SunRPC)	Official";
        case 113 : return "	TCP	UDP	Authentication Service (auth)	Official";
        case 115 : return "	TCP		Simple File Transfer Protocol (SFTP)	Official";
        case 117 : return "	TCP		UUCP Path Service	Official";
        case 118 : return "	TCP	UDP	SQL (Structured Query Language) Services	Official";
        case 119 : return "	TCP		Network News Transfer Protocol (NNTP)—used for retrieving newsgroup messages	Official";
        case 123 : return "		UDP	Network Time Protocol (NTP)—used for time synchronization	Official";
        case 135 : return "	TCP	UDP	Microsoft EPMAP (End Point Mapper), also known as DCE/RPC Locator service[7], used to remotely manage services including DHCP server, DNS server and WINS. Also used by DCOM	Unofficial";
        case 137 : return "	TCP	UDP	NetBIOS NetBIOS Name Service	Official";
        case 138 : return "	TCP	UDP	NetBIOS NetBIOS Datagram Service	Official";
        case 139 : return "	TCP	UDP	NetBIOS NetBIOS Session Service	Official";
        case 143 : return "	TCP	UDP	Internet Message Access Protocol (IMAP)—used for retrieving, organizing, and synchronizing e-mail messages	Official";
        case 152 : return "	TCP	UDP	Background File Transfer Program (BFTP)[8]	Official";
        case 153 : return "	TCP	UDP	SGMP, Simple Gateway Monitoring Protocol	Official";
        case 156 : return "	TCP	UDP	SQL Service	Official";
        case 158 : return "	TCP	UDP	DMSP, Distributed Mail Service Protocol	Unofficial";
        case 161 : return "		UDP	Simple Network Management Protocol (SNMP)	Official";
        case 162 : return "	TCP	UDP	Simple Network Management Protocol Trap (SNMPTRAP)[9]	Official";
        case 170 : return "	TCP		Print-srv, Network PostScript	Official";
        case 177 : return "	TCP	UDP	X Display Manager Control Protocol (XDMCP)	Official";
        case 179 : return "	TCP		BGP (Border Gateway Protocol)	Official";
        case 194 : return "	TCP	UDP	IRC (Internet Relay Chat)	Official";
        case 199 : return "	TCP	UDP	SMUX, SNMP Unix Multiplexer	Official";
        case 201 : return "	TCP	UDP	AppleTalk Routing Maintenance	Official";
        case 209 : return "	TCP	UDP	The Quick Mail Transfer Protocol	Official";
        case 210 : return "	TCP	UDP	ANSI Z39.50	Official";
        case 213 : return "	TCP	UDP	Internetwork Packet Exchange (IPX)	Official";
        case 218 : return "	TCP	UDP	Message posting protocol (MPP)	Official";
        case 220 : return "	TCP	UDP	Internet Message Access Protocol (IMAP), version 3	Official";
        case 256 : return "	TCP	UDP	2DEV 2SP Port	Unofficial";
        case 259 : return "	TCP	UDP	ESRO, Efficient Short Remote Operations	Official";
        case 264 : return "	TCP	UDP	BGMP, Border Gateway Multicast Protocol	Official";
        case 308 : return "	TCP		Novastor Online Backup	Official";
        case 311 : return "	TCP		Mac OS X Server Admin (officially AppleShare IP Web administration)	Official";
        case 318 : return "	TCP	UDP	PKIX TSP, Time Stamp Protocol	Official";
        case 319 : return "		UDP	Precision time protocol event messages	Official";
        case 320 : return "		UDP	Precision time protocol general messages	Official";
        case 323 : return "	TCP	UDP	IMMP, Internet Message Mapping Protocol	Unofficial";
        case 350 : return "	TCP	UDP	MATIP-Type A, Mapping of Airline Traffic over Internet Protocol	Official";
        case 351 : return "	TCP	UDP	MATIP-Type B, Mapping of Airline Traffic over Internet Protocol	Official";
        case 366 : return "	TCP	UDP	ODMR, On-Demand Mail Relay	Official";
        case 369 : return "	TCP	UDP	Rpc2portmap	Official";
        case 370 : return "	TCP	UDP	codaauth2 – Coda authentication server	Unofficial";
        case 371 : return "	TCP	UDP	ClearCase albd	Official";
        case 383 : return "	TCP	UDP	HP data alarm manager	Official";
        case 384 : return "	TCP	UDP	A Remote Network Server System	Official";
        case 387 : return "	TCP	UDP	AURP, AppleTalk Update-based Routing Protocol	Official";
        case 389 : return "	TCP	UDP	Lightweight Directory Access Protocol (LDAP)	Official";
        case 401 : return "	TCP	UDP	UPS Uninterruptible Power Supply	Official";
        case 402 : return "	TCP		Altiris, Altiris Deployment Client	Unofficial";
        case 411 : return "	TCP		Direct Connect Hub	Unofficial";
        case 412 : return "	TCP		Direct Connect Client-to-Client	Unofficial";
        case 427 : return "	TCP	UDP	Service Location Protocol (SLP)	Official";
        case 443 : return "	TCP		HTTPS (Hypertext Transfer Protocol over SSL/TLS)	Official";
        case 444 : return "	TCP	UDP	SNPP, Simple Network Paging Protocol (RFC 1568)	Official";
        case 445 : return "	TCP		Microsoft-DS Active Directory, Windows shares	Official";
        case 464 : return "	TCP	UDP	Kerberos Change/Set password	Official";
        case 465 : return "	TCP		SMTP over SSL	Unofficial";
        case 475 : return "	TCP		tcpnethaspsrv (Aladdin Knowledge Systems Hasp services, TCP/IP version)	Official";
        case 497 : return "	TCP		Dantz Retrospect	Official";
        case 500 : return "	_	UDP	Internet Security Association and Key Management Protocol (ISAKMP)	Official";
        case 501 : return "	TCP		STMF, Simple Transportation Management Framework – DOT NTCIP 1101	Unofficial";
        case 502 : return "	TCP	UDP	Modbus, Protocol	Unofficial";
        case 504 : return "	TCP	UDP	Citadel – multiservice protocol for dedicated clients for the Citadel groupware system	Official";
        case 510 : return "	TCP		First Class Protocol	Unofficial";
        case 512 : return "	TCP		Rexec, Remote Process Execution	Official";
        case 513 : return "	TCP	UDP	rlogin	Official/ Who";
        case 514 : return "		UDP	Syslog—used for system logging	Official";
        case 515 : return "	TCP		Line Printer Daemon—print service	Official";
        case 517 : return "		UDP	Talk	Official";
        case 518 : return "		UDP	NTalk	Official";
        case 520 : return "		UDP	Routing Information Protocol (RIP)	Official";
        case 524 : return "	TCP	UDP	NetWare Core Protocol (NCP) is used for a variety things such as access to primary NetWare server resources, Time Synchronization, etc.	Official";
        case 525 : return "		UDP	Timed, Timeserver	Official";
        case 530 : return "	TCP	UDP	RPC	Official";
        case 531 : return "	TCP	UDP	AOL Instant Messenger, IRC	Unofficial";
        case 532 : return "	TCP		netnews	Official";
        case 533 : return "		UDP	netwall, For Emergency Broadcasts	Official";
        case 540 : return "	TCP		UUCP (Unix-to-Unix Copy Protocol)	Official";
        case 542 : return "	TCP	UDP	commerce (Commerce Applications)	Official";
        case 543 : return "	TCP		klogin, Kerberos login	Official";
        case 544 : return "	TCP		kshell, Kerberos Remote shell	Official";
        case 545 : return "	TCP		OSIsoft PI (VMS), OSISoft PI Server Client Access	Unofficial";
        case 546 : return "	TCP	UDP	DHCPv6 client	Official";
        case 547 : return "	TCP	UDP	DHCPv6 server	Official";
        case 548 : return "	TCP		Apple Filing Protocol (AFP) over TCP	Official";
        case 550 : return "		UDP	new-rwho, new-who	Official";
        case 554 : return "	TCP	UDP	Real Time Streaming Protocol (RTSP)	Official";
        case 556 : return "	TCP		Remotefs, RFS, rfs_server	Official";
        case 560 : return "		UDP	rmonitor, Remote Monitor	Official";
        case 561 : return "		UDP	monitor	Official";
        case 563 : return "	TCP	UDP	NNTP protocol over TLS/SSL (NNTPS)	Official";
        case 587 : return "	TCP		e-mail message submission[10] (SMTP)	Official";
        case 591 : return "	TCP		FileMaker 6.0 (and later) Web Sharing (HTTP Alternate, also see port 80)	Official";
        case 593 : return "	TCP	UDP	HTTP RPC Ep Map, Remote procedure call over Hypertext Transfer Protocol, often used by Distributed Component Object Model services and Microsoft Exchange Server	Official";
        case 604 : return "	TCP		TUNNEL profile[11], a protocol for BEEP peers to form an application layer tunnel	Official";
        case 623 : return "		UDP	ASF Remote Management and Control Protocol (ASF-RMCP)	Official";
        case 631 : return "	TCP	UDP	Internet Printing Protocol (IPP)	Official";
        case 635 : return "	TCP	UDP	RLZ DBase	Official";
        case 636 : return "	TCP	UDP	Lightweight Directory Access Protocol over TLS/SSL (LDAPS)	Official";
        case 639 : return "	TCP	UDP	MSDP, Multicast Source Discovery Protocol	Official";
        case 641 : return "	TCP	UDP	SupportSoft Nexus Remote Command (control/listening) : return A proxy gateway connecting remote control traffic	Official";
        case 646 : return "	TCP	UDP	LDP, Label Distribution Protocol, a routing protocol used in MPLS networks	Official";
        case 647 : return "	TCP		DHCP Failover protocol[12]	Official";
        case 648 : return "	TCP		RRP (Registry Registrar Protocol)[13]	Official";
        case 652 : return "	TCP		DTCP, Dynamic Tunnel Configuration Protocol	Unofficial";
        case 653 : return "	TCP	UDP	SupportSoft Nexus Remote Command (data) : return A proxy gateway connecting remote control traffic	Official";
        case 654 : return "	TCP		Media Management System (MMS) Media Management Protocol (MMP)[14]	Official";
        case 657 : return "	TCP	UDP	IBM RMC (Remote monitoring and Control) protocol, used by System p5 AIX Integrated Virtualization Manager (IVM)[15] and Hardware Management Console to connect managed logical partitions (LPAR) to enable dynamic partition reconfiguration	Official";
        case 660 : return "	TCP		Mac OS X Server administration	Official";
        case 665 : return "	TCP		sun-dr, Remote Dynamic Reconfiguration	Unofficial";
        case 666 : return "		UDP	Doom, first online first-person shooter	Official";
        case 674 : return "	TCP		ACAP (Application Configuration Access Protocol)	Official";
        case 691 : return "	TCP		MS Exchange Routing	Official";
        case 692 : return "	TCP		Hyperwave-ISP	Official";
        case 694 : return "	TCP	UDP	Linux-HA High availability Heartbeat	Official";
        case 695 : return "	TCP		IEEE-MMS-SSL (IEEE Media Management System over SSL)[16]	Official";
        case 698 : return "		UDP	OLSR (Optimized Link State Routing)	Official";
        case 699 : return "	TCP		Access Network	Official";
        case 700 : return "	TCP		EPP (Extensible Provisioning Protocol), a protocol for communication between domain name registries and registrars (RFC 5734)	Official";
        case 701 : return "	TCP		LMP (Link Management Protocol (Internet))[17], a protocol that runs between a pair of nodes and is used to manage traffic engineering (TE) links	Official";
        case 702 : return "	TCP		IRIS[18][19] (Internet Registry Information Service) over BEEP (Blocks Extensible Exchange Protocol)[20] (RFC 3983)	Official";
        case 706 : return "	TCP		Secure Internet Live Conferencing (SILC)	Official";
        case 711 : return "	TCP		Cisco Tag Distribution Protocol[21][22][23]—being replaced by the MPLS Label Distribution Protocol[24]	Official";
        case 712 : return "	TCP		Topology Broadcast based on Reverse-Path Forwarding routing protocol (TBRPF) (RFC 3684)	Official";
        case 720 : return "	TCP		SMQP, Simple Message Queue Protocol	Unofficial";
        case 749 : return "	TCP	UDP	Kerberos (protocol) administration	Official";
        case 750 : return "		UDP	kerberos-iv, Kerberos version IV	Official";
        case 751 : return "	TCP	UDP	kerberos_master, Kerberos authentication	Unofficial";
        case 753 : return "	TCP		Reverse Routing Header (rrh)[25]	Official";
        case 754 : return "	TCP	UDP	tell send	Official";
        case 760 : return "	TCP	UDP	ns	Official";
        case 782 : return "	TCP		Conserver serial-console management server	Unofficial";
        case 783 : return "	TCP		SpamAssassin spamd daemon	Unofficial";
        case 829 : return "	TCP		CMP (Certificate Management Protocol)	Unofficial";
        case 843 : return "	TCP		Adobe Flash socket policy server	Unofficial";
        case 860 : return "	TCP		iSCSI (RFC 3720)	Official";
        case 873 : return "	TCP		rsync file synchronisation protocol	Official USA only";
        case 888 : return "	TCP		cddbp, CD DataBase (CDDB) protocol (CDDBP)—unassigned but widespread use	Unofficial";
        case 901 : return "	TCP		Samba Web Administration Tool (SWAT)	Unofficial";
        case 902 : return "	TCP		ideafarm-door 902/tcp self documenting Door : return send 0x00 for info	Official";
        case 903 : return "	TCP		VMware Remote Console [26]	Unofficial";
        case 904 : return "	TCP		VMware Server Alternate (if 902 is in use, i.e. SUSE linux)	Unofficial";
        case 911 : return "	TCP		Network Console on Acid (NCA)—local tty redirection over OpenSSH	Unofficial";
        case 953 : return "	TCP	UDP	Domain Name System (DNS) RNDC Service	Unofficial";
        case 981 : return "	TCP		SofaWare Technologies Remote HTTPS management for firewall devices running embedded Check Point FireWall-1 software	Unofficial";
        case 989 : return "	TCP	UDP	FTPS Protocol (data) : return FTP over TLS/SSL	Official";
        case 990 : return "	TCP	UDP	FTPS Protocol (control) : return FTP over TLS/SSL	Official";
        case 991 : return "	TCP	UDP	NAS (Netnews Administration System)	Official";
        case 992 : return "	TCP	UDP	TELNET protocol over TLS/SSL	Official";
        case 993 : return "	TCP		Internet Message Access Protocol over SSL (IMAPS)	Official";
        case 995 : return "	TCP		Post Office Protocol 3 over TLS/SSL (POP3S)	Official";
        case 999 : return "	TCP		ScimoreDB Database System	Unofficial";
        case 1001 : return "	TCP		JtoMB	Unofficial";
        case 1002 : return "	TCP		Opsware agent (aka cogbot)	Unofficial";
        case 1023 : return "	TCP	UDP	Reserved[1]	Official";

    }

    return "unknown";

}
