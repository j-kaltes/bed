#include <type_traits>
#include <iostream>
using namespace std;
/*
char *proc() {
return nullptr;
}
*/
int proc() {
return 5 ;
}
int main() {
auto get=proc();
if constexpr ( std::is_same<typename std::decay<decltype(get)>::type, char *>::value)
cout<<"same"<<endl;
else
cout<<"different"<<endl;
}


