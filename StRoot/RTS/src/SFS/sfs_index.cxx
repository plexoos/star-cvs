#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "sfs_index.h"
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <rtsLog.h>

#include <byteswap.h>
#define swap16(x) bswap_16(x)
#define swap32(x) bswap_32(x)

//int debug = 0;

/********************************/
/*  Utilities                   */
/********************************/

int mstrcmp(char *s1, char *s2)
{
  if((s1[0] == '#') && (s2[0] == '#')) {
    int x1;
    int x2;

    x1 = atoi(&s1[1]);
    x2 = atoi(&s2[1]);
    if(x1 > x2) return 1;
    if(x1 < x2) return -1;
    return 0;
  }
  return strcmp(s1,s2);
}


//    /dir_1/dir_2/file  -->  /
//    file               -->  file
//    dir_1/file         -->  dir_1/
//    dir_1/             -->  dir_1/
//    ""                 -->  ""
//
void striptofirst(char *str) 
{
  while(*str != '\0') {
    if(*str == '/') *(str+1) = '\0';
    str++;
  }
}

// /dir_1/dir_2  --> /dir_1
// dir_1/dir_2/dir_3 --> /dir_1/dir_2

void striptofirstdir(char *str)
{
  int scount = 0;
  while(*str != '\0') {
    if(*str == '/') scount++;
    if(scount >= 2) {
      *str = '\0';
    }
    else str++;
  }
}

//    /dir_1/dir_2/file  --> file
//    file               --> file
//    dir_1/file         --> file
//    dir_1/             --> ""
//    ""                 --> ""
//
char *striptofile(char *str)
{
  char *ostr = str;

  while(*str != '\0') {
    if(*str == '/') ostr = str+1;
    str++;
  }

  return ostr;
}

//    /dir_1/dir_2/file  --> /dir_1/dir_2/
//    file               --> /
//    dir_1/file         --> dir_1
//    dir_1/             --> dir_1/
//    ""                 --> /
//
void stripfile(char *str)
{
  char *ostr = str;
  char *lslash = NULL;

  while(*str != '\0') {
    if(*str == '/') lslash = str;
    str++;
  }

  if(lslash) *(lslash+1) = '\0';
  else strcpy(ostr, "/");
}

int SFS_ittr::get(wrapfile *wrap)
{
  //  printf("ittr get file 0x%x %d\n",wrapbuff, fd);
  skipped_bytes = 0;

  char buff[12];
  memset(buff, 0, sizeof(buff));

  wfile = wrap;

  stickypath[0] = '\0';
  ppath[0] = '\0';
  fullpath[0]='\0';
  
  //printf("fileoffset %d\n",fileoffset);
  filepos = 0;
  strcpy(ppath,"/");

  return 0;
}


void SFS_ittr::swapEntry()
{
  //  int swap;
  if(entry.byte_order == 0x04030201) return;

  entry.byte_order = swap32(entry.byte_order);

  entry.byte_order = swap32(entry.byte_order);
  entry.sz = swap32(entry.sz);
  entry.reserved = swap16(entry.reserved);
}


