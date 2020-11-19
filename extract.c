#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include "../esb/esb.h"

/* Extracting contents from a XML document and store it in envelope_bmd.
 * We store if node type is element and it is a leaf */

void envelope_extract (xmlNode *node, envelope_bmd *ns){
    int n;

    while(node)
    {
        if(node->type == 1)
        {
                n= strlen((char*) xmlNodeGetContent(node));
                if((strcmp(((char *) node->name),"MessageID")==0))
                {
                    /* Dynamically allocating memory */
                    ns->MessageID = (char *)malloc(n*sizeof(char));

                    strcpy(ns->MessageID,(char *) xmlNodeGetContent(node));
                    printf(" MessageID is: %s\n",ns->MessageID );
                    if(strcmp(ns->MessageID, "") ==0){
                        free(ns->MessageID);
                        ns->MessageID=NULL;
                    }
                }

                else if((strcmp(((char *) node->name),"MessageType"))==0)
                {
                    /* Dynamically allocating memory */
                    ns->MessageType =  (char *)malloc(n*sizeof(char));
                    /* Copying content  */
                    strcpy(ns->MessageType ,(char *)xmlNodeGetContent(node));
                    printf(" MessageType is: %s\n",ns->MessageType );
                    /* Checking for Null values */
                    if(strcmp(ns->MessageType, "") ==0){
                        free(ns->MessageType);
                        ns->MessageType=NULL;
                    }
                }

                else if(strcmp(((char *) node->name), "Sender")==0)
                {
                    /* Dynamically allocating memory */
                    ns->Sender =  (char *)malloc(n*sizeof(char));
                    /* Copying content  */

                    strcpy(ns->Sender ,(char *)xmlNodeGetContent(node));
                    printf(" Sender is: %s\n",ns->Sender );
                    /* Checking for Null values */
                    if(strcmp(ns->Sender, "") ==0){
                        free(ns->Sender);
                        ns->Sender=NULL;
                    }


                }

                else if((strcmp(((char *) node->name),"Destination"))==0)
                {
                    /* Dynamically allocating memory */
                    ns->Destination =  (char *)malloc(n*sizeof(char));
                    /* Copying content  */

                    strcpy(ns->Destination ,(char *)xmlNodeGetContent(node));
                    printf(" Destination  is: %s\n",ns->Destination );
                    /* Checking for Null values */
                    if(strcmp(ns->Destination, "") ==0){
                        free(ns->Destination);
                        ns->Destination=NULL;
                    }

                }

                else if((strcmp(((char *) node->name),"CreationDateTime"))==0)
                {
                    /* Dynamically allocating memory */
                    ns->CreationDateTime =  (char *)malloc(n*sizeof(char));
                    /* Copying content  */

                    strcpy(ns->CreationDateTime ,(char *)xmlNodeGetContent(node));
                    printf(" CreationDateTime is:  %s\n",ns->CreationDateTime );
                    /* Checking for Null values */
                    if(strcmp(ns->CreationDateTime, "") ==0){
                        free(ns->CreationDateTime);
                        ns->CreationDateTime=NULL;
                    }

                }

                else  if((strcmp(((char *) node->name),"Signature"))==0)
                {
                    /* Dynamically allocating memory */
                    ns->Signature =  (char *)malloc(n*sizeof(char));
                    /* Copying content  */

                    strcpy(ns->Signature ,(char *)xmlNodeGetContent(node));
                    printf(" Signature is: %s\n",ns->Signature);
                    /* Checking for Null values */
                    if(strcmp(ns->Signature, "") ==0){
                        free(ns->Signature);
                        ns->Signature=NULL;
                    }
                }

                else if((strcmp(((char *) node->name),"ReferenceID"))==0)
                {
                    /* Dynamically allocating memory */
                    ns->ReferenceID =  (char *)malloc(n*sizeof(char));
                    /* Copying content  */

                    strcpy(ns->ReferenceID ,(char *)xmlNodeGetContent(node));
                    printf(" ReferenceID is: %s\n",ns->ReferenceID);
                    /* Checking for Null values */
                    if(strcmp(ns->ReferenceID, "") ==0){
                        free(ns->ReferenceID);
                        ns->ReferenceID=NULL;
                    }

                }
        }
        envelope_extract(node->children,ns);
        node = node->next;
    }

}

envelope_bmd * extract_envelope(char * filepath)
{
    xmlDoc *doc=NULL;
    xmlNode *root_element=NULL;

    /*parse the file and get the DOM */
    doc = xmlReadFile(filepath, NULL, 0);
    if (doc == NULL) {
        printf("Could not parse the XML file");
    }
    /*Get the root element node */
    envelope_bmd * ns =(envelope_bmd *) malloc(sizeof(envelope_bmd));
    root_element = xmlDocGetRootElement(doc);
    envelope_extract(root_element,ns);

    /*free the document */
    //xmlFreeDoc(doc);

    /*Free the global variables that may have been allocated by the parser.*/
    //xmlCleanupParser();
    return ns;
}


/* Extracting contents from a XML document and store it in payload_bmd.
 * We store if node type is element and it is a leaf */


char * extract_payload(char * filepath) {
    xmlDoc *doc=NULL;
    xmlNode *root_element=NULL;
    char * Payload;
    /*parse the file and get the DOM */
    doc = xmlReadFile(filepath, NULL, 0);
    if (doc == NULL) {
        printf("Could not parse the XML file");
    }
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    xmlNode * node = root_element -> children;

    while(node)
    {
        int n;

        if(node->type == 1)
        {
                n= strlen((char*) xmlNodeGetContent(node));

                if ((strcmp(((char *) node->name),"Payload"))==0)
                {
                    /* Dynamically allocating memory */
                    Payload = (char *)malloc(n* sizeof(char));
                    /* Copying content  */
                    strcpy(Payload , (char *) xmlNodeGetContent(node));
                    printf("Payload is:%s\n",Payload);
                    /* Checking for Null values */
                    if(n==0){
                        //free(Payload);
                        Payload=NULL;
                    }
                    return Payload;
                }
        }
        node= node->next;
    }

    /*free the document */
    xmlFreeDoc(doc);

    /* Free the global variables that may have been allocated by the parser.*/
    xmlCleanupParser();
    return NULL;
}

bmd*parse_bmd_xml(char * filepath)
{
    bmd * b = (bmd*) malloc (sizeof(bmd));
    b->envelope=  extract_envelope(filepath);
    b->payload= extract_payload(filepath);
    return b;
}

