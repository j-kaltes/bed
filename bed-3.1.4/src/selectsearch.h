#ifndef SELECTSEARCH_H
#define SELECTSEARCH_H
//#include "editor.h"
#include "searchaim.h"

typedef OFFTYPE (Editor::* SearchInit)(SearchAim &);
typedef OFFTYPE (Editor::* SearchNext)(SearchAim &);
//typedef SearchType::SelectProcT SelectProcT;
typedef struct {
	SearchInit init;
	SearchNext next;
} searchert;
/*
typedef struct {
	SelectProcT  type;
	searchert ser;
} SearcherT ;
*/
#define TNone RegType::None

#define GNU RegType::RegGNU

//#define ProcSelecter struct {uint8_t  Forward=true; RegexT Regex=RegGNU;uint8_t Raw,Case=true, Align=false, Unit=true; }

#endif