// Return -1 on error
// 0 on ok.
int SFS_ittr::next() 
{
  //if(legacy) return legacy_next();
  skipped_bytes = 0;

  LOG(DBG, "Calling next:  fileoffset=%lld filepos=%d",fileoffset,filepos);
  long long int ret;

  //printf("ittr next\n");

  LOG(DBG, "filepos = %d, entry.sz=%d entry.head_sz=%d",filepos,entry.sz,entry.head_sz);
  if(filepos == 1) {  // need to jump to next...
    ret = wfile->lseek(seeksize(entry.sz), SEEK_CUR);
    if(ret < 0) {
      LOG(ERR,"Error seeking: %s\n",strerror(errno));
      return -1;
    }
    filepos = 2;

    LOG(DBG, "fileoffset=%d --> + %d + %d",
	entry.sz, entry.head_sz);
    fileoffset += seeksize(entry.sz) + entry.head_sz;
  }

  if(filepos == 2) {  
    // update ppath...
    LOG(DBG,"---DIR:  name=%s entry.attr = 0x%x headsz=%d",entry.name, entry.attr,entry.head_sz);
   

    if(entry.attr & SFS_ATTR_NOCD) {
      // ppath unchanged....
      //printf("no change %s\n",entry.name);
    }
    else {   // update ppath regularly...
      if(entry.name[0] == '/') {
	//printf("strcpy %s\n",entry.name);
	strcpy(ppath, entry.name);
      }
      else {
	//printf("ccat %s %s\n",ppath,entry.name);
	strcat(ppath, entry.name);
      }
      stripfile(ppath);
    }

    if(entry.attr & SFS_ATTR_STICKY_CD) {
      strcpy(stickypath,ppath);
      // printf("(set) ppath = %s,  stickypath %s\n",ppath,stickypath);
    }
      

    //printf("----------ppath is now %s  (%s)\n",ppath,entry.name);

    filepos = 0;
  }


  // expect a FILE record, LRHD or DATAP...  jump the rest...
  for(;;) {
    char buff[10];
    int ret = wfile->read(buff, 8);
    
    if(ret == 0) {      // done...
      filepos = -1;
      return 0;  
    }

    if(ret != 8) {
      LOG(ERR, "Error reading next file record...");
      return -1;
    }

    wfile->lseek(-((long long int)8), SEEK_CUR);


    long long int xxx = wfile->lseek(0,SEEK_CUR);
    buff[5] = 0;
    LOG(DBG, "fileoffset=%lld xxx=%lld buff=%s",fileoffset,xxx,buff);


    if(memcmp(buff, "SFS V", 5) == 0) {
      //if(debug) LOG(DBG,"Found SFS version");
      wfile->lseek(12, SEEK_CUR);
      fileoffset += 12;
      skipped_bytes += 12;
      continue;
    }

    if(memcmp(buff, "LRHD", 4) == 0) {
      //if(debug) LOG(DBG,"Found LRHD");

      LOG(DBG, "BFR LRHD %lld %lld",wfile->lseek(0,SEEK_CUR),fileoffset);

      if(nextLRHD() >= 0) {  // good data LRHD... got entry...
	LOG(DBG, "AFTR LRHD %lld %lld",wfile->lseek(0,SEEK_CUR), fileoffset);
     
	LOG(DBG, "lrhd entry.sz = %d",entry.sz);
	return 0;
      }
      
      //LOG(DBG, "Seek: %d",wfile->lseek(0,SEEK_CUR));

      wfile->lseek(60, SEEK_CUR);

      //LOG(DBG, "Seek2: %d",wfile->lseek(0,SEEK_CUR));

      fileoffset += 60;
      skipped_bytes += 60;
      LOG(DBG, "Not a DATA LRHD  fileoffset=%d skipped_bytes=%d seek=%d",
	  fileoffset,skipped_bytes,wfile->lseek(0,SEEK_CUR));

      continue;
    }

    if(memcmp(buff, "DATAP", 5) == 0) {
      //if(debug) LOG(DBG,"Found DATAP");

      LOG(DBG, "Before datap: file=%d filepos=%d offset=%d entrysz=%d",
	  wfile->lseek(0,SEEK_CUR), filepos,fileoffset,entry.sz);

      if(nextDatap() >= 0) {  // DATAP and no following FILE.  got entry
	LOG(DBG, "After datap: file=%d filepos=%d offset=%d entrysz=%d",
	  wfile->lseek(0,SEEK_CUR), filepos,fileoffset,entry.sz);
	return 0;
      }

      wfile->lseek(204, SEEK_CUR);
      fileoffset += 204;
      skipped_bytes += 204;
      continue;
    }

    if(memcmp(buff, "HEAD", 4) == 0) {
      //if(debug) LOG(DBG,"Found HEAD");
      wfile->lseek(12, SEEK_CUR);
      fileoffset += 12;
      skipped_bytes += 12;
      continue;
    }

    if(memcmp(buff, "FILE", 4) == 0) {  // finally!
      break;
    }

    else {
      
      if(wfile->type == WRAP_MEM) {   // ugly hack... done!
	filepos = -1;
	return 0;
      }

      char ttmp[16] ;
      memcpy(ttmp,buff,16) ;
      ttmp[15] = 0 ;
      LOG(ERR,"Error: %s is not a valid specifier\n",ttmp);
      return -1;
    }
  }
  // Read File Desc
  // clear entrybuff
  memset(entryBuff, 0, sizeof(entryBuff));
  
  ret = wfile->read(entryBuff,16);
  if(ret == 0) {
    filepos = -1;
    return 0;  
  }

  if(ret != 16) {
    LOG(ERR,"Error reading file entry: %s (%d)\n",strerror(errno),ret);
    return -1;
  }
  
  //printf("1- compare file: %s\n",entry.type);

  if(memcmp(entry.type,"FILE",4) != 0) {
    LOG(ERR,"Error reading file entry: {%c%c%c%c} Not a file record...\n",
	   entry.type[0],entry.type[1],entry.type[2],entry.type[3]);
    return -1;
  }

  swapEntry();

  // printf("reading... bytes 0x%x,  head_sz %d,  sz = %d\n",
  // entry.byte_order, entry.head_sz, entry.sz);

  ret = wfile->read(entryBuff + 16, entry.head_sz - 16);
  if(ret != entry.head_sz - 16) {
    LOG(ERR,"Error reading file entry: size mismatch ret=%d sz=%d\n",ret,entry.head_sz);

    return -1;
  }


  LOG(DBG,"---DIR:  name=%s entry.attr = 0x%x\n",entry.name, entry.attr);


  // hacks for 2007
  if(strstr(entry.name, "legacy")) {
    entry.attr |= SFS_ATTR_POPSTICKY;
  }

  if(strstr(entry.name, "pad")) {
    entry.attr |= SFS_ATTR_POPSTICKY;
  }

  if(entry.attr & SFS_ATTR_POPSTICKY) {
    // doesn't reset stickypath!
    //printf("---DIR:   (pop) ppath %s stickypath %s\n",ppath,stickypath);
    if(stickypath[0] != '\0') 
      strcpy(ppath, stickypath);
  }

  if(entry.name[0] == '/') {
    strcpy(fullpath, entry.name);
  }
  else {
    strcpy(fullpath, ppath);
    strcat(fullpath, entry.name);
  }

  LOG(DBG,"fullpath %s, entry.name: %s, fileoffset %lld/%d, sz %d  head_sz %d",
      fullpath, entry.name, fileoffset, filepos, entry.sz, entry.head_sz);

  filepos = 1;
  return 0;
}

