
#include <windows.h>
#include <iostream>
using namespace std;
int showgeo(HANDLE handle)  {

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	 GetConsoleScreenBufferInfo(handle, &csbi);
	int scrHigh= csbi.srWindow.Bottom-csbi.srWindow.Top +1;
	int scrWidth=csbi.srWindow.Right- csbi.srWindow.Left+1;
	cout<<"width="<<scrWidth<<" height="<<scrHigh<<endl;
	}
int main()  {
auto handle=GetStdHandle(STD_OUTPUT_HANDLE);
showgeo(handle);
char ch;
cin>>ch;
showgeo(handle);

}
