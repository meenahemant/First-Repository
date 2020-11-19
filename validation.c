
/* Validating bmd xml. Checking whether xml file consists
 * of all appropriate elements. If it contains returns 1
 * else return 0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../esb/esb.h"

int is_bmd_valid(bmd*b)
{
    if(b-> envelope->MessageID ==NULL) {
        //printf("MessageID not exist in BMD\n");
        return 0;

    }

    if(b-> envelope->MessageType == NULL) {
        //printf("Message Type not exist in BMD\n");
        return 0;
    }

    if(b-> envelope->Sender == NULL) {
        //printf("Sender not exist in BMD\n");
        return 0;
    }


    if(b-> envelope->Destination == NULL) {
        //printf("Destination not exist in BMD\n");
        return 0;
    }


    if(b-> envelope->CreationDateTime == NULL) {
        //printf("CreationDateTime not exist in BMD\n");
        return 0;
    }


    if(b-> envelope->Signature == NULL) {
        //printf("Signature not exist in BMD\n");
        return 0;
    }

    if(b-> envelope->ReferenceID == NULL) {
        //printf("ReferenceID not exist in BMD\n");
        return 0;
    }

    if(b->payload == NULL) {
        //printf("Payload not exist in BMD\n");
        return 0;
    }
    return 1;

}

