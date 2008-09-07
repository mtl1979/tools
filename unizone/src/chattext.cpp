#include <q3dragobject.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QKeyEvent>

#include "chattext.h"
#include "textevent.h"
#include "debugimpl.h"
#include "global.h"
#include "util.h"
#include "winsharewindow.h"


#define DEFAULT_SIZE 10
#define MAX_SIZE 100

WChatText::WChatText(QObject * target, QWidget * parent)
	: Q3MultiLineEdit(parent), fTarget(target) 
{
	fBuffer = new Queue<QString>(DEFAULT_SIZE);
	fCurLine = 0;
	setAcceptDrops(true);
}

WChatText::~WChatText()
{
	delete fBuffer;
}

void
WChatText::keyPressEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)    
		// check for BOTH (they ARE different to Qt)
	{
		if ((event->state() & Qt::ShiftButton))
		{
			newLine();	// new line if SHIFT+ENTER is pressed
		}
		else
		{
			// otherwise, send text
			WTextEvent *wte = new WTextEvent(text());
			if (wte)
			{
				if (!wte->Valid())
				{
					delete wte;
					wte = NULL;
					return;
				}
				QApplication::postEvent(fTarget, wte);			// <postmaster@raasu.org> 20021024 -- Fix Access violation due duplicate definition with different type
			}
			QString line;
			fBuffer->GetItemAt(fCurLine, line);
			
			// don't add duplicate items
			if (text() != line)
			{
				AddLine(text());
				PRINT("Lines %d\n", fCurLine);
			}
			fCurLine = fBuffer->GetNumItems();	// reset
			setText(QString::null);
		}
	}
	else if (event->key() == Qt::Key_Tab)
	{
		PRINT("Emitting TAB\n");
		// tab completion
		emit TabPressed(text());
	}	
	else if (event->key() == Qt::Key_Up)
	{
		if (event->state() & Qt::AltButton)
		{
			Q3MultiLineEdit::keyPressEvent(event);
		}
		else if (event->state() & Qt::ControlButton)				
		{
			// First line
			if (fCurLine > 0)
			{
				fCurLine = 0;
				QString line;
				fBuffer->GetItemAt(fCurLine, line);
				setText(line);
				gotoEnd();
			}
		}
		else
		{
			// Previous line
			if (fCurLine > 0)
			{
				fCurLine--;
				QString line;
				fBuffer->GetItemAt(fCurLine, line);
				setText(line);
				gotoEnd();
			}
		}
	}
	else if (event->key() == Qt::Key_Down)
	{
		if (event->state() & Qt::AltButton)
		{
			Q3MultiLineEdit::keyPressEvent(event);
		}
		else if (event->state() & Qt::ControlButton)					
		{
			// Last line
			if (fCurLine < fBuffer->GetNumItems() - 2)
			{
				fCurLine = fBuffer->GetNumItems() - 1;
				QString line;
				fBuffer->GetItemAt(fCurLine, line);
				setText(line);
				gotoEnd();
			}
			else
			{
				fCurLine = fBuffer->GetNumItems();
				setText(QString::null);
			}
		}
		else
		{
			// Next line
			if (fCurLine < fBuffer->GetNumItems() - 1)
			{
				fCurLine++;
				QString line;
				fBuffer->GetItemAt(fCurLine, line);
				setText(line);
				gotoEnd();
			}
			else
			{
				fCurLine = fBuffer->GetNumItems();
				setText(QString::null);
			}
		}
	}
	else if (event->key() == Qt::Key_Escape)
	{
		setText(QString::null);
		fCurLine = fBuffer->GetNumItems();
	}
	else
		Q3MultiLineEdit::keyPressEvent(event);
}

void
WChatText::ClearBuffer()
{
	fLock.Lock();
	fBuffer->Clear();
	fCurLine = 0;	// no more items
	fLock.Unlock();
}

void
WChatText::gotoEnd()
{
	int lastline = numLines() - 1;
	int len = lineLength(lastline);
	setCursorPosition(lastline, len);
}

void
WChatText::AddLine(const QString &line)
{
	QString junk;
	unsigned int l = 0;
	fLock.Lock();
	// Remove duplicate entries
	while (l < fBuffer->GetNumItems())
	{
		fBuffer->GetItemAt(l, junk);
		if (junk == line)
		{
			fBuffer->RemoveItemAt(l);
			if (l <= fCurLine)
				fCurLine--;
		}
		else
		{
			l++;
		}

		if (l >= fBuffer->GetNumItems())
			break;
	}
	// Remove enough old entries, so total amount after adding this line will not exceed MAX_SIZE
	while (fBuffer->GetNumItems() >= MAX_SIZE)
	{
		fBuffer->RemoveHead(junk);
		if (fCurLine > 0) 
			fCurLine--;
	}
	fBuffer->AddTail(line);
	fLock.Unlock();
}

void
WChatText::dropEvent(QDropEvent* event)
{
    QStringList list;
	QString files;
	QString uid = gWin->GetUserID();

	printf("in dropEvent()\n");
	if (!uid.isEmpty())
	{	
		if ( Q3UriDrag::decodeLocalFiles(event, list) ) 
		{
			QStringList::Iterator iter = list.begin();
			while (iter != list.end())
			{
				QString filename = *iter;
				if (gWin->FindSharedFile(filename))
					AddToList(files, filename);
				iter++;
			}
			if (!files.isEmpty())
			{
				files.prepend("beshare:");
				files.append("@");
				files.append(uid);
				insert(files);
			}
			return;
		}
	}
	Q3MultiLineEdit::dropEvent(event);
}

void 
WChatText::dragEnterEvent(QDragEnterEvent* event)
{
    event->accept( Q3UriDrag::canDecode(event) || Q3TextDrag::canDecode(event));
}

