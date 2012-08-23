#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/getfile.h"

int main(int argc, char *argv[]){
  embInitPV("gcai",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__caiInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjBool    translate = 0;
  AjPStr    wabsent   = NULL;
  AjPStr    command   = NULL;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seqall    = ajAcdGetSeqall("sequence");
  translate = ajAcdGetBoolean("translate");
  wabsent   = ajAcdGetString("wabsent");
  
  if(translate){
    params.translate    = 1;
  }else{
    params.translate    = 0;
  }
  params.w_USCOREabsent   = ajCharNewS(wabsent);
  params.w_USCOREfilename = "w_value.csv";
  params.w_USCOREoutput   = "stdout";

  while(ajSeqallNext(seqall,&seq)){  
    soap_init(&soap);
    
    inseq = NULL;
    if(ajSeqGetFeat(seq)){
      ajStrAssignS(&filename,ajSeqallGetFilename(seqall));
      FILE *fp;
      char buf[256];
      fp = fopen(ajCharNewS(filename),"r");
      while(fgets(buf,256,fp) != NULL){
        ajStrAppendC(&inseq,buf);
      }
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }
    
    char* in0;
    in0 = ajCharNewS(inseq);
    if(soap_call_ns1__cai(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      ajStrAppendC(&filename,".csv");
      if(get_file(jobid,ajCharNewS(filename))==0){
	printf("Retrieval successful\n");
      }else{
	printf("Retrieval unsuccessful\n");
      }
    }else{
      soap_print_fault(&soap,stderr);
    }
    
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }
  
  embExit();
  return 0;
}
