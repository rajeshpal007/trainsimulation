#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define TRUE 1
#define FALSE 0
#define MAX 300

/* Data Structure */
struct resources
{
    int resourceQuantity;
    char resourceName[MAX];
    struct resources *next;
};

struct station
{
    int  portNumber;
    char stationName[MAX];
    struct station *next;
};

struct station *stationList = NULL;
struct resources *resourceList = NULL;

/*Stat for log file*/
int processedTrainCountWithoutErrors = 0;
int discardedTrainCountWrongStation = 0;
int discardedTrainCountFormatErrors = 0;
int processedTrainCountInvalidNextStation = 0;

/*Log file for dumping */
FILE *flog = NULL;

/* Log file dumer function*/
int log_file_dumper()
{
    char stationName[]="dest";
    fprintf(flog,"======\n");
    fprintf(flog,"Name of the station: %s\n", stationName);
    fprintf(flog,"Processed:%d\n", processedTrainCountWithoutErrors);
    fprintf(flog,"Not mine:%d\n", discardedTrainCountWrongStation);
    fprintf(flog,"Format err:%d\n", discardedTrainCountFormatErrors);
    fprintf(flog,"No fwd:%d\n", processedTrainCountInvalidNextStation);
    fprintf(flog,"Lexicographic comma separated list of stations\n");
    fprintf(flog,"Lexicographic list of resources with have been loaded or unloaded at the station (one per line)\n");
    fprintf(flog,"doomtrain or stopstation\n");
    fclose(flog);
}

/* Signal handler for : sighup*/
void sighup()
{
    signal(SIGHUP,sighup); /* reset signal */
    printf("\n I have received a SIGHUP\n");
    log_file_dumper();
}

/*  Graceful shutdown fucntion. This is only for cases when shutdown is required. */
void shut_down(int errorNumber)
{
    switch( errorNumber)
    {
    case 0:
        /*TODO: Clean this case, here only log_file_dumper and exit will remain*/
        fprintf(stderr, "\nPrinting starts here -------------------END\n");
        display_all_stations(stationList);
        display_all_resources(resourceList);
        log_file_dumper();
        exit(0);
    case 1:
        fprintf(stderr, "\n Usage: station name authfile logfile [port [host]]");
        break;
    case 2:
        fprintf(stderr, "Invalid name/auth\n");
        break;
    case 3:
        fprintf(stderr, "Unable to open log\n");
        break;
    case 4:
        fprintf(stderr, "Invalid port\n");
        break;
    case 5:
        fprintf(stderr, "Listen error\n");
        break;
    case 6:
        fprintf(stderr, "Unable to connect to station\n");
        break;
    case 7:
        fprintf(stderr, "Duplicate station names\n");
        break;
    default:
        /* All remaining the error cases will redirect here.*/
        fprintf(stderr, "Unspecified system call failure\n");
        errorNumber = 99;
        break;
    }
    exit(errorNumber);
}

/*TODO: Remove this function as this is for cleaning user input, for our testing only*/
string_string_newline( char **srcString)
{
    int len = strlen(*srcString);
    fprintf(stderr, "\n Before == %sEND\n", *srcString);
    while ( len > 0 && *srcString[len - 1] == '\n' )
    {
        *srcString[len - 1] = '\0';
        --len;
    }
    fprintf(stderr, "\n After == %sEND\n", *srcString);
}


/* This function will insert resources in inverntory*/
/* TODO 1: Make it lexographical*/
/* TODO 2: Load and Unload instead of just insering a new node*/
struct resources *insert_resources(struct resources *front, int resourceQuantity, char resourceName[])
{
    struct resources *newnode;
//   fprintf(stderr, "\n insert_resources called with resourceQuantity %d resourceName %s",resourceQuantity, resourceName);
    newnode = (struct resources*)malloc(sizeof(struct resources));

    if (newnode == NULL)
    {
        fprintf(stderr, "\n Allocation failed \n");
        exit(2);
    }
    newnode->resourceQuantity = resourceQuantity;
    strcpy(newnode->resourceName, resourceName);
    newnode->next = front;
    front = newnode;
    return(front);
}

