#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlschemastypes.h>
#include <libxml2/libxml/tree.h>
typedef struct  // structure that contain the customer datas
{
    char name[30];
    char surname[30];
    char gender[2];
    char occupacy[30];
    char level_of_education[4];
    char email[30];
    char bank_account_number[13];
    char IBAN[28];
    char account_type[20];
    char currency_unit[8];
    char total_balance_available[8];
    char available_for_loan[8];
} Customer;

void validation(char *xmlFile, char *xsdFile);//validates the xml files while using xsd file
void csvBIN(char *fileName, char *outFile);//converts the csv file to binary file
void binXML(char *inputFile, char *outputFile);//converts the binary file to xml file
char *enidanConvert(int endianType, char* element);//converts the little endian to big endian
char **split(char *string, char *split_char);//splits the chars and crates an char array
char *copyString(char *s);//copies chars to another memory location fot prevent the corruption

int main(int argc,char *argv[]){
    if(argc<=2&&argc>=4){
        printf("You have to enter 3.\n");
        return 1;
    }
    else if(atoi(argv[3])==1){
        char **fileExtensions1 = split(copyString(argv[1]),".");
        char **fileExtensions2 = split(copyString(argv[2]),".");
        if(strcmp(fileExtensions1[1],"csv")!=0 || strcmp(fileExtensions2[1],"bin")!=0){
            printf("Your file extensions have to be .csv(you: %s) and .bin(you: %s) .\n",fileExtensions1[1],fileExtensions2[1]);
            return 1;
        }
        csvBIN(argv[1],argv[2]);
    }
    else if(atoi(argv[3])==2){
        char **fileExtensions1 = split(copyString(argv[1]),".");
        char **fileExtensions2 = split(copyString(argv[2]),".");
        if(strcmp(fileExtensions1[1],"bin")!=0 || strcmp(fileExtensions2[1],"xml")!=0){
            printf("Your file extensions have to be .bin(you: %s) and .xml(you: %s) .\n",fileExtensions1[1],fileExtensions2[1]);
            return 1;
        }
        binXML(argv[1],argv[2]);
    }
    else if(atoi(argv[3])==3){
        char **fileExtensions1 = split(copyString(argv[1]),".");
        char **fileExtensions2 = split(copyString(argv[2]),".");
        if(strcmp(fileExtensions1[1],"xml")!=0 || strcmp(fileExtensions2[1],"xsd")!=0){
            printf("Your file extensions have to be .xml(you: %s) and .xsd(you: %s) .\n",fileExtensions1[1],fileExtensions2[1]);
            return 1;
        }
        validation(argv[1],argv[2]);
    }
    else{
        printf("Wrong command you have to enter 1,2 or 3.\n");
    }
    return 0;
}

void binXML(char *inputFile, char *outputFile){
    FILE *binFile = fopen(inputFile,"rb");
    xmlDocPtr doc; 
    xmlNodePtr cur,root,newNode,childs,total;
    doc = xmlNewDoc("1.0");
    root = xmlNewNode(NULL,"records");
    xmlDocSetRootElement(doc,root);
    if(doc==NULL){
        printf("Document not parsed");
        return;
    }
    cur = xmlDocGetRootElement(doc);
    if(cur==NULL){
        printf("Empty document");
        return;
    }
    Customer customer;
    char *uri;
    int index = 1;
    while (fread(&customer,sizeof(customer),1,binFile)!=0)
    {
        int lenght = snprintf(NULL,0,"%d",index);
        char *uri = malloc(lenght+1);
        snprintf(uri,lenght+1,"%d",index);
        index++;
        newNode = xmlNewTextChild(cur,NULL,"row",NULL);   
        xmlNewProp(newNode, "id",uri);
        childs = xmlNewTextChild(newNode,NULL,"customer_info",NULL);
        xmlNewTextChild(childs,NULL,"name",customer.name);
        xmlNewTextChild(childs,NULL,"surname",customer.surname);
        xmlNewTextChild(childs,NULL,"gender",customer.gender);
        xmlNewTextChild(childs,NULL,"occupacy",customer.occupacy);
        xmlNewTextChild(childs,NULL,"level_of_education",customer.level_of_education);
        xmlNewTextChild(childs,NULL,"email",customer.email);
        childs = xmlNewTextChild(newNode,NULL,"bank_account_info",NULL);
        xmlNewTextChild(childs,NULL,"bank_account_number",customer.bank_account_number);
        xmlNewTextChild(childs,NULL,"IBAN",customer.IBAN);
        xmlNewTextChild(childs,NULL,"account_type",customer.account_type);
        total = xmlNewTextChild(childs,NULL,"total_balance_available",customer.total_balance_available);
        xmlNewProp(total, "currency_unit", customer.currency_unit);
        xmlNewProp(total, "bigEndVersion", enidanConvert(0,customer.total_balance_available));
        xmlNewTextChild(childs,NULL,"available_for_loan",customer.available_for_loan);
    }  
    xmlSaveFormatFileEnc(outputFile,doc,"utf-8",1);
    xmlFreeDoc(doc);
    printf("XML Parsed\n");
}

