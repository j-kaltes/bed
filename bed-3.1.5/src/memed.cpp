#include "editor.h"
#include "desturbe.h"
#include "misc.h"

/*
http://masm32.com/board/index.php?topic=2130.0
https://msdn.microsoft.com/en-us/library/windows/desktop/cc644950(v=vs.85).aspx
http://www.needfulsoftware.com/Documentation/RawDiskAccess2
https://support.microsoft.com/en-us/help/138434/how-win32-based-applications-read-cd-rom-sectors-in-windows-nt

Allocates memory starting at a sector boundary (sometimes needed for \\.\PhysicalDrive0)

*/

bool Editor::searchdontedit(void) {
	if(backgroundsearch)  {
		beep();
		output("Don't edit during background search. Press Alt-M,s to stop it");
		return true;
		}	
	return false;
	}

OFFTYPE Editor::posputbuf(OFFTYPE pos,char *buf,OFFTYPE len){
	if(searchdontedit())
		return -1;
	OFFTYPE ret=0;
	INCDEST();
	memorize();
	int bs=blocksize();
	OFFTYPE end=pos+len;
	BLOCKTYPE bl1=pos/bs,bllast=(end-1)/bs,blnr=bllast-bl1+1;
	char *sbuf=(char *)alignedalloc(bs,blnr*bs);
	int inblock=pos%bs,inend=end%bs;
	mem.getblocks(bl1,sbuf,blnr);
	appendstrtoundo( pos,sbuf+inblock, len);
	BLOCKTYPE startbl;
	int over;
	if(blnr==1&&inend) {
		over=inend;
		goto PARTFIRST;
		}
	else
		over=bs;
	if(inblock) {
PARTFIRST:
		const int in1=over-inblock;
		memcpy(sbuf+inblock,buf,in1);
		const int did1=mem.putblock(bl1,sbuf);
		if(did1<over) {
			message("1: putblock(%d...) return %d, should be more than %d",bl1,did1,over);
			goto ENDALL;
			}
		ret+=in1;
		startbl=bl1+1;
		}
	else	
		startbl=bl1;
	if(blnr>1&&inend) {
		char * const lastblbuf= sbuf+(bllast-bl1)*bs;
		memcpy(lastblbuf,buf+len-inend,inend);
		const int did2=mem.putblock(bllast,lastblbuf);
		if(did2<inend) {
			message("2: putblock(%d...) return %d, should be more than %d",bllast,did2,inend);
			goto ENDALL;
			}
		ret+=inend;
		bllast--;
		}
	for(int bufpos=(startbl-bl1)*bs-inblock;startbl<=bllast;startbl++,bufpos+=bs) {
		const int did3=mem.putblock(startbl,buf+bufpos);
		if(did3<bs) {
			message("3: putblock(%d...) return %d, should be %d",startbl,did3,bs);
			goto ENDALL;
			}
		ret+=did3;
		}
ENDALL:
	alignedfree(sbuf);
	modified++;
	DECDEST();
	return ret;
	}
OFFTYPE Editor::putmem(OFFTYPE pos,char *buf,OFFTYPE len){
	OFFTYPE ret=posputbuf(pos,buf,len);
	if(pos>=filepos&&(pos<(filepos+screenmax()))) {
		updated=0;
	//	dofromfile=1;
		fromfile();
		}
	return ret;
	}

OFFTYPE Editor::putbuf(char *buf,OFFTYPE buflen) {
	int pos=lowborder();
	int len=minnum(buflen,mem.size()-(pos+filepos));
	memorize();
	OFFTYPE ret=posputbuf(pos+filepos,buf,len);
	dofromfile=1;
	topos(filepos+pos+len-1);
	return ret;
	}
OFFTYPE Editor::getmem(OFFTYPE pos,char *buf,OFFTYPE len){
		memorize();
		return mem.getpart(pos,len,buf);
		}

BEDS_SIZE_T Editor::getblock(BLOCKTYPE blocknr,char *buf)  {
	memorize();
	return mem.getblock(blocknr,buf);
	}
OFFTYPE Editor::getpart(OFFTYPE begin, OFFTYPE len, char * buf) {return mem.getpart(begin,len,buf);}
OFFTYPE Editor::putpart(OFFTYPE begin, OFFTYPE len, char * buf) {return mem.putpart(begin,len,buf);}