void printStation(struct station *p)
{
    fprintf(stderr, "\n Port Number  : %d", p->portNumber);
    fprintf(stderr, "  Station Name  : %sEND\n", p->stationName);
}

/* This function returns port number.*/
int search_station(struct station *front, char stationName[])
{
    struct station *ptr;

    for (ptr = front; ptr != NULL; ptr = ptr -> next)
    {

        fprintf(stderr, "\nsearch_station compare names:%s:%sEND\n",ptr->stationName, stationName);
        if (0 == strcmp(ptr->stationName, stationName) )
        {
            fprintf(stderr, "\n Key found: duplicate station nameEND\n");
            printStation(ptr);
            return ptr->portNumber;
        }
    }
    return 0;
}


/* Send doomtrain to all other connectd station.*/
int send_doomtrain_to_all_connected_stations(struct station *front)
{
    struct station *ptr;

    int sockSendDoomtrain, nSendDoomtrain;
    unsigned int lengthSendDoomtrain;
    struct sockaddr_in serverSendDoomtrain, fromSendDoomtrain;
    struct hostent *hpSendDoomtrain;
    char bufferSendDoomtrain[11]="doomtrain";
    sockSendDoomtrain= socket(AF_INET, SOCK_DGRAM, 0);
    if (sockSendDoomtrain < 0) error("socket");
    serverSendDoomtrain.sin_family = AF_INET;
    fprintf(stderr, "\ncalled send_doomtrain_to_all_connected_stations:END\n");

    hpSendDoomtrain = gethostbyname("127.0.0.1");
    if (0 ==hpSendDoomtrain) error("Unknown host");
    bcopy((char *)hpSendDoomtrain->h_addr,  (char *)&serverSendDoomtrain.sin_addr, hpSendDoomtrain->h_length);
    lengthSendDoomtrain=sizeof(struct sockaddr_in);

    for (ptr = front; ptr != NULL; ptr = ptr -> next)
    {
        /* TODOD  send_doomtrain() */
        printf("\nsearched station for doomtrain :%s:%d:END\n",ptr->stationName, ptr->portNumber);

        serverSendDoomtrain.sin_port = htons(ptr->portNumber);
        nSendDoomtrain=sendto(sockSendDoomtrain,bufferSendDoomtrain,
                              strlen(bufferSendDoomtrain),0,(const struct sockaddr *)&serverSendDoomtrain,lengthSendDoomtrain);
        if (nSendDoomtrain < 0)
            error("Sendto");
    }

    close(sockSendDoomtrain);
    return 0;
}

/* This function will insert station in list*/
/* TODO 1: Make it lexographical*/
struct station *insert_station(struct station *front, int portNum, char stationName[])
{

    struct station *newnode;
    int searchResult = 0;

    // if ( portNum <=0 || portNum > 65535 )
    //{
    //  shut_down(4);
//   }

    newnode = (struct station*)malloc(sizeof(struct station));
    if (newnode == NULL)
    {
        fprintf(stderr, "\n Allocation failed \n");
        exit(2);
    }

    // DONE : Check for duplicate station name
    searchResult = search_station(front, stationName);

    if ( 0 != searchResult)
        shut_down(7);

    newnode->portNumber = portNum;
    strcpy(newnode->stationName, stationName);
    newnode->next = front;
    front = newnode;
    return(front);
}

void printResource(struct resources *r)
{
    fprintf(stderr, "\n Resource quantity  : %d", r->resourceQuantity);
    fprintf(stderr, "  Resource Name  : %sEND\n", r->resourceName);
}

void display_all_stations(struct station  *front)
{
    struct station *ptr;

    printf("\nPrinting all Station :END\n");
    for (ptr = front; ptr != NULL; ptr = ptr->next)
    {
        printStation(ptr);
    }
}

void display_all_resources(struct resources  *front)
{
    struct resources *ptr;
    for (ptr = front; ptr != NULL; ptr = ptr->next)
    {
        printResource(ptr);
    }
}

