#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml2/libxml/parser.h> // includelar bizde sadece bu şekilde çalışıyor. Isterseniz değiştirebilirsiniz.
#include <libxml2/libxml/xmlmemory.h>
#include <libxml2/libxml/xmlschemastypes.h>
#include <libxml2/libxml/tree.h>
typedef struct  // structure that contain the customer datas
{
    char name[20];
    char surname[30];
    char gender[2];
    char occupacy[30];
    char level_of_education[4];
    char email[40];
    char bank_account_number[15];
    char IBAN[30];
    char account_type[20];
    char currency_unit[15];
    char total_balance_available[15];
    char available_for_loan[15];
} Customer;

void validation(char *xmlFile, char *xsdFile);//validates the xml files while using xsd file
void csvBIN(char *fileName, char *outFile);//converts the csv file to binary file
void binXML(char *inputFile, char *outputFile);//converts the binary file to xml file
char *endianConvert(int endianType, char* element);//converts the little endian to big endian
char **split(char *string, char *split_char);//splits the chars and creates an char array
char *copyString(char *s);//copies chars to another memory location fot prevent the corruption

int main(int argc,char *argv[]){
    
    if(argc<=2 && argc>=4){ //situation of not entering correct numbers of arguments. there must be 3 arguments two file names and an opearation.
        printf("You have to enter correct file names-extensions and operation.\n");
        return 1;
    }
    else if(atoi(argv[3])==1){// situation of first operation.
        char **fileExtensions1 = split(copyString(argv[1]),".");// determining the first file extention with splitting from the dot that entered file name.
        char **fileExtensions2 = split(copyString(argv[2]),".");// determining the second file extention.
        if(strcmp(fileExtensions1[1],"csv")!=0 || strcmp(fileExtensions2[1],"bin")!=0){ // comparing the file extentions.
            printf("Your file extensions have to be .csv(you: %s) and .bin(you: %s) .\n",fileExtensions1[1],fileExtensions2[1]); // error message
            return 1;
        }
        csvBIN(argv[1],argv[2]); // csv to bin operation.
    }
    else if(atoi(argv[3])==2){ // situation of second operation.
        char **fileExtensions1 = split(copyString(argv[1]),".");// determining the first file extention with splitting from the dot that entered file name.
        char **fileExtensions2 = split(copyString(argv[2]),".");// determining the second file extention with splitting from the dot that entered file name.
        if(strcmp(fileExtensions1[1],"bin")!=0 || strcmp(fileExtensions2[1],"xml")!=0){// comparing the file extentions.
            printf("Your file extensions have to be .bin(you: %s) and .xml(you: %s) .\n",fileExtensions1[1],fileExtensions2[1]);// error message
            return 1;
        }
        binXML(argv[1],argv[2]);// bin to xml operation
    }
    else if(atoi(argv[3])==3){ // situation of third operation.
        char **fileExtensions1 = split(copyString(argv[1]),".");// determining the first file extention with splitting from the dot that entered file name.
        char **fileExtensions2 = split(copyString(argv[2]),".");// determining the second file extention with splitting from the dot that entered file name.
        if(strcmp(fileExtensions1[1],"xml")!=0 || strcmp(fileExtensions2[1],"xsd")!=0){
            printf("Your file extensions have to be .xml(you: %s) and .xsd(you: %s) .\n",fileExtensions1[1],fileExtensions2[1]);// error message
            return 1;
        }
        validation(argv[1],argv[2]); // xml validation with xsd.
    }
    else{
        printf("Wrong command you have to enter 1,2 or 3.\n");
    }
    return 0;
}

void binXML(char *inputFile, char *outputFile){ //converts the binary file to xml file
    FILE *binFile = fopen(inputFile,"rb"); //open the given binary file
    xmlDocPtr doc; // document
    xmlNodePtr cur,root,newNode,childs,total;
    doc = xmlNewDoc("1.0"); // creating new document.
    root = xmlNewNode(NULL,"records");//creating root node of the xml file.
    xmlDocSetRootElement(doc,root); // creating root of the xml file.
    if(doc==NULL){
        printf("Document not parsed");
        return;
    }
    cur = xmlDocGetRootElement(doc); // assinging to cur root of doc.
    if(cur==NULL){
        printf("Empty document");
        return;
    }
    Customer customer;
    char *id; // holding row id.
    int index = 1;
    while (fread(&customer,sizeof(customer),1,binFile)!=0) // reading binary file customer.
    {
        int lenght = snprintf(NULL,0,"%d",index); // length of id
        char *id = malloc(lenght+1); // allocate bytes from memory for id. 
        snprintf(id,lenght+1,"%d",index); // store string specified length and format.
        index++;
        newNode = xmlNewTextChild(cur,NULL,"row",NULL); // creating new row.  
        xmlNewProp(newNode, "id",id); // assinging attribute of row.
        childs = xmlNewTextChild(newNode,NULL,"customer_info",NULL);// creating customer info row
        xmlNewTextChild(childs,NULL,"name",customer.name);// creating customer name
        xmlNewTextChild(childs,NULL,"surname",customer.surname);// creating customer surname
        xmlNewTextChild(childs,NULL,"gender",customer.gender);
        xmlNewTextChild(childs,NULL,"occupacy",customer.occupacy);
        xmlNewTextChild(childs,NULL,"level_of_education",customer.level_of_education);
        xmlNewTextChild(childs,NULL,"email",customer.email);
        childs = xmlNewTextChild(newNode,NULL,"bank_account_info",NULL); // creating bank account info
        xmlNewTextChild(childs,NULL,"bank_account_number",customer.bank_account_number);
        xmlNewTextChild(childs,NULL,"IBAN",customer.IBAN);
        xmlNewTextChild(childs,NULL,"account_type",customer.account_type);
        total = xmlNewTextChild(childs,NULL,"total_balance_available",customer.total_balance_available); 
        xmlNewProp(total, "currency_unit", customer.currency_unit); // attribute of total balance available
        xmlNewProp(total, "bigEndVersion", endianConvert(0,customer.total_balance_available)); // 
        xmlNewTextChild(childs,NULL,"available_for_loan",customer.available_for_loan);
    }  
    xmlSaveFormatFileEnc(outputFile,doc,"utf-8",1); // saving document
    xmlFreeDoc(doc); 
    printf("XML Parsed\n");
}

