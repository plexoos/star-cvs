/***************************************************************************
 *
 * $Id: StMem.cxx,v 1.3 2001/03/05 01:20:13 perev Exp $
 *
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include "StMem.h"
double  StMem::fUsed=0;

double StMem::Used()
{
  struct mallinfo info;
  info = mallinfo();
  return double(info.uordblks + info.usmblks)/1000000;
}

double StMem::ESize()
{
  static char *ps = 0;
  double res=0;  
  if (!ps) {
    int pid = ::getpid();
    ps = (char*)malloc(20);
    sprintf(ps,"ps -l -p %d",pid);
  }
  FILE *pipe = ::popen(ps,"r");
  if (!pipe) return 0.;
  
  char   psBuf[130];
  psBuf[0] = ' ';
  while( !feof( pipe ) ) {
    psBuf[1]=0;
    if(!fgets( psBuf+1, 128, pipe)) continue;
//    printf("%s\n",psBuf);
    int ifild=0;char *c;

    for (c=psBuf; c[0]; c++) {
      if (c[0]==' ' && c[1]!=' ') ifild++;
      if (ifild == 10) break;
    }
    res = (double)atoi(c+1);
    if (res) break;
  }
  ::pclose(pipe);
  res *=::getpagesize()/(1024.*1024.);

  return res;
}

void StMem::Print(const char *tit)
{
  double used = Used();
  double exec = ESize();

  if (tit) printf("\nStMem::%s",tit);
  printf("\t total =%10.6f heap =%10.6f (%+10.6f)\n",exec, used,used-fUsed);
  fUsed = used;
}