char *SFS_getpayload(char *buff)
{
  SFS_File *f = (SFS_File *)buff;
  return (buff + f->head_sz);
}


int sfs_index::writeFsHeader()
{
  static char *volumeSpec = "SFS V00.01\0\0\0";
  int ret;
  int sz;
  char *bb;
  static SFS_Header head;
  
  bb = volumeSpec;
  sz = 12;
  while(sz) {
    ret = wfile.write(volumeSpec, sz);
    
    if(ret < 0) {
      return -1;
    }
    sz -= ret;
    bb += ret;
  }
  
  memcpy(head.type, "HEAD", 4);
  head.byte_order = 0x04030201;
  head.time = time(NULL);
  
  bb = (char *)&head;
  sz = sizeof(head);
  while(sz) {
    ret = wfile.write(bb, sz);
    
    if(ret < 0) {
      return -1;
    }

    sz -= ret;
    bb += ret;
  }
  
  return 0;
}

int sfs_index::write(char *fn, char *buff, int size)
{
  int ret;
  char *bb;
  int sz;
  static char path[256];
  int attr = SFS_ATTR_NOCD;

  static char b[256];
  SFS_File *file = (SFS_File *)b;

  if(cdchanged) {
    // (a) if filename is absolute filename do nothing, leave cdchanged as is
    // (b) if filename has no subdirs, then use fullpath and unset NOCD
    // (c) if filename has subdirs, then write a directory entry separately
    
    //printf("cdchanged\n");

    if(fn[0] == '/') {
      strcpy(path, fn);
    }
    else {
      // any non-trailing slash?
      cdchanged = 0;        // now we do a cd either way!

      int subdirs = 0;
      int len = strlen(fn);
      for(int i=0;i<len-1;i++) {
	if(fn[i] == '/') {
	  subdirs = 1;
	  break;
	}
      }

      if(!subdirs) {
	getFullPath(path, fn);
	attr = 0;
      }
      
      if(subdirs) {
	memcpy(file->type, "FILE", 4);
	file->byte_order = 0x04030201;
	file->sz = 0;
	file->head_sz = seeksize(strlen(cwd)+1) + sizeof(SFS_File) - 4;
	file->attr = 0;
	strcpy(file->name, cwd);
	
	bb = (char *)file;
	sz = file->head_sz;
	
	while(sz) {
	  ret = wfile.write(bb, sz);
	  if(ret < 0) {
	    return -1;
	  }
	  
	  sz -= ret;
	  bb += ret;
	}
	
	strcpy(path, fn);
      }
    }
  }
  else {
    strcpy(path, fn);
  }

  //printf("cd changed = %d  path = %s\n",cdchanged,path);
  

  
  memcpy(file->type, "FILE", 4);
  file->byte_order = 0x04030201;
  file->sz = size;
  file->head_sz = seeksize(strlen(path)+1) + sizeof(SFS_File) - 4;
  file->attr = attr;

  strcpy(file->name, path);
  
  bb = (char *)file;
  sz = file->head_sz;
  
  while(sz) {
    ret = wfile.write(bb, sz);
    if(ret < 0) {
      return -1;
    }

    sz -= ret;
    bb += ret;
  }

  sz = size;
  bb = buff;

  while(sz) {
    ret = wfile.write(bb, sz);
    if(ret < 0) {
      return -1;
    }

    sz -= ret;
    bb += ret;
  }

  // pad to multiple of 4 bytes.
  char *zero = "\0\0\0\0";
  sz = seeksize(size) - size;
  bb = zero;
  
  while(sz) {
    ret = wfile.write(bb, sz); 
    if(ret < 0) {
      return -1;
    }
    
    sz -= ret;
    bb += ret;
  }
  
  return size;
}