void csvBIN(char *fileName, char *outFile){ //convert the csv file to bin file.
    FILE *csv = fopen(fileName,"r"); //opening the csv file
    FILE *bin = fopen(outFile,"wb"); // producing bin file to writing .
    Customer customer; 
    if(csv==NULL||bin==NULL){ 
        printf("File not opened.\n");
        return;
    }
    char lines[200];
    fgets(lines,sizeof(lines),csv); // first line reading operation from csv file because first line is column names.
    while (fgets(lines,sizeof(lines),csv)) //line reading operation from csv file.
    {
        char *splitChar=","; //determining the split character.
        char **array = split(lines,splitChar); // split operation to obtain tokens from string using split char ','
        strncpy(customer.name,array[0],sizeof(customer.name)); // copy the name obtaining from string elements to customer name.
        strncpy(customer.surname,array[1],sizeof(customer.surname));// copy the surname obtaining from string elements to customer surname.
        strncpy(customer.gender,array[2],sizeof(customer.gender)); // copy the gender obtaining from string elements to customer gender.
        strncpy(customer.occupacy,array[3],sizeof(customer.occupacy)); // copy the occupacy obtaining from string elements to customer occupacy.
        strncpy(customer.level_of_education,array[4],sizeof(customer.level_of_education));// copy thelevel of education obtaining from string elements to customer level of education.
        strncpy(customer.email,array[5],sizeof(customer.email));// copy the email obtaining from string elements to customer email.
        strncpy(customer.bank_account_number,array[6],sizeof(customer.bank_account_number));// assign the bank account number obtaining from string elements to customer bank accountnumber.
        strncpy(customer.IBAN,array[7],sizeof(customer.IBAN));// copy the IBAN obtaining from string elements to customer IBAN.
        strncpy(customer.account_type,array[8],sizeof(customer.account_type));// copy the accounttype obtaining from string elements to customer accounttype.
        strncpy(customer.currency_unit,array[9],sizeof(customer.currency_unit));//copy the currency unit obtaining from string elements to customer currencyunit.
        strncpy(customer.total_balance_available,array[10],sizeof(customer.total_balance_available));// copy the total balance avaible obtaining from string elements to customer total balance avaible.
        strncpy(customer.available_for_loan,array[11],sizeof(customer.available_for_loan));// copy the avaible for loan obtaining from string elements to customer avaible for loan.
        fwrite(&customer,sizeof(customer),1,bin);// writing operation of customer infos to bin file.
    }
    fclose(csv);
    fclose(bin);
    printf("%s created.\n" , outFile);
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

char *endianConvert(int endianType, char* element){

    uint32_t num = atoi(element); //convert the char element to uint32_t
    uint32_t a0,a1,a2,a3;
    uint32_t result;
    a0 = (num & 0x000000ff) << 24u; // shift the num's last three byte to left
    a1 = (num & 0x0000ff00) << 8u; // shift the num's third byte to one left byte
    a2 = (num & 0x00ff0000) >> 8u; // shift the num's second byte to one right byte
    a3 = (num & 0xff000000) >> 24u; // shift the num's first byte to three right byte
    result = a0|a1|a2|a3; // mergeing the bytes.
    int lenght = snprintf(NULL,0,"%d",result); // length of result
    char *res = malloc(lenght+1); // allocate bytes from mem.
    snprintf(res,lenght+1,"%d",result); 
    return res;
}

char *copyString(char *s){ //copies chars to another memory location for prevent the corruption
    char *copied = malloc(sizeof(char)*strlen(s)); // allocates the bytes to holding lenght of string of char.
    for (int i = 0; i < sizeof(char)*strlen(s); i++) // 
    {
        copied[i] = s[i]; // copying string
    }
    return copied;
}

char **split(char *string, char *split_char){
    
    char* p= strstr(string, ",,");//pointer p holds the place of ",," to transform it to ", ,".
    
    while (p != NULL)
    {
        memmove(p +3, p+2, strlen(p) -1);//in order to place a space in two commas, this line provide to movement of elements comes after ",,"
        memcpy(p,", ,",3); //this line copies to address of p ", ,"
        p = strstr(string, ",,");// looking for new ",," in that line.
    }
    
    char* substring = strtok(string,split_char);//dividing string into tokens by split_char(comma)
    char** array = malloc(sizeof(char*)*12);// allocaates the bytes size of a memory block to holding 12 char.
    int index = 0;
    while (substring!=NULL)
    {
        array[index] = substring;//assing the substring to array index
        index++; 
        substring = strtok(NULL, split_char);//producing the next substring.
    }
    array[index]= NULL;
    return array; 
}
