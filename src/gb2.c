#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int main(int argc, char *argv[]){
  embInitPV("gb2",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  char*     jobid;
  
  seqall = ajAcdGetSeqall("sequence");
  accid  = ajAcdGetString("accid");
  
  while(ajSeqallNext(seqall,&seq)){

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !strlen(ajCharNewS(accid))){
      inseq = getGenbank(seq,ajSeqGetFeat(seq));
    }else{
      if(!strlen(ajCharNewS(accid))){
        fprintf(stderr,"Sequence does not have features\n");
        fprintf(stderr,"Proceeding with sequence accession ID\n");
        ajStrAssignS(&accid,ajSeqGetAccS(seq));
      }
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr,"Invalid accession ID, exiting");
          return 1;
      }else{
        ajStrAssignS(&inseq,accid);
      }
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));


    if(soap_call_ns1__B2(&soap,NULL,NULL,in0,&jobid)==SOAP_OK){
      puts(jobid);
    }else{
      soap_print_fault(&soap,stderr);
    }
  
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);
  
  embExit();
  return 0;
}