int Editor::fromfile(OFFTYPE pos) { 
	dofromfile=0;
 /*Onderscheid: onveranderd, maar andere positie, of verandering*/

//	if(modified)
		return view.fill(pos,screenmax());
/*	else
		return view.getdata(pos,screenmax())==NULL;		*/
	}
/*
int Editor::fromfile(OFFTYPE pos) { 
	dofromfile=0;
	int bs=blocksize();
	BLOCKTYPE  blstart=(pos/bs);
	OFFTYPE retlen=mem.getblocks(blstart,view.buf(),view.getnr());	
	OFFTYPE begin=blstart*bs;
	view.setstart(pos-begin);
	return retlen;
	}
*/
int Editor::fromfile(void) { return(fromfile(filepos)); };

void Editor::memorize(void) {if(changed) {

				changed=0;
	if(searchdontedit())
		return;

				INCDEST();
		//		mem.putpart(filepos,shownbufsize,viewbuf);
//	BLOCKTYPE blstart=(filepos-view.getstart())/blocksize();
//	OFFTYPE retlen=mem.putblocks(blstart,view.buf(),view.getnr());	
view.retain(filepos);
				DECDEST();
				changed=0;
				modified++;
				}; };



//Viewbuf::Viewbuf(Editor &e,int size):mem(e.mem),offset(OFFTYPEINVALID),endoffset(0),start(0),shown(0),blnr(0),minbl((size-1)/e.mem.blocksize()+2),str(NULL),tmpbuf(NULL) { }

Viewbuf::Viewbuf(Editor &e,int size):Viewbuf(e.mem,size) {}




OFFTYPE Viewbuf::fill(OFFTYPE pos,int len) {
	int bs=mem.blocksize();
	BLOCKTYPE  blstart=(pos/bs),bls=(pos+len-1)/bs+1-blstart;
	 offset=blstart*bs;
	if(!str||(bls>blnr&&(alignedfree(str),true))) {
	 	blnr=maxnum(minbl,bls);
		int size=bs*blnr;
		str=(char *) alignedalloc(bs,size*sizeof(char));
		}
	OFFTYPE retlen=mem.getblocks(blstart,str,blnr);	
	setstart(pos-offset);
	endoffset=retlen+offset;
	int maxlen=endoffset-pos;
	shown=minnum(len,maxlen);;
	return retlen;
	}
OFFTYPE Viewbuf::retain(OFFTYPE pos) {
	if(str) {
		BLOCKTYPE blstart=(pos-getstart())/mem.blocksize();
		return mem.putblocks(blstart,buf(),getnr());	
		}

//	if(pos>=getoffset()&&pos<getendoffset)
//	return mem.putblocks(getoffset(),buf(),getnr());	
	return OFFTYPEINVALID;
	}
char *Viewbuf::getdata(OFFTYPE pos,int len) {
  OFFTYPE end=pos+len;
  if(str) {
    if(pos>=offset) {
      if(end<=getendoffset()) {
        size_t in=pos-offset;
        setstart(in); 
        shown=len; 
        return str+in;
	}
    if(pos<getendoffset()) {
	    tmpbuf=(char *)realloc(tmpbuf,len);
	    const int leftlen=getendoffset()-pos;
	    const int allen=mem.blocksize()*blnr;
	    memcpy(tmpbuf,str+allen-leftlen,leftlen);
	    const int nog=len-leftlen;
	    fill(getendoffset(),nog);
	    const int in=pos+leftlen-offset;
	    memcpy(tmpbuf+leftlen,str+in,nog);
	   return tmpbuf;
    	 }
	}
    else {
	    if(end<=getendoffset()&&end>=offset) {
		    tmpbuf=(char *)realloc(tmpbuf,len);
		    const int endlen=end-offset,nog=len-endlen;
		    memcpy(tmpbuf+nog,str,endlen);
		    fill(pos,nog);
		    const int in=pos-offset;
		    memcpy(tmpbuf,str+in,nog);
		    return tmpbuf;

		}
    }
  }
	if(fill(pos,len)==OFFTYPEINVALID)
		return NULL;
	return view();
	}
