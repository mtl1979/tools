#include "menubar.h"
#include "version.h"

#include <qapplication.h>
#include <q3accel.h>
//Added by qt3to4:
#include <Q3PopupMenu>

const char *appleItems[] = {
	QT_TRANSLATE_NOOP("QMenuBar", "Preference"),
        QT_TRANSLATE_NOOP("QMenuBar", "About"),
        NULL
};

MenuBar::MenuBar(QWidget * handler, QWidget * parent) : QMenuBar(parent)
{
	// create search... menu

	Q3PopupMenu *fSearch = new Q3PopupMenu(this);
	Q_CHECK_PTR(fSearch);

	fSearch->insertItem(tr("Music"), handler, SLOT(SearchMusic()));
	fSearch->insertItem(tr("Videos"), handler, SLOT(SearchVideos()));
	fSearch->insertItem(tr("Pictures"), handler, SLOT(SearchPictures()));
	fSearch->insertItem(tr("Disk Images"), handler, SLOT(SearchImages()));

	// create file menu

	fFile = new Q3PopupMenu(this);
	Q_CHECK_PTR(fFile);

	fFile->insertItem(tr("&Connect"), handler, SLOT(Connect()), Q3Accel::stringToKey(tr("CTRL+SHIFT+C")));
	fFile->insertItem(tr("&Disconnect"), handler, SLOT(Disconnect()), Q3Accel::stringToKey(tr("CTRL+SHIFT+D")));
	fFile->insertSeparator();
	fFile->insertItem(tr("Open &Shared Folder"), handler, SLOT(OpenSharedFolder()), Q3Accel::stringToKey(tr("CTRL+S")));
	fFile->insertItem(tr("Open &Downloads Folder"), handler, SLOT(OpenDownloadsFolder()), Q3Accel::stringToKey(tr("CTRL+D")));
	fFile->insertItem(tr("Open &Logs Folder"), handler, SLOT(OpenLogsFolder()), Q3Accel::stringToKey(tr("CTRL+L")));
	fFile->insertSeparator();
	fFile->insertItem(tr("&Search"), handler, SLOT(OpenSearch()), Q3Accel::stringToKey(tr("ALT+S")));
	fFile->insertItem(tr("Search..."), fSearch);
	fFile->insertSeparator();
	fFile->insertItem(tr("E&xit"), handler, SLOT(Exit()), Q3Accel::stringToKey(tr("ALT+X")));

	// edit menu
	fEdit = new Q3PopupMenu(this);
	Q_CHECK_PTR(fEdit);

	fEdit->insertItem(tr("Cl&ear Chat Log"), handler, SLOT(ClearChatLog()), Q3Accel::stringToKey(tr("CTRL+E")));
	fEdit->insertSeparator();
	fEdit->insertItem(tr("&Preferences"), handler, SLOT(Preferences()), Q3Accel::stringToKey(tr("CTRL+P")));

	// windows menu

	fWindows = new Q3PopupMenu(this);
	Q_CHECK_PTR(fWindows);

	fWindows->insertItem(tr("Picture Viewer"), handler, SLOT(OpenViewer()), Q3Accel::stringToKey(tr("F9")));
	fWindows->insertItem(tr("C&hannels"), handler, SLOT(OpenChannels()), Q3Accel::stringToKey(tr("F10")));
	fWindows->insertItem(tr("&Downloads"), handler, SLOT(OpenDownloads()), Q3Accel::stringToKey(tr("F11")));
	fWindows->insertItem(tr("&Uploads"), handler, SLOT(OpenUploads()), Q3Accel::stringToKey(tr("Shift+F11")));
	// help menu
	fHelp = new Q3PopupMenu(this);
	Q_CHECK_PTR(fHelp);

	QString about = tr( "&About Unizone (English) %1" ).arg(WinShareVersionString());

	fHelp->insertItem(about, handler, SLOT(AboutWinShare()), Q3Accel::stringToKey(tr("F12")));

	/* Insert into menubar */
	insertItem(tr("&File"), fFile);
	insertItem(tr("&Edit"), fEdit);
	insertItem(tr("&Window"), fWindows);
	insertItem(tr("&Help"), fHelp);
}

MenuBar::~MenuBar()
{
}

