#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>

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

void shut_down(int errorNumber)
{
    switch( errorNumber)
    {
        case 0:
            exit(0);
        case 1:
        printf("\nsdsdUsage: station name authfile logfile [port] [host]");
        break;
        case 2:
        printf("Invalid name/auth");
        break;
        case 3:
        printf("Unable to open log");
        break;
        case 4:
        printf("Invalid port");
        break;
        case 5:
        printf("Listen error");
        break;
        case 6:
        printf("Unable to connect to station ");
        break;
        case 7:
        printf("Duplicate station names");
        break;
        case 99:
        printf("Unspecified system call failure");
        break;
    }

    exit(errorNumber);
}

struct resources *insert_resources(struct resources *front, int resourceQuantity, char resourceName[])
{

    struct resources *newnode;
//   printf("\n insert_resources called with resourceQuantity %d resourceName %s",resourceQuantity, resourceName);

    newnode = (struct resources*)malloc(sizeof(struct resources));

    if (newnode == NULL)
    {
        printf("\n Allocation failed \n");
        exit(2);
    }
    newnode->resourceQuantity = resourceQuantity;
    strcpy(newnode->resourceName, resourceName);
    newnode->next = front;
    front = newnode;
    return(front);
}

struct station *insert_station(struct station *front, int portNum, char stationName[])
{

    struct station *newnode;
    newnode = (struct station*)malloc(sizeof(struct station));
    if (newnode == NULL)
    {
        printf("\n Allocation failed \n");
        exit(2);
    }
    newnode->portNumber = portNum;
    strcpy(newnode->stationName, stationName);
    newnode->next = front;
    front = newnode;
    return(front);


}

void printResource(struct resources *r)
{
    printf("\n Resource quantity  : %d", r->resourceQuantity);
    printf("  Resource Name  : %s", r->resourceName);
}

void printStation(struct station *p)
{
    printf("\n Port Number  : %d", p->portNumber);
    printf("  Station Name  : %s", p->stationName);
}

void display_all_stations(struct station  *front)
{
    struct station *ptr;
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
    char * delim = NULL;

    delim = delimeterColon;
    int state = -1;

    struct station *stationList = NULL;
    struct resources *resourceList = NULL;

    printf("\n Input String = %s\n", inputString);

    while ( token = strsep(&inputString, delimeterColon)  )
    {
        if( state == -1 )
        {

            if (strcmp("dest", token) ==0)
            {
                state++;
                //    printf("\n Station Name matches %s\n Remaining string  is : %s \n", token, inputString);
                continue;
            }
            else   break;
        }

        if ( '\0' == token)
        {
            printf("\n\nNothing left to.");
            break;

        }

//        printf("\n TOKEN ::::%s;:::::\n", token);


        /* case : doomtrainstop and stop train */
        if (strcmp("doomtrain",token) ==0)
        {
            printf("\n Doomtrain encountered");
        }
        else if (strcmp("stopstation",token) ==0)
        {
            printf("\n stoptrain encountered");
        }
        else if (strncmp("add(",token,4) ==0)
        {
//       printf("\n add( encountered");    /* All Connecting Station are here*/

            tokenAllConnectingStations = strsep(&token, delimeterAdd); // must return NULL
            tokenAllConnectingStations = strsep(&token, delimeterParenthesisClose);
//        printf("\n tokenAllConnectingStations %s  token %s",tokenAllConnectingStations, token);

            for (; tokenConnectingStation = strsep(&tokenAllConnectingStations, delimeterComa);)
            {
                if( NULL != strrchr(tokenConnectingStation,'@'))
                {
                    portNumber = strsep(&tokenConnectingStation, delimeterAt);
                    stationList = insert_station(stationList, atoi(portNumber), tokenConnectingStation);
             //      printf("\n insert_station portNumber: %s hostname : %s",portNumber, tokenConnectingStation);
                }
                else
                {
                    /* Bad Port Hostname pair*/
                    printf("In Port Hostname pair @ not found");
                    exit;
                }
            }         /* End : Connecting Station are here*/

        }
        else
        {
            //       printf("\n In Resource List %s %s",token , inputString);
            tokenAllResourcePair=token;
            /* Resources are here*/
            for (; tokenResourcePair = strsep(&tokenAllResourcePair, delimeterComa );)
            {

  //                printf("\n In Resource tokenAllResourcePair  %s  tokenResourcePair %s",tokenAllResourcePair , tokenResourcePair);

                if ( NULL ==  tokenResourcePair)
                {
                    printf("\n No more tokenResourcePair");
                    break;
                }
                else

                    if( NULL != strrchr(tokenResourcePair,'+'))
                    {
                        resourceName = strsep(&tokenResourcePair, delimeterPlus);
                        resourceList = insert_resources(resourceList, atoi(tokenResourcePair), resourceName);
//                        printf("\n                insert_resources  resourceName : %s resourceQuantity %s",resourceName, tokenResourcePair);
                    }
                    else if( NULL != strrchr(tokenResourcePair,'-'))
                    {
                        resourceName = strsep(&tokenResourcePair, delimeterMinus);
                        resourceList = insert_resources(resourceList, atoi(tokenResourcePair), resourceName);
  //                      printf("\n                insert_resources  resourceName : %s resourceQuantity %s",resourceName, tokenResourcePair);
                    }
                    else
                    {
                        /* Bad resource & quantity*/
                        printf("In resource pair, + or - not found");
                     //   exit;
                    }
            }         /* End : Resources are here*/
        }


    } /* first for loop ends here */


  display_all_stations(stationList);
  display_all_resources(resourceList);
printf("\n");

}


