#include "stdafx.h"
//#include "afx.h"
#include "UTF8Util.h"


//UTF8Util::UTF8Util(void)
//{
//}


//UTF8Util::~UTF8Util(void)
//{
//}

void UTF8Util::SkipUtf8Bom(FILE* fp) {
  /* UTF-8 BOM is EF BB BF */
  if (fp == NULL) {
    return;
  }
  /* If we are not at beginning of file, return */
  if (ftell(fp) != 0) {
    return;
  }

  /* Try to read first 3 bytes */
  int bom[3];
  int n;
  for (n = 0; n <= 2 && (bom[n] = getc(fp)) != EOF; n++) {}
  /* If we can only read <3 bytes, push them back */
  /* Or if first 3 bytes is not BOM, push them back */
  if ((n < 3) || (bom[0] != 0xEF) || (bom[1] != 0xBB) || (bom[2] != 0xBF)) {
    for (n--; n >= 0; n--) {
      ungetc(bom[n], fp);
    }
  }

  /* Otherwise, BOM is already skipped */
}
