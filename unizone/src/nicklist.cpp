#include "nicklist.h"

WNickListItem::WNickListItem(	
				   QListView * parent,
				   QString a, QString b, QString c, QString d,
				   QString e, QString f, QString g, QString h 
				   )
				   : WUniListItem(parent, a, b, c, d, e, f, g, h) 
{
	setColumnType(Name, String_NoCase_Stripped);
	setColumnType(ID, Number);
	setColumnType(Status, String_NoCase_Stripped);
	setColumnType(Files, Number);
	setColumnType(Connection, ConnectionSpeed);
	setColumnType(Load, TransferLoad);
	setColumnType(Client, String_NoCase);
}

void
WNickListItem::paintCell(QPainter * p, const QColorGroup & cg, int column, int w, int alignment)
{
	QListViewItem::paintCell(p, cg, column, w, alignment);
}
