#include <egt/ui>
#include <egt/uiloader.h>
#include <iostream>

#define T1S_ENABLE

#ifdef T1S_ENABLE 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_EVENTS 2

#define POLLFD_T1S	1

#define MAX_BUF_SIZE	128
unsigned char recvBuf[MAX_BUF_SIZE];
#endif

#define WORKING_TIMER		100

typedef enum __TURN_DIR__
{
	DIR_NONE=0,
	DIR_LEFT,
	DIR_RIGHT
}TURN_DIR;

int main(int argc, char** argv)
{
	// emulate speed
	int count = 0;
	int testSpeed = 0;

#ifdef T1S_ENABLE  	
	int dig1, dig2, dig3;
	TURN_DIR dir;
	// poll
	int t1s_nfds = 1;
	struct pollfd fds[MAX_EVENTS];
	
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	int retval;
	unsigned int len = sizeof(struct sockaddr_in);
	bool isUpdate = false;
	
	if( argc != 2 )
	{
		printf("Usage: udp_server PORT_NUMBER\r\n");
		return -1;
	}
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
		
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(atoi(argv[1]));
	
	// Bind the socket with the server address
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	
	fds[POLLFD_T1S].fd = sockfd;
	fds[POLLFD_T1S].events = POLLIN;
	
#endif // end of T1S_ENABLE

	egt::Application app(argc, argv);
	egt::experimental::UiLoader loader;
	
	auto window = std::static_pointer_cast<egt::Window>(loader.load("file:ui.xml"));

	auto iconCall = window->find_child<egt::ImageLabel>("imgCalling");
	auto iconSMS = window->find_child<egt::ImageLabel>("imgSMS");
	auto btnAccept = window->find_child<egt::ImageButton>("btnAccept");
	auto btnReject = window->find_child<egt::ImageButton>("btnReject");
	auto labelCaller = window->find_child<egt::Label>("labCaller");
	auto labelMessage = window->find_child<egt::Label>("labelMessage");
	auto iconBLE = window->find_child<egt::Label>("iconBLE");
	auto labSpeed1 = window->find_child<egt::Label>("labSpeed1");
	auto labSpeed2 = window->find_child<egt::Label>("labSpeed2");
	auto labSpeed3 = window->find_child<egt::Label>("LabSpeed3");
	
	auto labRight = window->find_child<egt::Label>("labRight");
	auto labLeft = window->find_child<egt::Label>("labLeft");
	
	iconCall->hide();
	iconSMS->hide();
	btnAccept->hide();
	btnReject->hide();
	labelCaller->hide();
	labelMessage->hide();
	iconBLE->hide();
	labSpeed1->hide();
	labSpeed2->hide();
	labSpeed3->hide();
	labRight->hide();
	labLeft->hide();
		
	window->show();
	
	egt::PeriodicTimer timer(std::chrono::milliseconds(WORKING_TIMER));
	timer.on_timeout([&]()
    {
#ifdef T1S_ENABLE    
    	auto t1s_receive = [&]() {
    		if( poll(fds, t1s_nfds, 0) >= 0 )
    		{
				int id;
				retval = recvfrom(sockfd, recvBuf, MAX_BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
				if( retval > 0 )
				{
					id = recvBuf[0];
					
					if( id == 20 )
					{
						dig1 = recvBuf[2]%10;
						dig2 = (recvBuf[2]%100)/10;
						dig3 = (recvBuf[2]%1000)/100;
						isUpdate = true;
					}
					else if( id == 30 )
					{
						dir = (TURN_DIR)recvBuf[3];
						isUpdate = true;
					}
					
					
				}
			}
    	};
    	
    	t1s_receive();
    	
    	if( isUpdate )
    	{
    		labSpeed1->text(std::to_string(dig1));
			labSpeed1->show();
			if( dig3 != 0 || dig2 != 0 )
			{
				labSpeed2->text(std::to_string(dig2));
				labSpeed2->show();
			}
			else
			{
				labSpeed2->hide();
			}
			if( dig3 != 0 )
			{
				labSpeed3->text(std::to_string(dig3));
				labSpeed3->show();
			}
			else
			{
				labSpeed3->hide();
			}
			
			switch(dir)
			{
				case DIR_NONE:
					labRight->hide();
					labLeft->hide();
					break;
					
				case DIR_RIGHT:
					labRight->show();
					labLeft->hide();
					break;
					
				case DIR_LEFT:
					labRight->hide();
					labLeft->show();
					break;
			}
			
    		isUpdate = false;
    	}
#endif // end of T1S_ENABLE
    });
    timer.start();

    return app.run();
}