int state; /* 0 doomtrain  , 1 stoptrain , 2 add , 3 resourses */
int send_doomtrain(void);
int stop_stop_station(void);

/* This function process all trains recieved at current station*/
/* TODO: call this function in a new thread; there may be many messages */

train_processing(char * trainMessage)
{
    char string[]="dest:doomtrain:stopstation:add(1111@1hostname,2222@2hostname):res1+1,res2+1,res3-1";
// char string[]="dest";
//    char *inputString = string;
    char *inputString = trainMessage;
    /* Delimeter set for State machine */
    char *delimeterColon = ":";
    char *delimeterAt = "@";
    char *delimeterAdd = "(";
    char *delimeterParenthesisClose = ")";
    char *delimeterPlus = "+";
    char *delimeterMinus = "-";
    char *delimeterComa = ",";
    char *delimeterStationNameColon = ":";
    char * token = NULL;
    char * tokenAllResourcePair = NULL;
    char * resourceName = NULL;
    char * resourceQuantity = NULL;
    char * tokenResourcePair = NULL;
    char * tokenAllConnectingStations = NULL;
    char * tokenConnectingStation = NULL;
    char * portNumber = NULL;
    char * intPortNumber;
    char * delim = NULL;
    int nextStationPortNum=0;
    int len=0;

    delim = delimeterColon;
    int state = -1;

    /* For sending messages to other station*/
    int sock_send, n_send;
    unsigned int length_send;
    struct sockaddr_in server_send, from_send;
    struct hostent *hp_send;
    char buffer_send[256];
    sock_send= socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_send < 0) error("socket");
    server_send.sin_family = AF_INET;
    /*-------------------------------------------*/

    fprintf(stderr, "\n Input String = ---%s---END\n", inputString);

    while ( token = strsep(&inputString, delimeterColon)  )
    {
//      fprintf(stderr, "\ntoken---%s---Remaining stringis:---%s----", token, inputString);
        if ( -1 == state)
        {
            state++;
            if (strcmp("dest", token) ==0)
            {
                fprintf(stderr, "\n Station Name matches %s Remaining string  is : %s END\n", token, inputString);
                continue;
            }
            else
            {
                discardedTrainCountWrongStation++;
                break;
            }
        }

        if ( '\0' == token)
        {
            fprintf(stderr, "\nNothing left to END\n");
            break;
        }
        if (NULL ==inputString )
            token[strlen(token) -1] = '\0';
        fprintf(stderr, "\nTOKEN ::::;%s;:::::END\n", token);

        /* case : doomtrainstop and stop train */
        if (strcmp("doomtrain",token) ==0)
        {
            printf("\n Doomtrain encountered: Send doomtrain message to all connected stationEND\n");
            /*TODO : Doomtrain encountered: Send doomtrain message to all connected station*/
            display_all_stations(stationList);
            send_doomtrain_to_all_connected_stations(stationList);
            shut_down(0);
        }
        else if (strcmp("stopstation",token) ==0)
        {
            fprintf(stderr, "\n stoptrain encounteredEND\n");
            /* This is tricky: will do this in last, lets make everything else before this*/
            /*TODO : 1. Process this train
                     2. Send remaining message to next station
                     3. Shut down -- Done*/
            shut_down(0);
        }
        else if (strncmp("add(",token,4) ==0)
        {
//       fprintf(stderr, "\n add( encountered");    /* All Connecting Station are here*/
            tokenAllConnectingStations = strsep(&token, delimeterAdd); // must return NULL
            tokenAllConnectingStations = strsep(&token, delimeterParenthesisClose);
//        fprintf(stderr, "\n tokenAllConnectingStations %s  token %s",tokenAllConnectingStations, token);

            for (; tokenConnectingStation = strsep(&tokenAllConnectingStations, delimeterComa);)
            {
                if( NULL != strrchr(tokenConnectingStation,'@'))
                {
                    portNumber = strsep(&tokenConnectingStation, delimeterAt);

                    if( atoi(portNumber) < 0 | atoi(portNumber) > 65535)
                    {
                        discardedTrainCountFormatErrors++;
                        return;
                    }
                    stationList = insert_station(stationList, atoi(portNumber), tokenConnectingStation);
                    //      fprintf(stderr, "\n insert_station portNumber: %s hostname : %s",portNumber, tokenConnectingStation);
                }
                else
                {
                    /* Bad Port Hostname pair*/
                    //  fprintf(stderr, "In Port Hostname pair @ not foundEND\n");
                    //  exit;
                    discardedTrainCountFormatErrors++;
                    return;
                }
            }         /* End : Connecting Station are here*/
        }
        else if ( ( NULL != strrchr(token,'+')) && ( NULL != strrchr(token,'-')))
        {
            //       fprintf(stderr, "\n In Resource List %s %s",token , inputString);
            tokenAllResourcePair=token;
            /* Resources are here*/
            for (; tokenResourcePair = strsep(&tokenAllResourcePair, delimeterComa );)
            {
                //fprintf(stderr, "\n In Resource tokenAllResourcePair  %s  tokenResourcePair %s",tokenAllResourcePair , tokenResourcePair);
                if ( NULL ==  tokenResourcePair)
                {
                    fprintf(stderr, "\n No more tokenResourcePairEND\n");
                    break;
                }
                else
                    if( NULL != strrchr(tokenResourcePair,'+'))
                    {
                        resourceName = strsep(&tokenResourcePair, delimeterPlus);
                        resourceList = insert_resources(resourceList, atoi(tokenResourcePair), resourceName);
//                        fprintf(stderr, "\n                insert_resources  resourceName : %s resourceQuantity %s",resourceName, tokenResourcePair);
                    }
                    else if( NULL != strrchr(tokenResourcePair,'-'))
                    {
                        resourceName = strsep(&tokenResourcePair, delimeterMinus);
                        resourceList = insert_resources(resourceList, atoi(tokenResourcePair), resourceName);
                        //                      fprintf(stderr, "\n                insert_resources  resourceName : %s resourceQuantity %s",resourceName, tokenResourcePair);
                    }
                    else
                    {
                        /* Bad resource & quantity*/
                        discardedTrainCountFormatErrors++;
                        return;
                        fprintf(stderr, "\nIn resource pair, + or - not found= ---%s---END\n",tokenAllResourcePair);
                    }
            }         /* End : Resources are here*/
        }
        else
        {
            /* here token is next station , search this station and its port*/
            /* token + input string */
            //      fprintf(stderr, "\n $$$New Station is Token --with data in inputString$$$");
            //     fprintf(stderr, "\n Please send me to next station: token  = ---%s---",token );
            /*
                        len = strlen(token);
                        while ( len > 0 && token[len - 1] == '\n' )
                        {
                            token[len - 1] = '\0';
                            --len;
                        }
            */
            //     fprintf(stderr, "\n Pls send me2 : inputString  = ---%s---",inputString );
            //          display_all_stations(stationList);

            nextStationPortNum = search_station(stationList,token);
            //         fprintf(stderr, "\n For station name = ---%s--- , portNum = ---%d---",token, nextStationPortNum );
            if ( 0 == nextStationPortNum )
            {
                fprintf(stderr, "\n ERROR : For station name = ---%s--- , portNum = ---%d---END\n",token, nextStationPortNum );
            }
            else
            {
                /* Send message code*/
                /*TODO :  Authentication to be handled i.e. Check page 4*/
                //      fprintf(stderr, "\n\nstart of Send message code\n\n");
                hp_send = gethostbyname("localhost");
                if (0 ==hp_send) error("Unknown host");

                bcopy((char *)hp_send->h_addr,  (char *)&server_send.sin_addr, hp_send->h_length);
                server_send.sin_port = htons(nextStationPortNum);
                length_send=sizeof(struct sockaddr_in);
                bzero(buffer_send,256);
                //         char *strcat(char *dest, const char *src);
                strcat (buffer_send,token);
                if ( 0< strlen(inputString))
                    strcat (buffer_send,inputString);
                n_send=sendto(sock_send,buffer_send,
                strlen(buffer_send),0,(const struct sockaddr *)&server_send,length_send);
                if (n_send < 0)
                {
                    processedTrainCountInvalidNextStation++;
                    error("Sendto");
                }
                close(sock_send);
                //        fprintf(stderr, "\n\nEnd of Send message code\n\n");
            }

        }
    } /* first for loop ends here */
    processedTrainCountWithoutErrors++;
    state = -1;
}

