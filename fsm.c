#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAX 30

/* Data Structure */
struct resources
{
    char *resourceName;
    unsigned int resourceQuantity;
    struct resources *next;
} *resources;

struct station
{
    int  portNumber;
    char stationName[MAX];
    struct station *next;
};



struct station *insert_station(struct station *front, int portNum, char stationName[])
{
    struct station *newnode;
   printf("\n insert_station called with portnum %d station %s",portNum, stationName);

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

void printStation(struct station *p)
{
    printf("\n Station Details : \n");
    printf("\n Port Namuber  : %d", p->portNumber);
    printf("\n Station Name  : %s", p->stationName);
    printf("\n-------------------------------------\n");
}

void display_all_stations(struct station  *front)
{
    struct station *ptr;

    for (ptr = front; ptr != NULL; ptr = ptr->next)
    {
        printStation(ptr);
    }
}

char *myStationName;
int state; /* 0 doomtrain  , 1 stoptrain , 2 add , 3 resourses */
//resources *resourcesList;
//station *stationList;

/* Function Prototypes*/
int send_doomtrain(void);
int stop_stop_station(void);

int main()
{

    char string[]="dest:doomtrain:stopstation:add(1111@1hostname,2222@2hostname):res1+1,res2+1,res3-1";
    char *inputString = string;

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

    struct station *stationList;
    struct station *resourceList;



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

        if ( NULL == token)
        {
            printf("\n\nNothing left to.");
            break;

        }

        /* case : doomtrainstop and stop train */
        //token = strsep(&modifiedString, delimeterColon);

        if (strcmp("doomtrain",token) ==0)
        {
            /* */
            printf("\n Doomtrain encountered\n");
        }
        else if (strcmp("stopstation",token) ==0)
        {
            /* */
            printf("\n stoptrain encountered\n");
        }
        else if (strncmp("add(",token,4) ==0)
        {
            /* */
//       printf("\n add( encountered");

            /* All Connecting Station are here*/

            tokenAllConnectingStations = strsep(&token, delimeterAdd); // must return NULL
            tokenAllConnectingStations = strsep(&token, delimeterParenthesisClose);
//        printf("\n tokenAllConnectingStations %s  token %s",tokenAllConnectingStations, token);

            for (; tokenConnectingStation = strsep(&tokenAllConnectingStations, delimeterComa);)
            {
                if( NULL != strrchr(tokenConnectingStation,'@'))
                {
                    portNumber = strsep(&tokenConnectingStation, delimeterAt);
                    //addStation(portNumber, tokenConnectingStation);
                   stationList = insert_station(stationList, atoi(portNumber), tokenConnectingStation);
             //      printf("\n portNumber: %s hostname : %s",portNumber, tokenConnectingStation);
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
                if ( NULL ==  tokenResourcePair)
                {
                    printf("\n No more tokenResourcePair");
                    break;
                }
                else

                    if( NULL != strrchr(tokenResourcePair,'+'))
                    {
                        resourceName = strsep(&tokenResourcePair, delimeterPlus);
                        //add_resource(resourceName, tokenResourcePair);
                         //resourceList = insert_station(resourceList, atoi(tokenResourcePair), resourceName);

                          //  resourceList = insert_station(resourceList, 1, "Name +");

                        printf("\n                add_resource  resourceName : %s resourceQuantity %s",resourceName, tokenResourcePair);
                    }
                    else if( NULL != strrchr(tokenResourcePair,'-'))
                    {
                        resourceName = strsep(&tokenResourcePair, delimeterMinus);
                        //minus_resource(resourceName, tokenResourcePair);
//                        resourceList = insert_station(resourceList, atoi(tokenResourcePair), resourceName);
//                            resourceList = insert_station(resourceList, 1, "name -");

                        printf("\n                minus_resource  resourceName : %s resourceQuantity %s",resourceName, tokenResourcePair);
                    }
                    else
                    {
                        /* Bad resource & quantity*/
                        printf("In resource pair, + or - not found");
                        exit;
                    }
            }         /* End : Resources are here*/
        }


    } /* first for loop ends here */


    display_all_stations(stationList);
   // display_all_stations(resourceList);

}
