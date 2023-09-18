#include <iostream>
#include <pdcurses.h>


using namespace std;
int main() {


cout<<A_BOLD<<endl;
#undef A_BOLD 
#define A_BOLD        (chtype)0x00800000
cout<<A_BOLD<<endl;
}