int sfs_index::getwritevsz(fs_iovec *fsiovec, int n)
{
  int sz = 0;
  for(int i=0;i<n;i++) {
    if(fsiovec[i].filename) {
      sz += seeksize(strlen(fsiovec[i].filename)+1);
      sz += sizeof(SFS_File) - 4;
    }

    sz += seeksize(fsiovec[i].len);
  }
  return sz;
}

int sfs_index::writev(fs_iovec *fsiovec, int n)
{
  return writev_sticky(fsiovec, n, NULL);
}


// Non-interruptable writev
int sfs_index::writev_call_retry(int fd, iovec *iovec, int vec)
{
  
  struct iovec iovec_new[20];
  int vec_new=0;

  int len=0;
  for(int i=0;i<vec;i++) {
    len += iovec[i].iov_len;
  }

  int ret = ::writev(fd, iovec, vec);

  if(ret != len) {
   for(int i=0;i<vec;i++) {
      LOG(DBG, "socket error iovec[%d].base=%d,  len=%d",
	  i,iovec[i].iov_base,iovec[i].iov_len);
    }

   LOG(NOTE, "writev failed: ret=%d of %d, err=%s.  will retry",ret, len, strerror(errno));
  }

  // keep retrying!
  if(ret < 0) {
    if((errno != EINTR) &&
       (errno != EAGAIN)) {
      
      LOG(ERR, "Error on writev (%s)",strerror(errno));
    }
  
    ret = 0;
  }

  if(ret != len) {  // Need to retry...
    int nstart = ret;
    LOG(WARN, "writev only wrote %d of %d... Retry", ret, len);
    

    for(int i=0;i<vec;i++) {
      if(nstart < (int)iovec[i].iov_len) {
	char *base = (char *)iovec[i].iov_base;
	base += nstart;
	
	iovec_new[vec_new].iov_base = (void *)base;
	iovec_new[vec_new].iov_len = iovec[i].iov_len - nstart;
	vec_new++;
      }
      
      nstart -= iovec[i].iov_len;
      if(nstart<0) nstart = 0;
    }

    int ret2 = writev_call_retry(fd, iovec_new, vec_new);

    ret += ret2;
    LOG(WARN, "writev retry returned %d (retry was %d)",ret,ret2);
  }
  return ret;
}



// Header is an optional, non-fs file header
// of course in stardaq, it is the iccp2k ethernet header...
// this is now threadsafe!


