#ifndef __SHOWTREE_H__
#define __SHOWTREE_H__

#include <qwindow.h>
#include <qpainter.h>
#include <qprinter.h>
#include <qpushbt.h>
#include <qradiobt.h>
#include <qbttngrp.h>
#include <qapp.h>
#include <math.h>
#include <qscrbar.h> 
#include <qtablevw.h>
#include <qlined.h> 
#include <stdlib.h>
#include <qlabel.h> 
#include <qmlined.h> 
class Tree;

#include "rbtree.h"

class DrawTree : public QWidget
{
    Q_OBJECT
public:
    	DrawTree(Tree *);
	void drawnode( int x, int y, char *name,int color  ); 
	void drawline(int x, int y, int xchild);
protected:

	void printIt();
void updateIt( int index );
    void   drawIt( QPainter * );
    void   paintEvent( QPaintEvent * );
    void   resizeEvent( QResizeEvent * );
private:
    Tree *tree;
    QFont *TextFont;
    QPainter *paint;
    QWidget *balk;
    QLabel *addLabel;
    QLabel *delLabel;
    QLabel *searchLabel;

    QLabel *message;
    QLineEdit *del;
    QLineEdit *add;
    QLineEdit *search;
    int xstop,ystop,xoff,yoff,nodew,nodeh;
public slots:
	void remove();
	void searchfor() ;
	void addel() ;
};


#endif /*__SHOWTREE_H__*/
