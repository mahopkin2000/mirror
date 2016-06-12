#include "memory.h"
#include "primitive.h"
#ifdef NT
#include "winsock2.h"
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include "errno.h"
#include "signal.h"

// socket type constants
static mmObjectPtr mm_sock_stream=(mmObjectPtr)0;
static mmObjectPtr mm_sock_dgram=(mmObjectPtr)0;
static mmObjectPtr mm_sock_raw=(mmObjectPtr)0;
static mmObjectPtr mm_sock_rdm=(mmObjectPtr)0;
static mmObjectPtr mm_sock_seqpacket=(mmObjectPtr)0;
static mmObjectPtr mm_proto_icmp=(mmObjectPtr)0;
static mmObjectPtr mm_proto_tcp=(mmObjectPtr)0;
static mmObjectPtr mm_proto_udp=(mmObjectPtr)0;
static mmObjectPtr mm_proto_raw=(mmObjectPtr)0;

static void _mm_init_constants() {
    mm_sock_stream=mm_new_integer(SOCK_STREAM);
    mm_inc_ref(mm_sock_stream);
    mm_sock_dgram=mm_new_integer(SOCK_DGRAM);
    mm_inc_ref(mm_sock_dgram);
    mm_sock_raw=mm_new_integer(SOCK_RAW);
    mm_inc_ref(mm_sock_raw);
    mm_sock_rdm=mm_new_integer(SOCK_RDM);
    mm_inc_ref(mm_sock_rdm);
    mm_sock_seqpacket=mm_new_integer(SOCK_SEQPACKET);
    mm_inc_ref(mm_sock_seqpacket);
    mm_proto_icmp=mm_new_integer(IPPROTO_ICMP);
    mm_inc_ref(mm_proto_icmp);
    mm_proto_tcp=mm_new_integer(IPPROTO_TCP);
    mm_inc_ref(mm_proto_tcp);
    mm_proto_udp=mm_new_integer(IPPROTO_UDP);
    mm_inc_ref(mm_proto_udp);
    mm_proto_raw=mm_new_integer(IPPROTO_RAW);
    mm_inc_ref(mm_proto_raw);
}

static int _mm_sock_stream(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_sock_stream;
    return 0;
}

static int _mm_sock_dgram(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_sock_dgram;
    return MM_PRIM_STD_RETURN;
}

static int _mm_sock_row(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_sock_raw;
    return MM_PRIM_STD_RETURN;
}

static int _mm_sock_rdm(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_sock_rdm;
    return MM_PRIM_STD_RETURN;
}

static int _mm_sock_seqpacket(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_sock_seqpacket;
    return MM_PRIM_STD_RETURN;
}

// socket protocol constants
static int _mm_iproto_icmp(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_proto_icmp;
    return MM_PRIM_STD_RETURN;
}

static int _mm_iproto_tcp(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_proto_tcp;
    return MM_PRIM_STD_RETURN;
}

static int _mm_iproto_udp(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_proto_udp;
    return MM_PRIM_STD_RETURN;
}

static int _mm_iproto_raw(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    *rv=mm_proto_raw;
    return MM_PRIM_STD_RETURN;
}