int sfs_index::writev_sticky(fs_iovec *fsiovec, int n, char *sticky)
{
  iovec iovec[20];
  char _buff[(sizeof(SFS_File) + 40)*50];
  char *b = _buff;

  int i;
  int vec=0;

//   if(!writevbuff) {
//     writevbuff = (char *)malloc(64*100);
//   }

//   char *b = writevbuff;

  for(i=0;i<n;i++) {   // each file...

    LOG(DBG, "i=%d\n",i);

    if(fsiovec[i].filename) {
      SFS_File *file = (SFS_File *)b;
      
      memcpy(file->type, "FILE", 4);
      file->byte_order = 0x04030201;
      file->sz = fsiovec[i].len;
      file->head_sz = seeksize(strlen(fsiovec[i].filename)+1) + sizeof(SFS_File) - 4;
      file->attr = SFS_ATTR_NOCD;
      if(sticky) {
	if(sticky[i]) {
	  file->attr |= SFS_ATTR_POPSTICKY;
	}
      }
      strcpy(file->name, fsiovec[i].filename);
      

      iovec[vec].iov_base = b;
      iovec[vec].iov_len = file->head_sz;
      LOG(DBG, "fn:   iovec[%d] name=%s: base=0x%x(0x%x) len=%d",
	vec,fsiovec[i].filename,
	iovec[vec].iov_base,
	  
	iovec[vec].iov_len);


      vec++;
   
      b += file->head_sz;
    }
     
    iovec[vec].iov_base = fsiovec[i].buff;
    iovec[vec].iov_len = seeksize(fsiovec[i].len);

    LOG(DBG, "iovec[%d] name=%s: base=0x%x len=%d",
	vec,fsiovec[vec].filename,
	iovec[vec].iov_base,
	iovec[vec].iov_len);

    vec++;

 
  }

  // we need this check when writing over ethernet!
  int all_size = 0 ;
  for(int i=0;i<vec;i++) {
	all_size += iovec[i].iov_len ;
  }

  // int ret = ::writev(wfile.fd, iovec, vec);     // This can be interrupted
  int ret = writev_call_retry(wfile.fd, iovec, vec);
  if(ret != all_size) {
	LOG(ERR,"writev expects %d, sent %d",all_size,ret) ;
  
  	// supposedly write always writes at least full buffers so let's see if this is tru...
  	int so_far = 0 ;
  	for(int i=0;i<vec;i++) {
		so_far += iovec[i].iov_len ;
		if(so_far == ret) {
			LOG(TERR,"Managed to write %th buffer fully! Need to repeat!",i) ;
		}
		else {
			LOG(TERR,"Trying after buffer %d: have %d, need %d",i,so_far,ret) ;
		}
  	}
  }

  return ret;
}

sfs_index::sfs_index() : fs_index() 
{
  singleDirMount = 0;
  singleDirIttr = NULL;
  root = NULL;
  cw_inode = NULL;

  return;
}

int sfs_index::mountSingleDirMem(char *buffer, int size)
{
  wfile.close();   // just in case
  oflags = O_RDONLY;
  wfile.openmem(buffer, size);
  return mountSingleDir();
}

// returns -1 on error
// returns 0 on eof
// returns 1 on valid dir
int sfs_index::mountSingleDir(char *fn, long long int offset)
{
  LOG(DBG,"the spec is: " __DATE__ ":" __TIME__);

  // open file...
  wfile.close();   // just in case!
  wfile.opendisk(fn, O_RDONLY);
  if(wfile.fd < 0) return wfile.fd;
  wfile.lseek(offset, SEEK_SET);

  return mountSingleDir();
}

