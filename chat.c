#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

#define IP "127.0.0.1"
#define PORT 3000
#define MAX_CLIENT 1024
#define MAX_DATA 1024
#define MAX_EVENTS 10

/*
#define LISTEN_BACKLOG 256
#define ADD_EV(a, b)	if(add_ev(a, b) == -1) { pr_err("fail:add_ev"); exit(1); }
#define DEL_EV(a, b)	if(del_ev(a, b) == -1) { pr_err("fail:del_ev"); exit(1); }
const int max_ep_events = 256;

int epollfd;
int add_ev(int efd, int fd);
int del_ev(int efd, int fd);
int fcntl_setnb(int fd);
*/

struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, conn_sock, nfds, epollfd;
int setnonblocking(int sock);
//int do_use_fd(int efd, struct epoll_event ev);

static struct termios term_old;
void initTermios(void);
void resetTermios(void);

int launch_chat(void);
int launch_clients(int num_client);
int launch_server(void);
int get_server_status(void);
 
int
main(int argc, char *argv[])
{
    int ret = -1;
    int num_client;

    if ((argc != 2) && (argc != 3)) {
usage:  fprintf(stderr, "usage: %s a|m|s|c num_client\n", argv[0]);
        goto leave;
    }
    if ((strlen(argv[1]) != 1))
        goto usage;
    switch (argv[1][0]) {
      case 'a': if (argc != 3)
                    goto usage;
                if (sscanf(argv[2], "%d", &num_client) != 1)
                    goto usage;
                // Launch Automatic Clients
                ret = launch_clients(num_client);
                break;
      case 's': // Launch Server
                ret = launch_server();
                break;
      case 'm': // Read Server Status
                ret = get_server_status();
                break;
      case 'c': // Start_Interactive Chatting Session
                ret = launch_chat();
                break;
      default:
                goto usage;
    }
leave:
    return ret;
}

int
launch_chat(void)
{
    int clientSock;
    struct sockaddr_in serverAddr;
    fd_set rfds, wfds, efds;
    int ret = -1;
    char rdata[MAX_DATA];
    int i = 1;
    struct timeval tm;
    int n;
    socklen_t serverAddrSize = sizeof(serverAddr);
/*
    socklen_t len_saddr;
    int fd, fd_listener;
    int ret_recv, ret_poll;
    char *port, buf[1024];
    struct epoll_event *ep_events;

    struct addrinfo ai, *ai_ret;
    int rc_gai;
    memset(&ai, 0, sizeof(ai));
    ai.ai_family = A_INET;
    ai.ai_socktype = SOCK_STREAM;
    ai.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;

    if ((rc_gai = getaddrinfo(NULL, port, &ai, &ai_ret)) != 0 ){
        pr_err("fail:getaddrinfor():%s",gai_strerror(rc_gai));
        goto leave;
    }
    if ((fd_listener = socket(ai_ret->ai_family, ai_ret->ai_socktype, ai_ret->ai_protocol)) == -1){
        pr_err("fail:socket()");
        goto leave;
    }
    fcntl_setnb(fd_listener);
    if (bind(fd_listener, ai_ret->ai_addr, ai_ret->ai_addrlen) == -1){
        pr_err("fail:bind()");
        goto leave;
    }
    listen(fd_listener, LISTEN_BACKLOG);
    if ((epollfd = epoll_create(1)) == -1){
        goto leave;
    }
    if ((ep_events = calloc(max_ep_events, sizseof(struct epoll_event))) == NULL){
        goto leave;
    }
    ADD_EV(epollfd, fd_listener);
    while(1) {
        pr_out("epoll waiting");
        if ((ret_poll = epoll_wait(epollfd, ep_events, max_pen_files, -1)) == -1){
        //error
        }
        pr_out("epoll return (%d)", ret_poll);
        for(i=0; i<ret_poll; i++) {
            if (ep_events[i].events & EPOLLIN) {
                if(ep_events[i].data.fd == fd_listener) {
                    struct sockaddr_storage saddr_c:
                    while(1) {
                        len_saddr = sizeof(saddr_c);
                        fd = accept(fd_listener, (struct sockaddr *)&saddr_c, &len_addr);
                        if (fd == -1) {
                            if (errno == EAGAIN) {
                                break;
                            }
                        pr_err("Error get connectin from listen socket");
                        break;
                        }
                    }
                    fcntl_setnb(fd);
                    ADD_EV(epollfd, fd);
                    pr_out("accept: add socket (%d)", fd);
                }   
                continue;
            }
            if ((ret_recv = recv(ep_events[i].data.fd, buf, sizeof(buf), 0)) == -1) {
            //error
            }
            else {
                if (ret_recv == 0) {
                    pr_out("fd(%d) : Session closed", ep_events[i].data.fd);
                    DEL_EV(epollfd, ep_events[i].data.fd);
                }
                else {
                    pr_out("recv(fd= %d, n= %d) = %.*s", ep_events[i].data.fd, ret_recv, ret_recv, buf);
                }
            }
        }else if (ep_events[i].events & EPOLLPRI) {
            pr_out("EPOLLPRI : Urgent data detected");
            if ((ret_recv = recv(ep_events[i].data.fd, buf, 1, MSG_OOB)) == -1) {
                //error
            }
            pr_out("recv(fd=%d, n=1) = %.*s(OOB)", ep_events[i].data.fd, 1, buf);
        }else if (ep_events[i].events & EPOLLERR) {
            //error
        }else {
            pr_out("fd(%d) epoll event(%d) err(%s)", ep_events[i].data.fd, ep_events[i].events, strerror(errno));
        }
    }
   


*/
    if ((ret = clientSock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        goto leave;
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP);
    serverAddr.sin_port = htons(PORT);

    if ((ret = connect(clientSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))) {
        perror("connect");
        goto leave1;
    }
    printf("[CLIENT] Connected to %s\n", inet_ntoa(*(struct in_addr *)&serverAddr.sin_addr));

    initTermios();

	epollfd = epoll_create(10);
	if (epollfd == -1) {
		perror("epoll_create");
		goto leave1;
	}

	ev.events = EPOLLIN;
	ev.data.fd = listen_sock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
		perror("epoll_ctl: listen_sock");
		goto leave1;
	}

	for (;;) {
		if ((nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1)) == -1) {
			perror("epoll_pwait");
			goto leave1;
		}
		for (n = 0; n < nfds; ++n) {
			if (events[n].data.fd == listen_sock) {
				conn_sock = accept(listen_sock, (struct sockaddr*)&serverAddr, &serverAddrSize);
				if (conn_sock == -1) {
					perror("accept");
					goto leave1;
				}
				setnonblocking(conn_sock);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = conn_sock;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
					perror("epoll_ctl: conn_sock");
					goto leave1;
				}
			}
//			else {
//				do_use_fd(epollfd,events[n].data.fd);
//			}
		}
	}
