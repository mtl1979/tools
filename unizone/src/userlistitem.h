// Universal List View class (C) 2002 FieldNet Association / Team UniShare
// Released under Lesser GPL as in LGPL.TXT in source root folder

#ifndef USERLISTITEM_H
#define USERLISTITEM_H

#ifdef WIN32
#pragma warning(disable: 4786)
#endif

#include "ulistview.h"
#include "nicklist.h"

#include <qlistview.h>

class WUserListItem : public WNickListItem
{
public:

	WUserListItem(QListView * parent) 
		: WNickListItem(parent) 
	{
		// empty
	}
	
	WUserListItem(QListViewItem * parent) 
		: WNickListItem(parent) 
	{
		// empty
	}
	
	WUserListItem(QListView * parent, QListViewItem * after) 
		: WNickListItem(parent, after) 
	{
		// empty
	}
	
	WUserListItem(QListViewItem * parent, QListViewItem * after)
		: WNickListItem(parent, after) 
	{
		// empty
	}
	
	WUserListItem(QListView * parent, QString a, QString b = QString::null,
				QString c = QString::null, QString d = QString::null,
				QString e = QString::null, QString f = QString::null,
				QString g = QString::null, QString h = QString::null );

	// if more constructors are needed, they will be added later

	virtual void paintCell(QPainter *, const QColorGroup & cg, int column, int w,
							int alignment);
	

	void SetFirewalled(bool b) 
	{ 
		fFire = b; 
	}
	
	bool Firewalled() 
	{ 
		return fFire; 
	}

private:
	bool fFire;
	QColorGroup _cg;
};

#endif
