#' XPDF utilities
#'
#' Dump wrappers around xpdf command line utilites.
#'
#' @aliases xpdf poppler
#' @rdname xpdf
#' @param file path or name of the PDF file
#' @param args additional arguments passed to the cmd util
#' @export
#' @examples setwd(tempdir())
#' download.file("https://cran.r-project.org/doc/manuals/r-release/R-intro.pdf", "R-intro.pdf")
#' pdfinfo("R-intro.pdf")
#' pdffonts("R-intro.pdf")
#'
#' # convert to text
#' pdftotext("R-intro.pdf")
#' browseURL("R-intro.txt")
#'
#' # convert to html
#' pdftohtml("R-intro.pdf")
#' browseURL("R-intro.html")
pdfinfo <- function(file, args = ""){
  exec_util("pdfinfo", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdftotext <- function(file, args = ""){
  exec_util("pdftotext", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdftohtml <- function(file, args = ""){
  exec_util("pdftohtml", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdftops <- function(file, args = ""){
  exec_util("pdftops", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdfimages <- function(file, args = ""){
  exec_util("pdfimages", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdffonts <- function(file, args = ""){
  exec_util("pdffonts", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdfseparate <- function(file, args = ""){
  exec_util("pdfseparate", paste(normalizePath(file), args))
}

#' @rdname xpdf
#' @export
pdfunite <- function(file, args = ""){
  exec_util("pdfunite", paste(normalizePath(file), args))
}

exec_util <- function(name, args){
  postfix <- ifelse(identical(.Platform$OS.type, "windows"), ".exe", "")
  path <- system.file(paste0("bin/", name, postfix), package = "xpdf")
  stopifnot(file.exists(path))
  system2(path, args, stdout = TRUE)
}