void check_arguments(int argc, char* argv[]);
int check_authfile(char* argv[]);
int check_valid_port_num(char *argv[]);

void check_arguments(int argc, char* argv[])
{
    if (argc < 3 || argc > 5)
    {
        fprintf(stderr, "Usage: station name authfile log-file [port] [host]\n");
        exit(1);
    }
}

/* TODO: Handle other two conditions:
 1. only the first line is empty
 2. string is empty
 */
int check_authfile(char* argv[])
{
    FILE *authFile = fopen(argv[2], "r");
    if (authFile == NULL)
    {
        fprintf(stderr, "Invalid name/auth\n");
        exit(3);
    }
    return TRUE;
}

int check_valid_port_num(char *argv[])
{
    char *lastLetter;
    int portNum;
    portNum = (int) strtol(argv[3], &lastLetter, 10);

    if (*lastLetter != '\0' || portNum < 2 || portNum > 4)
    {
        fprintf(stderr, "Invalid port\n");
        exit(3);
    }
    return portNum;
}

int check_argument_number(int argc)
{
    if(argc < 4 || argc > 6)
        shut_down(1) ;
}


int main(int argc, char* argv[])
{
    int ret;
    char buffer[256];
    signal(SIGHUP,sighup); /* set function calls */

   /*TODO: Wrap all cmdline argument checking in a function.*/
    check_argument_number(argc);
    char *name = argv[1];
    //name must be at least 1 char long
    if (strlen(name) < 1)
        shut_down(2) ;
    //second argument is the authfile
    FILE *fauth = fopen(argv[2],"r");
    //could not open authfile
    if (!fauth)
        shut_down(2) ;

    char authstr[128];
    //could not read authstring
    // fgets will read 128 chars or will stop if it getes into newline or EOF
    if (!fgets(authstr, 128, fauth))
        shut_down(2) ;

    //authstring must be more than 0 in length
    if (strlen(authstr) == 0)
        shut_down(2) ;

    //third arg is the logfile
    // a stands for appendnig
//    FILE *flog = fopen(argv[3],"a");
    flog = fopen(argv[3],"a");

    //could not open logfile
    if (!flog)
        shut_down(3) ;

    //default port is 0 (any)
    int port = 0;

    //parse optional 4th argument
    if (argc > 4)
    {
        char *end;
        port = strtol(argv[4], &end, 10);
        //the port argument is not a number
        if (*end != '\0')
            shut_down(4) ;

    }

    char *iface = NULL;
    //parse optional 5th argument
    if (argc > 5)
    {
        iface = argv[5];
    }

    /* TODO:  remove or comment these*/
    fprintf(stderr, "name:  %s\n", name);
    fprintf(stderr, "host: %s\n", iface);
    fprintf(stderr, "port:  %d\n", port);
    fprintf(stderr, "auth:  %s\n", argv[2]);
    fprintf(stderr, "log:   %s\n", argv[3]);

    int sock, length, n;
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024];

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided END\n");
        exit(0);
    }

    sock=socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("Opening socket");
    length = sizeof(server);
    bzero(&server,length);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port);
    if (bind(sock,(struct sockaddr *)&server,length)<0)
        error("binding");
    fromlen = sizeof(struct sockaddr_in);
    while (1)
    {
        /*TODO :  Authentication to be handled i.e. Check page 4*/
        n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
        if (n < 0) error("recvfrom");
        buf[n] = '\0';
        printf("\n Recieved string = --%d---%s---END\n", n,buf);
        train_processing(buf);
        bzero(buf,256);
    }
    close(sock);
    return 0;
}
