#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int 
main(int argc, char *argv[])
{
  embInitPV("gscs", argc, argv, "GEMBASSY", "1.0.0");

  struct soap	  soap;
  struct ns1__scsInputParams params;

  AjPSeqall	  seqall;
  AjPSeq	  seq;
  AjPStr	  inseq = NULL;
  AjPStr	  id = NULL;
  ajint		  translate = 0;
  AjPStr	  delkey = NULL;
  AjPStr	  accid = NULL;
  char           *result;

  AjBool	  show = 0;
  AjPFile	  outf = NULL;

  seqall = ajAcdGetSeqall("sequence");
  id = ajAcdGetString("id");
  delkey = ajAcdGetString("delkey");
  translate = ajAcdGetBoolean("translate");
  accid = ajAcdGetString("accid");

  show = ajAcdGetToggle("show");
  outf = ajAcdGetOutfile("outfile");

  if (translate) {
    params.translate = 1;
  } else {
    params.translate = 0;
  }
  params.id = ajCharNewS(id);
  params.del_USCOREkey = ajCharNewS(delkey);

  while (ajSeqallNext(seqall, &seq)) {

    soap_init(&soap);

    inseq = NULL;

    if (ajSeqGetFeat(seq) && !ajStrGetLen(accid)) {
      inseq = getGenbank(seq);
      ajStrAssignS(&accid, ajSeqGetAccS(seq));
    } else {
      if (!ajStrGetLen(accid)) {
	fprintf(stderr, "Sequence does not have features\n");
	fprintf(stderr, "Proceeding with sequence accession ID\n");
	ajStrAssignS(&accid, ajSeqGetAccS(seq));
      }
      if (!valID(ajCharNewS(accid))) {
	fprintf(stderr, "Invalid accession ID, exiting");
	return 1;
      } else {
	ajStrAssignS(&inseq, accid);
      }
    }

    char           *in0;
    in0 = ajCharNewS(inseq);

    if (soap_call_ns1__scs(
			   &soap, NULL, NULL,
			   in0, &params, &result
			   ) == SOAP_OK) {
      if (show)
	ajFmtPrint("Sequence: %S SCS: %S\n",
		   accid, ajStrNewC(result));
      else
	ajFmtPrintF(outf, "Sequence: %S SCS: %S\n",
		    accid, ajStrNewC(result));
    } else {
      soap_print_fault(&soap, stderr);
    }

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  if (outf)
    ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);

  embExit();
  return 0;
}
