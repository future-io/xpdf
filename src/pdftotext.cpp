#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "GString.h"
#include "gmem.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "TextOutputDev.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "TextString.h"
#include "config.h"

#define STRICT_R_HEADERS
#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
std::string wrap_version(){
  return xpdfVersion;
}

// [[Rcpp::export]]
std::string wrap_pdftotext(std::string R_input, std::string R_output, int firstPage, int lastPage,
  bool physLayout, bool tableLayout, bool linePrinter, bool rawOrder, double fixedPitch,
  double fixedLineSpacing, bool clipText, std::string R_enc, std::string R_eol, bool noPageBreaks,
  std::string R_opw, std::string R_upw, bool quiet, std::string R_config) {

  /* convert arguments */
  GString *fileName = new GString(R_input.c_str());
  GString *textFileName = new GString(R_output.c_str());
  const char *textEncName = R_enc.c_str();
  char *textEOL = (char*) R_eol.c_str();
  const char *ownerPassword = R_opw.c_str();
  const char *userPassword = R_upw.c_str();
  const char *cfgFileName = R_config.c_str();

  PDFDoc *doc;
  GString *ownerPW, *userPW;
  TextOutputControl textOutControl;
  TextOutputDev *textOut;
  UnicodeMap *uMap;
  Object info;

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  if (textEncName[0]) {
    globalParams->setTextEncoding(textEncName);
  }
  if (textEOL[0]) {
    if (!globalParams->setTextEOL(textEOL)) {
      throw std::invalid_argument("Bad 'eol' argument value");
    }
  }
  if (noPageBreaks) {
    globalParams->setTextPageBreaks(gFalse);
  }
  if (quiet) {
    globalParams->setErrQuiet(quiet);
  }

  // get mapping to output encoding
  if (!(uMap = globalParams->getTextEncoding())) {
    throw std::runtime_error("Couldn't get text encoding");
  }

  // open PDF file
  if (ownerPassword[0] != '\001') {
    ownerPW = new GString(ownerPassword);
  } else {
    ownerPW = NULL;
  }
  if (userPassword[0] != '\001') {
    userPW = new GString(userPassword);
  } else {
    userPW = NULL;
  }
  doc = new PDFDoc(fileName, ownerPW, userPW);
  if (userPW) {
    delete userPW;
  }
  if (ownerPW) {
    delete ownerPW;
  }
  if (!doc->isOk()) {
    goto err2;
  }

  // check for copy permission
  if (!doc->okToCopy())
    throw std::runtime_error("Copying of text from this document is not allowed.");

  // get page range
  if (firstPage < 1) {
    firstPage = 1;
  }
  if (lastPage < 1 || lastPage > doc->getNumPages()) {
    lastPage = doc->getNumPages();
  }

  // write text file
  if (tableLayout) {
    textOutControl.mode = textOutTableLayout;
    textOutControl.fixedPitch = fixedPitch;
  } else if (physLayout) {
    textOutControl.mode = textOutPhysLayout;
    textOutControl.fixedPitch = fixedPitch;
  } else if (linePrinter) {
    textOutControl.mode = textOutLinePrinter;
    textOutControl.fixedPitch = fixedPitch;
    textOutControl.fixedLineSpacing = fixedLineSpacing;
  } else if (rawOrder) {
    textOutControl.mode = textOutRawOrder;
  } else {
    textOutControl.mode = textOutReadingOrder;
  }
  textOutControl.clipText = clipText;
  textOut = new TextOutputDev(textFileName->getCString(), &textOutControl, gFalse);
  if (textOut->isOk()) {
    doc->displayPages(textOut, firstPage, lastPage, 72, 72, 0, gFalse, gTrue, gFalse);
  } else {
    delete textOut;
    goto err3;
  }
  delete textOut;

  // clean up
  err3:
    delete textFileName;
  err2:
    delete doc;
  uMap->decRefCnt();
  delete globalParams;

  //check for memory leaks (not allowed in R)
  //Object::memCheck(stderr);
  //gMemReport(stderr);
  return R_output;
}