/*
    // start select version of chatting ...
    i = 1;
    ioctl(0, FIONBIO, (unsigned long *)&i);
    if ((ret = ioctl(clientSock, FIONBIO, (unsigned long *)&i))) {
        perror("ioctlsocket");
        goto leave1;
    }

    tm.tv_sec = 0; tm.tv_usec = 1000;
    while (1) {
        FD_ZERO(&rfds); FD_ZERO(&wfds); FD_ZERO(&efds);
        //FD_SET(clientSock, &wfds);
        FD_SET(clientSock, &rfds);
        FD_SET(clientSock, &efds);
        FD_SET(0, &rfds);

        if ((ret = select(clientSock + 1, &rfds, &wfds, &efds, &tm)) < 0) {
            perror("select");
            goto leave1;
        } else if (!ret)	// nothing happened within tm
            continue;
        if (FD_ISSET(clientSock, &efds)) {
            printf("Connection closed\n");
            goto leave1;
        }
        if (FD_ISSET(clientSock, &rfds)) {
            if (!(ret = recv(clientSock, rdata, MAX_DATA, 0))) {
                printf("Connection closed by remote host\n");
                goto leave1;
            } else if (ret > 0) {
                for (i = 0; i < ret; i++) {
                    printf("%c", rdata[i]);
                }
                fflush(stdout);
            } else
                break;
        }
        if (FD_ISSET(0, &rfds)) {
            int ch = getchar();
            if ((ret = send(clientSock, &ch, 1, 0)) < 0)
                goto leave1;
        }
    }
*/
leave1:
    resetTermios();
    close(clientSock);