#define TRUE 1
#define FALSE 0

void check_arguments(int argc, char* argv[]);
int check_authfile(char* argv[]);
int check_valid_port_num(char *argv[]);


void check_arguments(int argc, char* argv[]) {
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage: station name authfile log-file [port] [host]\n");
        exit(1);
    }
}

/* TODO: Handle other two conditions:
 1. only the first line is empty
 2. string is empty
 */
int check_authfile(char* argv[]) {
    FILE *authFile = fopen(argv[2], "r");
    if (authFile == NULL) {
        fprintf(stderr, "Invalid name/auth\n");
        exit(3);
    }
    return TRUE;
}


int check_valid_port_num(char *argv[]) {
    char *lastLetter;
    int portNum;
    portNum = (int) strtol(argv[3], &lastLetter, 10);

    if (*lastLetter != '\0' || portNum < 2 || portNum > 4) {
        fprintf(stderr, "Invalid port\n");
        exit(3);
    }
    return portNum;
}


int check_argument_number(int argc){
    if(argc < 4) {
        fprintf(stderr, "Usage: station name authfile lof-file [port] [host]\n");
        return 1;
    }

    if (argc > 6) {
        fprintf(stderr, "Usage: station name authfile lof-file [port] [host]\n");
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    /*
     //must have at least 4 arguments.
     if (argc < 4) {
	    fprintf(stderr,"Usage: %s name authfile log-file [port] [host]\n", argv[0]);
	    return 1;
     }

     //cannot have more than 6 arguments.
     if (argc > 6) {
	    fprintf(stderr,"Usage: %s name authfile log-file [port] [host]\n", argv[0]);
	    return 1;
     }*/
    int ret;
    char buffer[256];

    ret = check_argument_number(argc);
    if (ret) {
        return ret;
    }

    //first arg is name
    char *name = argv[1];

    //name must be at least 1 char long
    if (strlen(name) < 1) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //second argument is the authfile
    FILE *fauth = fopen(argv[2],"r");

    //could not open authfile
    if (!fauth) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    char authstr[128];

    //could not read authstring
    // fgets will read 128 chars or will stop if it getes into newline or EOF
    if (!fgets(authstr, 128, fauth)) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //authstring must be more than 0 in length
    if (strlen(authstr) == 0) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //third arg is the logfile
    // a stands for appendnig
    FILE *flog = fopen(argv[3],"a");

    //could not open logfile
    if (!flog) {
        fprintf(stderr,"Invalid name/auth\n");
        return 2;
    }

    //default port is 0 (any)
    int port = 0;

    //parse optional 4th argument
    if (argc > 4) {
        char *end;
        port = strtol(argv[4], &end, 10);

        //the port argument is not a number
        if (*end != '\0') {
            fprintf(stderr,"Invalid port\n");
            return 3;
        }
    }

    char *iface = NULL;

    //parse optional 5th argument
    if (argc > 5) {
        iface = argv[5];
    }

    printf("name:  %s\n", name);
    printf("iface: %s\n", iface);
    printf("port:  %d\n", port);
    printf("auth:  %s\n", argv[2]);
    printf("log:   %s\n", argv[3]);

    int listenfd;
    listenfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in servaddr;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    servaddr.sin_port=htons(port);

    bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

    if (listen(listenfd,1024) == -1) {
        fprintf(stderr, "Unspecified system call failure\n");
        exit(99);
    }


int connfd;
int n=0;
socklen_t clilen;
struct sockaddr_in cliaddr;

connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);

for (;; )
{  n=0;
   bzero(buffer,256);
   n = read( connfd,buffer,255 );

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
//TODO: start thread to handle connection here...
        printf("go a connection\n");
train_processing(buffer);
}
       close(connfd);
   close(listenfd);

    return 0;


}
