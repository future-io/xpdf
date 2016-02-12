//========================================================================
//
// pdfinfo.cc
//
// Copyright 1998-2013 Glyph & Cog, LLC
//
//========================================================================

#include <aconf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "parseargs.h"
#include "GString.h"
#include "gmem.h"
#include "gfile.h"
#include "GlobalParams.h"
#include "Object.h"
#include "Stream.h"
#include "Array.h"
#include "Dict.h"
#include "XRef.h"
#include "Catalog.h"
#include "Page.h"
#include "PDFDoc.h"
#include "CharTypes.h"
#include "UnicodeMap.h"
#include "TextString.h"
#include "config.h"

#include <Rcpp.h>
using namespace Rcpp;

Rcpp::String getMeta(GString *meta){
  return (!meta) ? Rcpp::String() : Rcpp::String(meta->getCString());
}

Rcpp::String getField(Dict *infoDict, const char *key) {
  Object obj;
  if (infoDict->lookup(key, &obj)->isString()) {
    return Rcpp::String(obj.getString()->getCString());
  }
  obj.free();
  return Rcpp::String();
}

Rcpp::String getDate(Dict *infoDict, const char *key) {
  Object obj;
  char *s;
  int year, mon, day, hour, min, sec, n;
  struct tm tmStruct;
  char buf[256];

  if (infoDict->lookup(key, &obj)->isString()) {
    s = obj.getString()->getCString();
    if (s[0] == 'D' && s[1] == ':') {
      s += 2;
    }
    if ((n = sscanf(s, "%4d%2d%2d%2d%2d%2d",
                    &year, &mon, &day, &hour, &min, &sec)) >= 1) {
      switch (n) {
      case 1: mon = 1;
      case 2: day = 1;
      case 3: hour = 0;
      case 4: min = 0;
      case 5: sec = 0;
      }
      tmStruct.tm_year = year - 1900;
      tmStruct.tm_mon = mon - 1;
      tmStruct.tm_mday = day;
      tmStruct.tm_hour = hour;
      tmStruct.tm_min = min;
      tmStruct.tm_sec = sec;
      tmStruct.tm_wday = -1;
      tmStruct.tm_yday = -1;
      tmStruct.tm_isdst = -1;
      // compute the tm_wday and tm_yday fields
      if (mktime(&tmStruct) != (time_t)-1 &&
          strftime(buf, sizeof(buf), "%c", &tmStruct)) {
        obj.free();
        return Rcpp::String(buf);
      } else {
        obj.free();
        return Rcpp::String(s);
      }
    } else {
      obj.free();
      return Rcpp::String(s);
    }
  }
  return Rcpp::String();
}

// [[Rcpp::export]]
List wrap_pdfinfo(std::string R_input, std::string R_enc, std::string R_opw, std::string R_upw, std::string R_config) {
  GString *fileName = new GString(R_input.c_str());
  const char *textEncName = R_enc.c_str();
  const char *ownerPassword = R_opw.c_str();
  const char *userPassword = R_upw.c_str();
  const char *cfgFileName = R_config.c_str();

  // read config file
  globalParams = new GlobalParams(cfgFileName);
  if (textEncName[0]) {
    globalParams->setTextEncoding(textEncName);
  }

  // get mapping to output encoding
  UnicodeMap *uMap = globalParams->getTextEncoding();
  if (!uMap)
    throw std::runtime_error("Couldn't get text encoding");

  // open PDF file
  GString *ownerPW = NULL;
  if (ownerPassword[0] != '\001')
    ownerPW = new GString(ownerPassword);

  GString *userPW = NULL;
  if (userPassword[0] != '\001')
    userPW = new GString(userPassword);

  PDFDoc *doc = new PDFDoc(fileName, ownerPW, userPW);
  if (userPW)
    delete userPW;

  if (ownerPW)
    delete ownerPW;

  if(!doc->isOk()){
    uMap->decRefCnt();
    delete doc;
    delete globalParams;
  }

  Object info;
  doc->getDocInfo(&info);

  //return list with stuff
  List out = List::create(
    Named("Title") = getField(info.getDict(), "Title"),
    Named("Subject") = getField(info.getDict(), "Subject"),
    Named("Keywords") = getField(info.getDict(), "Keywords"),
    Named("Author") = getField(info.getDict(), "Author"),
    Named("Creator") = getField(info.getDict(), "Creator"),
    Named("Producer") = getField(info.getDict(), "Producer"),
    Named("CreationDate") = getDate(info.getDict(), "CreationDate"),
    Named("ModDate") = getDate(info.getDict(), "ModDate"),
    Named("Tagged") = (bool) doc->getStructTreeRoot()->isDict(),
    Named("Form") = (bool) (doc->getCatalog()->getAcroForm())->isDict(),
    Named("Pages") = doc->getNumPages(),
    Named("Encrypted") = (bool) doc->isEncrypted(),
    Named("Width") = doc->getPageCropWidth(1),
    Named("Height") = doc->getPageCropHeight(1),
    Named("Rotation") = doc->getPageRotate(1),
    Named("Optimized") = (bool) doc->isLinearized(),
    Named("Version") = doc->getPDFVersion(),
    Named("Meta") = getMeta(doc->readMetadata())
  );
  info.free();
  return out;
}
