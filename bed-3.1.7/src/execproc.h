class Proclist {
	public:
	int (Editor::**initproc)(void);
	int initprocnr;
	 int initprociter;
	Proclist(void) ;
	~Proclist() ;
	int addprocedure(int (Editor::*proc)(void)) ;
	int			straddprocedure(char *name) ;
	int executeinitproc(Editor *ed) ;
	};
extern Proclist *execopen,*execones,*execswitch;

extern  int addprocedure(int (Editor::*proc)(void)) ;
extern int call_open(int (Editor::*proc)(void)) ;
extern int str_call_open(char *name) ;
extern int call_once(int (Editor::*proc)(void)) ;
extern int str_call_once(char *name) ;
extern int call_switch(int (Editor::*proc)(void)) ;
extern int str_call_switch(char *name) ;
#define initproclist(name) {if(!name) name=new Proclist;}
