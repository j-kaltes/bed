

#include <iostream>
#include <windows.h>
#include <WINIOCTL.H>
#include <stdio.h>
using namespace::std;

int printguid(const GUID &gu) { 
const byte *b=gu.Data4;
	printf("%08lx-%04x-%04x",gu.Data1,gu.Data2,gu.Data3);
	printf("-%02x%02x-",b[0],b[1]);
	for(int i=2;i<8;i++)
		printf("%02x",b[i]);
putchar('\n');
	return 0;
}


#undef DEFINE_GUID


#define DEFINE_GUID(label,d1,d2,d3,...) {const char lab[]=#label; GUID v={d1,d2,d3,{__VA_ARGS__}};if(memcmp(&gu,&v,sizeof(v))==0) {printf("%s",lab);return; } ;}
void printid(GUID &gu) {

#include <diskguid.h>
}

char **defmem(int nr) {
return (char **)malloc(sizeof(char *)* nr);
}

char **getpartitions(FILEHANDLE handleuit,int &count) {
if(handleuit==INVALID_HANDLE_VALUE) {
	return NULL;
	}
int nrpart=4;
DWORD layoutsize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + nrpart * sizeof(PARTITION_INFORMATION_EX); 
DRIVE_LAYOUT_INFORMATION_EX *disk_layout = (DRIVE_LAYOUT_INFORMATION_EX*) new char[layoutsize];
DWORD stored;

while(!DeviceIoControl( handleuit,               // handle to device
                      IOCTL_DISK_GET_DRIVE_LAYOUT_EX, // dwIoControlCode
                      NULL,                           // lpInBuffer
                      0,                              // nInBufferSize
                      disk_layout,           // output buffer
                      layoutsize,         // size of output buffer
                      &stored,      // number of bytes returned
                      NULL))  {
	DWORD er=GetLastError();
	if( er== ERROR_INSUFFICIENT_BUFFER) {
		delete disk_layout;
		layoutsize*=2;
		disk_layout = (DRIVE_LAYOUT_INFORMATION_EX*) new char[layoutsize];
		}
	else {
 		CloseHandle(handleuit) ;
		return -1;
		}
	}

count=disk_layout->PartitionCount;

char **devices=defmem(count);
/*
switch( disk_layout->PartitionStyle) {
  case PARTITION_STYLE_MBR: 
	cout<<"MBR, signature "<< disk_layout->Mbr.Signature<<"\n";break;
  case PARTITION_STYLE_GPT: cout<<"GPT"<<endl;break;
	printf("DiskId ");printguid( disk_layout->Gpt.DiskId);
	printf("\nStartingUsableOffset %ld",disk_layout->Gpt.StartingUsableOffset.QuadPart);
	printf("\nUsableLength %ld",disk_layout->Gpt.UsableLength.QuadPart);
	printf("\nMax %ul", disk_layout->Gpt.MaxPartitionCount);
	break;
  case PARTITION_STYLE_RAW: cout<<"RAW"<<endl;break;
  default: cout<<"Error style"<<endl;break;
	};
*/
if( disk_layout->PartitionStyle!=PARTITION_STYLE_RAW) {
	int parts=0;
	for(int it=0;it<count;it++) {
		PARTITION_INFORMATION_EX *pinfo=&disk_layout->PartitionEntry[it];
if( pinfo->PartitionLength.QuadPart>0L) {
		if( disk_layout->PartitionStyle!=pinfo->PartitionStyle)
			cerr<<"PartitionStyle different\n";
		cout<<"Partition "<<it<<"("<<pinfo->PartitionNumber<<")";
		cout << " StartingOffset "<<pinfo->StartingOffset.QuadPart;
		cout << " PartitionLength "<<pinfo->PartitionLength.QuadPart;


switch( disk_layout->PartitionStyle) {
  case PARTITION_STYLE_MBR: 
	cout << " boot "<<(pinfo->Mbr.BootIndicator?"Yes":"No");
	cout<<" Recognized "<< (pinfo->Mbr.RecognizedPartition?"Yes":"No")<<' ';
	cout<<" HiddenSectors "<< pinfo->Mbr.HiddenSectors<<endl;
	if( !IsContainerPartition( pinfo->Mbr.PartitionType)&&pinfo->Mbr.PartitionType!=PARTITION_ENTRY_UNUSED)
		parts++;
char *label;
	; break;

  case PARTITION_STYLE_GPT: cout<<"GPT"<<endl;

	cout<<"GPT "<<"Name: ";wcout<<pinfo->Gpt.Name;
	cout<<" Type "; 	printid(pinfo->Gpt.PartitionType);printf(" "); printguid(pinfo->Gpt.PartitionType);

	cout<<"PartitionId ";printguid(pinfo->Gpt.PartitionId);
	if(pinfo->PartitionLength.QuadPart!=0L)
		parts++;
	break;
	default: printf("Error \n");
	};
	}
}
printf("Used partitions %d\n",parts);


}
delete disk_layout;
}