leave:
    return -1;
}
int
launch_server(void)
{
    int serverSock, acceptedSock;
    struct sockaddr_in Addr;
    socklen_t AddrSize = sizeof(Addr);
    char data[MAX_DATA], *p;
    int ret, count, i = 1;

    if ((ret = serverSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        goto leave;
    }

    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (void *)&i, sizeof(i));

    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = INADDR_ANY;
    Addr.sin_port = htons(PORT);
    if ((ret = bind(serverSock, (struct sockaddr *)&Addr,sizeof(Addr)))) {
        perror("bind");
        goto error;
    }

    if ((ret = listen(serverSock, 1))) {
        perror("listen");
        goto error;
    }

    if ((acceptedSock = accept(serverSock, (struct sockaddr*)&Addr, &AddrSize)) < 0) {
        perror("accept");
        ret = -1;
        goto error;
    }
    printf("[SERVER] Connected to %s\n", inet_ntoa(*(struct in_addr *)&Addr.sin_addr));
    //close(serverSock);

    while (1) {
        if (!(ret = count = recv(acceptedSock, data, MAX_DATA, 0))) {
            fprintf(stderr, "Connect Closed by Client\n");
            break;
        }
        if (ret < 0) {
            perror("recv");
            break;
        }
        //printf("[%d]", count); fflush(stdout);
        for (i = 0; i < count; i++)
            printf("%c", data[i]);
        fflush(stdout);
        p = data;
        while (count) {
            if ((ret = send(acceptedSock, p, count, 0)) < 0) {
                perror("send");
                break;
            }
            count -= ret;
            p = p + ret;
        }
    }

    close(acceptedSock);
error:
    close(serverSock);
leave:
    return ret;
}

int
launch_clients(int num_client)
{
    return 0;
}

int
get_server_status(void)
{
    return 0;
}

/* Initialize new terminal i/o settings */
void
initTermios(void) 
{
    struct termios term_new;

    tcgetattr(0, &term_old); /* grab old terminal i/o settings */
    term_new = term_old; /* make new settings same as old settings */
    term_new.c_lflag &= ~ICANON; /* disable buffered i/o */
    term_new.c_lflag &= ~ECHO;   /* set no echo mode */
    tcsetattr(0, TCSANOW, &term_new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void
resetTermios(void) 
{
    tcsetattr(0, TCSANOW, &term_old);
}

int
setnonblocking(int sock)
{
    int flags = fcntl(sock, F_GETFL);
    flags |= O_NONBLOCK;
    if (fcntl(sock, F_SETFL, flags) < 0)
    {
        perror("fcntl, executing nonblock error");
        return -1;
    }
    return 0;
}
/*
int
do_use_fd(int efd, struct epoll_event ev)
{
    int readn;
    int sendflags = 0;
    char buf_in[1024] = {'\0'};
    int cfd = ev.data.fd;

#ifdef USE_ET
    int sizen = 0;
    char readbuf[256] = {'\0'};
    while(1){
        readn = read(cfd, readbuf, 255);
        if (readn < 0){
            if (EAGAIN == errno){
                break;
            }
            do_del_fd(efd,cfd);
            close(cfd);
          printf ("Close fd %d by %d\n", cfd,readn); 
          perror("Closed"); 
          return -1; 
    }    

    sizen += readn; 
        if (sizen >= 1024) { 
           do_del_fd(efd, cfd); 
           close (cfd); 
       printf ("Close buffer full fd %d by %d\n", cfd,readn); 
       perror("Closed"); 
       return -1; 
        } 
        memcpy(buf_in,readbuf,readn); 
  } 
  printf ("read data %d, %s", sizen, buf_in); 
  send (cfd, buf_in, strlen (buf_in), sendflags); 

#ifdef USE_ETONESHOT 
  // re-set fd to epoll 
  do_modify_fd(efd, cfd); 
#endif // USE_ETONESHOT 

#else //#ifdef USE_ET 

  //memset (buf_in, 0x00, 256); 
  readn = read (cfd, buf_in, 255); 
// if it occured ploblems with reading, delete from epoll event pool and close socket 
  if (readn <= 0) 
    { 
      do_del_fd(efd,cfd); 
      close (cfd); 
      printf ("Close fd ", cfd); 
    } 
  else 
    { 
      printf ("read data %s", buf_in); 
      send (cfd, buf_in, strlen (buf_in), sendflags); 
    } 
#endif //#ifdef USE_ET 
  return 1; 
} 
*/

int add_ev(int efd, int fd)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = fd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1){
        printf("fd(%d) EPOLL_CTL_ADD error(%d:%s)", fd, errno, strerror(errno));
        return -1;
    }
    return 0;
}

int del_ev(int efd, int fd){
    if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL) == -1){
        printf("fd(%d) EPOLL_CTL_DEL error(%d:%s)", fd, errno, strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}

int fcntl_setnb(int fd){
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL | O_NONBLOCK) == -1)){
        return errno;
    }
    return 0;
}