int sfs_index::getSingleDirSize(char *fn, long long int offset)
{
  char topdir[40];
  int topdirlen=0;
  topdir[0] = '\0';

  LOG(DBG, "singledirdize file=%s, offset=%lld",fn,offset);

  wfile.close();
  wfile.opendisk(fn, O_RDONLY);
  if(wfile.fd < 0) return wfile.fd;
  wfile.lseek(offset, SEEK_SET);

  int sz = 0;

  SFS_ittr *ittr = new SFS_ittr(offset);
  if(ittr->get(&wfile) < 0) {
    delete ittr;
    wfile.lseek(offset,SEEK_SET);
    return sz;
  }

  sz += ittr->skipped_bytes;

  LOG(DBG, "partial sz=%d, skipped=%d",sz,ittr->skipped_bytes);

  while(ittr->next() >= 0) {

    if(ittr->filepos == -1) break;   // EOF

    // set topdir, if not yet set...
    if(topdir[0] == '\0') {
      strncpy(topdir,ittr->fullpath,40);
      for(int i=0;i<40;i++) {
	if(topdir[i] == '\0') break;
	if((i>0) && (topdir[i] == '/')) {
	  topdir[i] = '\0';
	  topdirlen = strlen(topdir);
	  break;
	}
      }
    }

    LOG(DBG, "Topdir=%s sz(before)=%d",topdir,sz);

    if(memcmp(topdir, ittr->fullpath, topdirlen) != 0) {
      break;
    }

    sz += ittr->skipped_bytes;
    sz += ittr->entry.head_sz;
    int esz = (ittr->entry.sz + 3) / 4;
    esz *= 4;
    sz += esz;
  }

  delete ittr;
  wfile.close();

  LOG(DBG, "returning %d",sz);
  return sz;
}

int sfs_index::mountSingleDir()   // mounts from current position of wfile...
{
  if(singleDirIttr) delete singleDirIttr;
  
  long long int offset = wfile.lseek(0,SEEK_CUR);

  singleDirMount = 1;
  singleDirIttr = new SFS_ittr(offset);
 
  if(singleDirIttr->get(&wfile) < 0) {
    delete singleDirIttr;
    singleDirIttr = NULL;
    singleDirMount = 0;
    
    root = alloc_inode("",0,0);
    strcpy(cwd, "/");
    index_created = 1;
    return -1;
  }

  if(singleDirIttr->next() < 0) {

    root = alloc_inode("",0,0);
    strcpy(cwd, "/");
    index_created = 1;
    
    return -1;
  }

  return mountNextDir();
}

// returns -1 on error
// returns 0 on eof (no directory...)
// returns 1 on valid dir
int sfs_index::mountNextDir()
{
  int files_added=0;

  if(index_created) free_inode(root);
  index_created = 0;

  root = alloc_inode("",0,0);
  strcpy(cwd, "/");
  cw_inode = root;
  index_created = 1;

  // eof?
  if(singleDirIttr->filepos == -1) {
    return 0;
  }

  // get to a non-'/' ittr...
  while(strcmp(singleDirIttr->fullpath,"/") == 0) {
    if(singleDirIttr->next() < 0) {
      return -1;
    }
    if(singleDirIttr->filepos == -1) {
      return 0;
    }
  }
  
  // Have the first file record in place...
  char basedir[256];
  char currdir[256];
  strcpy(basedir, singleDirIttr->fullpath);
  striptofirstdir(basedir);
  

 
  for(;;) {
    
    files_added++;
    addnode(singleDirIttr);

    if(singleDirIttr->next() < 0) {
      //      printf("next() ittr return -1\n");
      return -1;
    }

    if(singleDirIttr->filepos == -1) {  // end of file...
      // printf("next() ittr filepos = -1\n");
      return (files_added > 0) ?  1 : 0;
    }


    strcpy(currdir, singleDirIttr->fullpath);
    striptofirstdir(currdir);

    //if(debug) {
    //  LOG(DBG,"basedir=%s singleDirIttr->fullpath=%s currdir=%s\n",
    //     basedir, singleDirIttr->fullpath, currdir);
    //}

    if(strcmp(basedir, currdir) != 0) {
      return (files_added > 0) ? 1 : 0;
    }
  }

  return 0;
}

int sfs_index::_create()
{
  root = alloc_inode("",0,0);
  strcpy(cwd,"/");
  cw_inode = root;
 
  SFS_ittr ittr;

  


  if(ittr.get(&wfile) < 0) {
    return -1;
  }

  for(;;) {
   
    //printf("ittrnext\n");
    //fflush(stdout);

    if(ittr.next() < 0) {
      //printf("Error itterating...\n");
      //fflush(stdout);
      return -1;
    }

    //printf("ittrnexted\n");
    //fflush(stdout);

    if(ittr.filepos == -1) {  // end of file...

      return 0;
    }

    //printf("addnode\n");
    //fflush(stdout);
    addnode(&ittr);
    //printf("addnode 2\n");
    //fflush(stdout);
  }

  LOG(ERR,"never happens\n");
  return -1;
}