// main socket system calls
static int _mm_open(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock;
    int type=mm_get_integer(mm_get_slot(args,0));
    int proto=mm_get_integer(mm_get_slot(args,1));
    sock=socket(AF_INET,mm_get_integer(mm_get_slot(args,0)),mm_get_integer(mm_get_slot(args,1)));
    if (sock < 0) {
        return MM_PRIM_FAILURE;
    } else {
        *rv=mm_new_integer(sock);
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_close(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    *rv=mm_new_integer(close(sock));
    return MM_PRIM_STD_RETURN;
}

static int _mm_bind(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    char* address=mm_get_string(mm_get_slot(args,1));
    u_short port=(u_short)mm_get_integer(mm_get_slot(args,2));
    struct sockaddr_in saddr;
    int rc;
    struct hostent* hentry;

    bzero((void*)&saddr,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);
    if ( (saddr.sin_addr.s_addr=inet_addr(address))==INADDR_NONE) {
        hentry=gethostbyname(address);
        if (hentry==NULL) {
            return MM_PRIM_FAILURE;
        } else {
            memcpy(&saddr.sin_addr,hentry->h_addr_list[0],hentry->h_length);
        }
    }
    rc=bind(sock,(const struct sockaddr*)&saddr,sizeof(saddr));
    if (rc < 0) {
        return MM_PRIM_FAILURE;
    } else {
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_connect(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    char* address=mm_get_string(mm_get_slot(args,1));
    u_short port=(u_short)mm_get_integer(mm_get_slot(args,2));
    struct sockaddr_in saddr;
    int rc;
    struct hostent* hentry;

    bzero((void*)&saddr,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);
    if ( (saddr.sin_addr.s_addr=inet_addr(address))==INADDR_NONE) {
        hentry=gethostbyname(address);
        if (hentry==NULL) {
            return MM_PRIM_FAILURE;
        } else {
            memcpy(&saddr.sin_addr,hentry->h_addr_list[0],hentry->h_length);
        }
    }
    rc=connect(sock,(const struct sockaddr*)&saddr,sizeof(saddr));
    if (rc < 0) {
        return MM_PRIM_FAILURE;
    } else {
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_listen(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    int backlog=mm_get_integer(mm_get_slot(args,1));
    int rc;
    rc=listen(sock,backlog);
    if (rc < 0) {
        return MM_PRIM_FAILURE;
    } else {
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_accept(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    mmObjectPtr sa=mm_get_slot(args,1);
    struct sockaddr_in saddr;
    int len=sizeof(saddr);
    int rc;
    char *address;
    sigset_t blockedSignals;

    sigfillset(&blockedSignals);
    sigprocmask (SIG_UNBLOCK, &blockedSignals, NULL);
    rc=accept(sock,(struct sockaddr*)&saddr,&len);
    sigprocmask (SIG_BLOCK, &blockedSignals, NULL);
    if (rc < 0) {
        return MM_PRIM_FAILURE;
    } else {
        address=(char*)inet_ntoa(saddr.sin_addr);
        mm_store_slot(sa,0,mm_new_string(address));
        mm_store_slot(sa,1,mm_new_integer(saddr.sin_port));
        *rv=mm_new_integer(rc);
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_writen(int fd,void* buff,size_t count) {
    int bytesLeft=count,bytesWritten=0;
    while (bytesLeft>0) {
        bytesWritten=write(fd,buff,bytesLeft);
        if (bytesWritten <= 0) {
            return  bytesWritten;
        }
        bytesLeft-=bytesWritten;
        buff+=bytesWritten;
    }
    return (count-bytesLeft);
}

static int _mm_readn(int fd,void* buff,size_t count) {
    int bytesLeft=count,bytesRead=0;
    while (bytesLeft > 0) {
        bytesRead=read(fd,buff,bytesLeft);
        if (bytesRead < 0) {
            return bytesRead;
        } else if (bytesRead==0) {
            break;
        }
        bytesLeft-=bytesRead;
        buff+=bytesRead;
    }
    return (count-bytesLeft);
}

static int _mm_primWriteFrom(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    char* str=mm_get_string(mm_get_slot(args,1));
    int pos=mm_get_integer(mm_get_slot(args,2));
    int bytes=mm_get_integer(mm_get_slot(args,3));
    int numBytes;
    numBytes=_mm_writen(sock,((void*)str+pos),bytes);
    if (numBytes <= 0) {
        return MM_PRIM_FAILURE;
    } else {
        *rv=mm_new_integer(numBytes);
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_primWriteChar(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    char c=mm_get_character(mm_get_slot(args,1));
    char buff[1];
    int numBytes;
    buff[0]=c;
    numBytes=_mm_writen(sock,buff,1);
    if (numBytes <= 0) {
        return MM_PRIM_FAILURE;
    } else {
        *rv=mm_new_integer(numBytes);
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_primReadChar(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    char buff[1];
    int numBytes;
    numBytes=_mm_readn(sock,buff,1);
    if (numBytes > 0) {    
        *rv=mm_new_character(buff[0]);
        return MM_PRIM_STD_RETURN;
    } else if (numBytes==0) {
        *rv=mm_nil;
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

static int _mm_primReadInto(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    mmByte* ba=mm_get_raw_bytes(mm_get_slot(args,1));
    int pos=mm_get_integer(mm_get_slot(args,2));
    int bytes=mm_get_integer(mm_get_slot(args,3));
    int numBytes;
    numBytes=_mm_readn(sock,ba+pos,bytes);
    if (numBytes >= 0) {
        *rv=mm_new_integer(numBytes);
        return MM_PRIM_STD_RETURN;
    } else {
        return MM_PRIM_FAILURE;
    }
}

static int _mm_htonl(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    unsigned long int nl=htonl(mm_get_integer(mm_get_slot(args,0)));
    mmObjectPtr ba=mm_new_byte_array(mm_bytearray_class,
            sizeof(unsigned long int));
    memcpy(mm_get_raw_bytes(ba),&nl,sizeof(unsigned long int));
    *rv=ba;
    return MM_PRIM_STD_RETURN;
}

static int _mm_htons(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    unsigned short int ns=htons(mm_get_integer(mm_get_slot(args,0)));
    mmObjectPtr ba=mm_new_byte_array(mm_bytearray_class,
            sizeof(unsigned short int));
    memcpy(mm_get_raw_bytes(ba),&ns,sizeof(unsigned short int));
    *rv=ba;
    return MM_PRIM_STD_RETURN;
}

static int _mm_ntohl(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    unsigned long int nl;
    mmObjectPtr ba=mm_get_slot(args,0);
    memcpy(&nl,mm_get_raw_bytes(ba),sizeof(unsigned long int));
    *rv=mm_new_integer(ntohl(nl));
    return MM_PRIM_STD_RETURN;
}

static int _mm_ntohs(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    unsigned short int ns;
    mmObjectPtr ba=mm_get_slot(args,0);
    memcpy(&ns,mm_get_raw_bytes(ba),sizeof(unsigned short int));
    *rv=mm_new_integer(ntohs(ns));
    return MM_PRIM_STD_RETURN;
}

static int _mm_getpeername(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    mmObjectPtr sa=mm_get_slot(args,1);
    struct sockaddr_in saddr;
    int len=sizeof(saddr);
    int rc;
    char* address;
    rc=getpeername(sock,(struct sockaddr*)&saddr,&len);
    if (rc < 0) {
        return MM_PRIM_FAILURE;
    } else {
        address=(char*)inet_ntoa(saddr.sin_addr);
        mm_store_slot(sa,0,mm_new_string(address));
        mm_store_slot(sa,1,mm_new_integer(ntohs(saddr.sin_port)));
        *rv=mm_new_integer(rc);
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_getsockname(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    int sock=mm_get_integer(mm_get_slot(args,0));
    mmObjectPtr sa=mm_get_slot(args,1);
    struct sockaddr_in saddr;
    int len=sizeof(saddr);
    int rc;
    char* address;
    rc=getsockname(sock,(struct sockaddr*)&saddr,&len);
    if (rc < 0) {
        return MM_PRIM_FAILURE;
    } else {
        address=(char*)inet_ntoa(saddr.sin_addr);
        mm_store_slot(sa,0,mm_new_string(address));
        mm_store_slot(sa,1,mm_new_integer(ntohs(saddr.sin_port)));
        *rv=mm_new_integer(rc);
        return MM_PRIM_STD_RETURN;
    }
}

static int _mm_gethostbyname(mmObjectPtr context,mmObjectPtr rcvr,mmObjectPtr args,mmObjectPtr* rv) {
    char* host=mm_get_string(mm_get_slot(args,0));
    struct hostent* hentry;
    char* address;
    struct sockaddr_in saddr;

    hentry=gethostbyname(host);
    if (hentry==NULL) {
        return MM_PRIM_FAILURE;
    } else {
        memcpy(&saddr.sin_addr,hentry->h_addr_list[0],hentry->h_length);
        address=(char*)inet_ntoa(saddr.sin_addr);
        *rv=mm_new_string(address);
        return MM_PRIM_STD_RETURN;
    }
}

void initsocket(mmObjectPtr cl) {
    mmObjectPtr mc=cl->_class;

    static mmPrimMethod socket_cl_primitives[]={
            {"primOpen",_mm_open},
            {"primClose",_mm_close},
            {"primBind",_mm_bind},
            {"primConnect",_mm_connect},
            {"primListen",_mm_listen},
            {"primAccept",_mm_accept},
            {"primWriteFrom",_mm_primWriteFrom},
            {"primWriteChar",_mm_primWriteChar},
            {"primReadChar",_mm_primReadChar},
            {"primReadInto",_mm_primReadInto},
            {"primGetSockName",_mm_getsockname},
            {"primGetPeerName",_mm_getpeername},
            {NULL,NULL}
    };
    static mmPrimMethod socket_mc_primitives[]={
            {"sockStream",_mm_sock_stream},
            {"sockDGram",_mm_sock_dgram},
            {"sockRaw",_mm_sock_row},
            {"sockRDM",_mm_sock_rdm},
            {"sockSeqPacket",_mm_sock_seqpacket},
            {"protoICMP",_mm_iproto_icmp},
            {"protoTCP",_mm_iproto_tcp},
            {"protoUDP",_mm_iproto_udp},
            {"protoRaw",_mm_iproto_raw},
            {"htonl",_mm_htonl},
            {"htons",_mm_htons},
            {"ntohl",_mm_ntohl},
            {"ntohs",_mm_ntohs},
            {"getHostByName",_mm_gethostbyname},
            {NULL,NULL}
    };
    _mm_init_constants();
    mm_map_primitives(cl,socket_cl_primitives);
    mm_map_primitives(mc,socket_mc_primitives);
}


