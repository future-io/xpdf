#include <Rinternals.h>
#include <config.h>

SEXP poppler_version(){
  return mkString(VERSION);
}