void sfs_index::addnode(SFS_ittr *ittr)
{
  char fullpath[256];
  char thispathonly[256];
  char *next[20];

  LOG(DBG, "addnode: %s %lld %d",ittr->fullpath, ittr->fileoffset, ittr->filepos);

  if(ittr->entry.attr == SFS_ATTR_INVALID) return;
  
  strcpy(thispathonly, ittr->fullpath);
  stripfile(thispathonly);
  
  strcpy(fullpath, ittr->fullpath);

  next[0] = strtok(&fullpath[1], "/");
  
  int nn=1;
  while((next[nn] = strtok(NULL, "/"))) nn++;
    
  fs_inode *inode = root;
    
  for(int i=0;i<nn-1;i++) {
    inode = add_inode(inode, next[i], 0, 0);
  }
    
  inode = add_inode(inode, next[nn-1], ittr->fileoffset + ittr->entry.head_sz, ittr->entry.sz);
}



void sfs_index::dump(char *path, fs_inode *inode) {
  if(inode->fchild == NULL) {
    LOG(DBG,"%s%s\n",path,inode->name);
    return;
  }
  
  char fp[256];
  sprintf(fp,"%s%s/",path,inode->name);
  
  fs_inode *curr = inode->fchild;
  
  while(curr) {
    dump(fp,curr);
    curr = curr->next;
  }
}
fs_inode *sfs_index::add_inode_from(fs_inode *prev, char *name, long long int offset, int sz)
{
  int eq=0;

  fs_inode *parent = prev->parent;

  fs_inode *link = find_last_lesser_neighbor(prev, name, eq);
  if(eq == 0) return link;

  // if(newn < prev, have to go from start...
  if(!link) return add_inode(parent,name,offset,sz);
  
  fs_inode *newn = alloc_inode(name,offset,sz);
  if(!newn) return NULL;

  
  newn->parent = parent;
  newn->next = link->next;
  link->next = newn;
  return newn;
}

fs_inode *sfs_index::add_inode(fs_inode *parent, char *name, long long int offset, int sz)
{
  int eq=0;
  int first=0;
 
  fs_inode *link = find_last_lesser_child(parent, name, first, eq);

  if(eq == 0) return link;
 
  fs_inode *newn = alloc_inode(name,offset,sz);

  if(!newn) return NULL;
  newn->parent = parent;

  if(first) {
    newn->next = parent->fchild;
    parent->fchild = newn;
    
    return newn;
  }

  newn->next = link->next;
  link->next = newn;
  return newn;
}

fs_inode *sfs_index::find_last_lesser_neighbor(fs_inode *first, char *name, int &eq)
{
  eq = -1;
  fs_inode *curr = first;
  if(!curr) return NULL;

  //printf("lln1 %s %s\n",curr->name, name);
  eq = mstrcmp(curr->name, name);
  
  if(eq > 0) return NULL;
  if(eq == 0) return curr;

  fs_inode *next = curr->next;

  while(next) {
    //printf("lln2 %s %s\n",next->name, name);
    eq = mstrcmp(next->name, name);
    if(eq > 0) return curr;
    if(eq == 0) return next;
    curr = next;
    next = curr->next;
  }
  return curr;
}

fs_inode *sfs_index::find_last_lesser_child(fs_inode *parent, char *name, int &first, int &eq)
{
  first = 1;
  eq = -1;

  fs_inode *curr = parent->fchild;
  if(!curr) return NULL;

  //printf("llc1 %s %s\n",curr->name, name);
  eq = mstrcmp(curr->name, name);

  if(eq > 0) return NULL;
  if(eq == 0) {
    return curr;
  }

  first = 0;

  fs_inode *next = curr->next;
  
  while(next) {
    //printf("llc2 %s %s\n",next->name, name);
    eq = mstrcmp(next->name, name);
    if(eq > 0) return curr;
    if(eq == 0) return next;

    curr = next;
    next = curr->next;
  }

  return curr;
}

int sfs_index::getfileheadersz(char *fn)
{
  return (sizeof(SFS_File) - 4 + seeksize(strlen(fn)+1));
}


int sfs_index::putfileheader(char *ptr, char *fn, int filesz, int flags)
{
  return sfs_putfileheader(ptr,fn,filesz,flags);
}