void csvBIN(char *fileName, char *outFile){
    FILE *csv = fopen(fileName,"r");
    FILE *bin = fopen(outFile,"wb");
    Customer customer;
    if(csv==NULL||bin==NULL){
        printf("File not opened.\n");
        return;
    }
    char lines[200];
    fgets(lines,sizeof(lines),csv);
    while (fgets(lines,sizeof(lines),csv))
    {
        char *splitChar=",";
        char **array = split(lines,splitChar);
        strncpy(customer.name,array[0],sizeof(customer.name));
        strncpy(customer.surname,array[1],sizeof(customer.surname));
        strncpy(customer.gender,array[2],sizeof(customer.gender));
        strncpy(customer.occupacy,array[3],sizeof(customer.occupacy));
        strncpy(customer.level_of_education,array[4],sizeof(customer.level_of_education));
        strncpy(customer.email,array[5],sizeof(customer.email));
        strncpy(customer.bank_account_number,array[6],sizeof(customer.bank_account_number));
        strncpy(customer.IBAN,array[7],sizeof(customer.IBAN));
        strncpy(customer.account_type,array[8],sizeof(customer.account_type));
        strncpy(customer.currency_unit,array[9],sizeof(customer.currency_unit));
        strncpy(customer.total_balance_available,array[10],sizeof(customer.total_balance_available));
        strncpy(customer.available_for_loan,array[11],sizeof(customer.available_for_loan));
        fwrite(&customer,sizeof(customer),1,bin);
    }
    fclose(csv);
    fclose(bin);
}

void validation(char *xmlFile, char *xsdFile){
    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
	
    char *XMLFileName = xmlFile; // write your xml file here
    char *XSDFileName = xsdFile; // write your xsd file here
    
    
    xmlLineNumbersDefault(1); //set line numbers, 0> no substitution, 1>substitution
    ctxt = xmlSchemaNewParserCtxt(XSDFileName); //create an xml schemas parse context
    schema = xmlSchemaParse(ctxt); //parse a schema definition resource and build an internal XML schema
    xmlSchemaFreeParserCtxt(ctxt); //free the resources associated to the schema parser context
    
    doc = xmlReadFile(XMLFileName, NULL, 0); //parse an XML file
    if (doc == NULL)
    {
        fprintf(stderr, "Could not parse %s\n", XMLFileName);
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;  //structure xmlSchemaValidCtxt, not public by API
        int ret;
        
        ctxt = xmlSchemaNewValidCtxt(schema); //create an xml schemas validation context 
        ret = xmlSchemaValidateDoc(ctxt, doc); //validate a document tree in memory
        if (ret == 0) //validated
        {
            printf("%s validates\n", XMLFileName);
        }
        else if (ret > 0) //positive error code number
        {
            printf("%s fails to validate\n", XMLFileName);
        }
        else //internal or API error
        {
            printf("%s validation generated an internal error\n", XMLFileName);
        }
        xmlSchemaFreeValidCtxt(ctxt); //free the resources associated to the schema validation context
        xmlFreeDoc(doc);
    }
    // free the resource
    if(schema != NULL)
        xmlSchemaFree(schema); //deallocate a schema structure

    xmlSchemaCleanupTypes(); //cleanup the default xml schemas types library
    xmlCleanupParser(); //cleans memory allocated by the library itself 
    xmlMemoryDump(); //memory dump
}

char *enidanConvert(int endianType, char* element){
    uint32_t num = atoi(element);
    uint32_t a0,a1,a2,a3;
    uint32_t result;
    a0 = (num & 0x000000ff) << 24u;
    a1 = (num & 0x0000ff00) << 8u;
    a2 = (num & 0x00ff0000) >> 8u;
    a3 = (num & 0xff000000) >> 24u;
    result = a0|a1|a2|a3;
    int lenght = snprintf(NULL,0,"%d",result);
    char *res = malloc(lenght+1);
    snprintf(res,lenght+1,"%d",result);
    return res;
}

char *copyString(char *s){
    char *copied = malloc(sizeof(char)*strlen(s));
    for (int i = 0; i < sizeof(char)*strlen(s); i++)
    {
        copied[i] = s[i];
    }
    return copied;
}

char **split(char *string, char *split_char){
    char* substring = strtok(string,split_char);
    char** array = malloc(sizeof(char*)*12);
    int index = 0;
    while (substring!=NULL)
    {
        array[index] = substring;
        index++;
        substring = strtok(NULL,split_char);
    }
    array[index] = NULL;
    return array; 
}

