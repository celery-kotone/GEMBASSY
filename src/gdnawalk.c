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
  embInitPV("gdnawalk",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;
  struct ns1__dnawalkInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  AjBool    accid    = 0;
  AjPStr    filename  = NULL;
  char*     jobid;
  
  seqall = ajAcdGetSeqall("sequence");
  accid  = ajAcdGetBoolean("accid");

  params.gmap = 0;

  while(ajSeqallNext(seqall,&seq)){

    soap_init(&soap);

    soap.send_timeout = 0; 
    soap.recv_timeout = 0;

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq);
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }
    
    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));

    if(!ajSeqGetFeat(seq) && !accid)
      fprintf(stderr,"Sequence does not have features\nProceeding with sequence accession ID\n");

    if(soap_call_ns1__dnawalk(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      ajStrAppendC(&filename,".png");
      if(get_file(jobid,ajCharNewS(filename))==0){
        fprintf(stderr,"Retrieval successful\n");
      }else{
        fprintf(stderr,"Retrieval unsuccessful\n");
      }
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