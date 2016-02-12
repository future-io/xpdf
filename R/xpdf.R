#' Extract text from PDF file
#'
#' An R interface to the \code{pdftotext} utility from \code{xpdf}.
#'
#' @export
#' @useDynLib xpdf
#' @rdname xpdf
#' @aliases xpdf
#' @importFrom Rcpp sourceCpp
#' @param input pdf path or filename
#' @param output txt path or filename
#' @param firstpage first page to convert
#' @param lastpage last page to convert
#' @param layout maintain original physical layout
#' @param table similar to \code{layout} but optimized for tables
#' @param lineprinter use strict fixed-pitch/height layout
#' @param raw keep strings in content stream order
#' @param fixed assume fixed-pitch (or tabular) text
#' @param linespacing fixed line spacing for LinePrinter mode
#' @param clip separate clipped text
#' @param enc output text encoding name
#' @param eol output end-of-line convention (unix, dos, or mac)
#' @param nopgbrk don't insert page breaks between pages
#' @param opw owner password (for encrypted files)
#' @param upw user password (for encrypted files)
#' @param quiet don't print any messages or errors
#' @param cfg configuration file to use in place of .xpdfrc
#' @examples download.file("https://cran.r-project.org/doc/manuals/r-release/R-exts.pdf", "R-exts.pdf")
#' pdfinfo("R-exts.pdf")
#' pdftotext("R-exts.pdf")
pdftotext <- function(input, output = "out.txt", firstpage = 1, lastpage = 0, layout = FALSE,
  table = FALSE, lineprinter = FALSE, raw = FALSE, fixed = 0, linespacing = 0, clip = FALSE,
  enc = "", eol = "", nopgbrk = FALSE, opw = "", upw = "", quiet = FALSE, cfg = ""){
  wrap_pdftotext(input, output, firstpage, lastpage, layout, table, lineprinter, raw, fixed,
    linespacing, clip, enc, eol, nopgbrk, opw, upw, quiet, cfg)
}

#' @export
#' @rdname xpdf
pdfinfo <- function(input, enc = "", opw = "", upw = "", cfg = ""){
  wrap_pdfinfo(input, enc, opw, upw, cfg)
}

#' @export
#' @rdname xpdf
xpdf_version <- function(){
  wrap_version()
}
