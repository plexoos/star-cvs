/* these defines MAY NOT be moved intto a header file, they are of use only in this source file  M.W.S */
#define SWP0312  {help = data[i] ;  cdp[i*4]   = ch[3] ; cdp[i*4+1] = ch[2] ;  cdp[i*4+2] = ch[1] ;  cdp[i*4+3] = ch[0] ;}
#define SWP12    {help = data[i] ;  cdp[i*4]   = ch[0] ; cdp[i*4+1] = ch[2] ; cdp[i*4+2] = ch[1] ; cdp[i*4+3] = ch[3] ;}
#define SWP04    { help = data[i];  cdp[i*4]   = ch[3] ; cdp[i*4+1] = ch[1] ; cdp[i*4+2] = ch[2] ; cdp[i*4+3] = ch[0] ;}
#define SWP_SHORT {help = data[i] ; cdp[i*4] =  ch[1] ; cdp[i*4+1] = ch[0] ; cdp[i*4+2] = ch[3] ; cdp[i*4+3] = ch[2] ;}


int swap_raw(int data_byte_ordering,int* data, int size)
{
  if(data_byte_ordering == 0x04030201) return(0) ;
  char* cdp = (char*) data ;
  int help ;
  char* ch = (char*)&help ;
  int i ;
  if(data_byte_ordering == 0x01020304) { /* swap 0-3 1 1-2 */ for(i = 0  ; i < size ;i++) SWP0312 return(1) ; }
  if(data_byte_ordering == 0x04020301) { /* swap 1-2 */ for(i = 0  ; i < size ;i++)SWP12  return(2) ; }
  if(data_byte_ordering == 0x01030204) { /* swap 0-4 */ for(i = 0  ; i < size ;i++)SWP04  return(3) ; }
  return(-1) ; /* realy bad */
};

int swap_short(int data_byte_ordering,int* data, int size)
{
  if(data_byte_ordering == 0x04030201) return(0) ;
  char* cdp = (char*) data ;
  int help ;
  char* ch = (char*)&help ;
  int i ;
  if(data_byte_ordering == 0x01020304) { /* swap 0-3 1 1-2 */ for(i = 0  ; i < size ;i++) SWP_SHORT return(1) ; }
  if(data_byte_ordering == 0x04020301) { return -1; }
  if(data_byte_ordering == 0x01030204) { return -1; }
  return(-1) ; /* realy bad */
};
#undef  SWP0312
#undef  SWP12
#undef  SWP04
#undef  SWP_SHORT


