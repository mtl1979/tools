#ifdef WIN32
#pragma warning(disable: 4786)
#endif

#include <qthread.h>
#include <qobject.h>
#include <qqueue.h>
#include <qdir.h>
#include <time.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <list>
using std::list;
using std::iterator;

#include "message/Message.h"
#include "downloadimpl.h"
#include "md5.h"
#include "debugimpl.h"
#include "global.h"
#include "filethread.h"
#include "settings.h"
#include "downloadthread.h"
#include "uploadthread.h"
#include "wdownloadevent.h"
#include "wuploadevent.h"
#include "iogateway/MessageIOGateway.h"
#include "reflector/RateLimitSessionIOPolicy.h"
#include "wstring.h"
#include "transferitem.h"
#include "gotourl.h"
// #include "platform.h"
#include "util.h"
#include "netclient.h"

// ----------------------------------------------------------------------------------------------

WDownload::WDownload(QWidget * parent, QString localID, WFileThread * ft)
: QDialog(parent, "WDownload", false, /* QWidget::WDestructiveClose |*/ QWidget::WStyle_Minimize |
		  QWidget::WStyle_Maximize | QWidget::WStyle_Title | QWidget::WStyle_SysMenu),
		  fLock(true)
{
	resize(450, 265); // <postmaster@raasu.org> 20020927 Changed from 250 to 265
	fSharedFiles = ft;
	fLocalSID = localID;
	
	fMainSplit = new QSplitter(this);
	CHECK_PTR(fMainSplit);
	fMainSplit->setOrientation(QSplitter::Vertical);
	
	fBoxD = new QVBox(fMainSplit);
	CHECK_PTR(fBoxD);
	fDownloads = new QListView(fBoxD);
	CHECK_PTR(fDownloads);
	fDownloads->addColumn(tr("Status"));
	fDownloads->addColumn(tr("Filename"));
	fDownloads->addColumn(tr("Received"));
	fDownloads->addColumn(tr("Total"));
	fDownloads->addColumn(tr("Rate"));
	fDownloads->addColumn(tr("ETA"));
	fDownloads->addColumn(tr("Elapsed"));
	fDownloads->addColumn(tr("User"));
	fDownloads->addColumn(tr("Index"));
	fDownloads->addColumn(tr("QR"));
	
	fDownloads->setColumnAlignment(WTransferItem::Received, AlignRight);	// <postmaster@raasu.org> 20021213
	fDownloads->setColumnAlignment(WTransferItem::Total, AlignRight);		// 
	fDownloads->setColumnAlignment(WTransferItem::Rate, AlignRight);		// 
	fDownloads->setColumnAlignment(WTransferItem::ETA, AlignRight);			// 
	fDownloads->setColumnAlignment(WTransferItem::Elapsed, AlignRight);		// 20030729
	fDownloads->setColumnAlignment(WTransferItem::QR, AlignRight);			// 20030310
	
	fDownloads->setAllColumnsShowFocus(true);
	
	fBoxU = new QVBox(fMainSplit);
	CHECK_PTR(fBoxU);
	
	fUploads = new QListView(fBoxU);
	CHECK_PTR(fUploads);
	fUploads->addColumn(tr("Status"));
	fUploads->addColumn(tr("Filename"));
	fUploads->addColumn(tr("Sent"));
	fUploads->addColumn(tr("Total"));
	fUploads->addColumn(tr("Rate"));
	fUploads->addColumn(tr("ETA"));
	fUploads->addColumn(tr("Elapsed"));
	fUploads->addColumn(tr("User"));
	fUploads->addColumn(tr("Index"));
	fUploads->addColumn(tr("QR"));
	
	fUploads->setColumnAlignment(WTransferItem::Received, AlignRight);	// <postmaster@raasu.org> 20021213
	fUploads->setColumnAlignment(WTransferItem::Total, AlignRight);		// 
	fUploads->setColumnAlignment(WTransferItem::Rate, AlignRight);		// 
	fUploads->setColumnAlignment(WTransferItem::ETA, AlignRight);		//
	fUploads->setColumnAlignment(WTransferItem::Elapsed, AlignRight);	// 20030729
	fUploads->setColumnAlignment(WTransferItem::QR, AlignRight);		// 20030310
	
	fUploads->setAllColumnsShowFocus(true);
	
	connect(gWin->fNetClient, SIGNAL(UserDisconnected(const QString &, const QString &)), 
			this, SLOT(UserDisconnected(const QString &, const QString &)));
	
	
	setCaption(tr("File Transfers"));
	
	fDLPopup = new QPopupMenu(this, "Download Popup");
	CHECK_PTR(fDLPopup);
	
	// Create the download popup menu
	fDLPopup->insertItem(tr("Queue"), ID_QUEUE);
	
	// Create throttle sub menu
	fDLThrottleMenu = new QPopupMenu(fDLPopup, "Throttle Popup");
	CHECK_PTR(fDLThrottleMenu);
	
	fDLThrottleMenu->insertItem(tr( "No Limit" ), ID_NO_LIMIT);
	
	// Set initial throttle to none
	fDLThrottleMenu->setItemChecked(ID_NO_LIMIT, true);
	fDLThrottle = ID_NO_LIMIT;
	
	fDLThrottleMenu->insertItem(tr( "64 bytes" ), ID_64);
	fDLThrottleMenu->insertItem(tr( "128 bytes" ), ID_128);
	fDLThrottleMenu->insertItem(tr( "256 bytes" ), ID_256);
	fDLThrottleMenu->insertItem(tr( "512 bytes" ), ID_512);
	fDLThrottleMenu->insertItem(tr( "1 kB" ), ID_1KB);
	fDLThrottleMenu->insertItem(tr( "2 kB" ), ID_2KB);
	fDLThrottleMenu->insertItem(tr( "4 kB" ), ID_4KB);
	fDLThrottleMenu->insertItem(tr( "6 kB" ), ID_6KB);
	fDLThrottleMenu->insertItem(tr( "8 kB" ), ID_8KB);
	fDLThrottleMenu->insertItem(tr( "10 kB" ), ID_10KB);
	fDLThrottleMenu->insertItem(tr( "12 kB" ), ID_12KB);
	fDLThrottleMenu->insertItem(tr( "14 kB" ), ID_14KB);
	fDLThrottleMenu->insertItem(tr( "16 kB" ), ID_16KB);
	fDLThrottleMenu->insertItem(tr( "32 kB" ), ID_32KB);
	fDLThrottleMenu->insertItem(tr( "64 kB" ), ID_64KB);
	fDLThrottleMenu->insertItem(tr( "128 kB" ), ID_128KB);
	fDLThrottleMenu->insertItem(tr( "256 kB" ), ID_256KB);
	fDLThrottleMenu->insertItem(tr( "512 kB" ), ID_512KB);
	fDLThrottleMenu->insertItem(tr( "1 MB" ), ID_1MB);
	fDLThrottleMenu->insertItem(tr( "2 MB" ), ID_2MB);
	fDLThrottleMenu->insertItem(tr( "4 MB" ), ID_4MB);
	fDLThrottleMenu->insertItem(tr( "8 MB" ), ID_8MB);
	fDLThrottleMenu->insertItem(tr( "16 MB" ), ID_16MB);
	fDLThrottleMenu->insertItem(tr( "32 MB" ), ID_32MB);
	
	fDLRunMenu = new QPopupMenu(fDLPopup, "Run Popup");
	CHECK_PTR(fDLRunMenu);
	
	fDLPopup->insertItem(tr("Throttle"), fDLThrottleMenu, ID_THROTTLE);
	fDLPopup->insertItem(tr("Run..."), fDLRunMenu, ID_RUN);
	
	fDLPopup->insertItem(tr("Move Up"), ID_MOVEUP);
	fDLPopup->insertItem(tr("Move Down"), ID_MOVEDOWN);
	
	fDLPopup->insertItem(tr("Clear Finished"), ID_CLEAR);
	fDLPopup->insertItem(tr("Cancel"), ID_CANCEL);
	
	connect(fDLPopup, SIGNAL(activated(int)), this, SLOT(DLPopupActivated(int)));
	connect(fDLThrottleMenu, SIGNAL(activated(int)), this, SLOT(DLPopupActivated(int)));
	connect(fDLRunMenu, SIGNAL(activated(int)), this, SLOT(DLPopupActivated(int)));
	connect(fDownloads, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(DLRightClicked(QListViewItem *, const QPoint &, int)));

	fDLPopupItem = NULL;
	
	fULPopup = new QPopupMenu(this, "Upload Popup");
	CHECK_PTR(fULPopup);
	
	// Create the upload popup menu
	fULPopup->insertItem(tr("Queue"), ID_QUEUE);
	
	// Create throttle sub menu
	fULThrottleMenu = new QPopupMenu(fULPopup, "Throttle Popup");
	CHECK_PTR(fULThrottleMenu);
	
	fULThrottleMenu->insertItem(tr( "No Limit" ), ID_NO_LIMIT);
	
	// Set initial throttle to none
	fULThrottleMenu->setItemChecked(ID_NO_LIMIT, true);
	fULThrottle = ID_NO_LIMIT;
	
	fULThrottleMenu->insertItem(tr( "64 bytes" ), ID_64);
	fULThrottleMenu->insertItem(tr( "128 bytes" ), ID_128);
	fULThrottleMenu->insertItem(tr( "256 bytes" ), ID_256);
	fULThrottleMenu->insertItem(tr( "512 bytes" ), ID_512);
	fULThrottleMenu->insertItem(tr( "1 kB" ), ID_1KB);
	fULThrottleMenu->insertItem(tr( "2 kB" ), ID_2KB);
	fULThrottleMenu->insertItem(tr( "4 kB" ), ID_4KB);
	fULThrottleMenu->insertItem(tr( "6 kB" ), ID_6KB);
	fULThrottleMenu->insertItem(tr( "8 kB" ), ID_8KB);
	fULThrottleMenu->insertItem(tr( "10 kB" ), ID_10KB);
	fULThrottleMenu->insertItem(tr( "12 kB" ), ID_12KB);
	fULThrottleMenu->insertItem(tr( "14 kB" ), ID_14KB);
	fULThrottleMenu->insertItem(tr( "16 kB" ), ID_16KB);
	fULThrottleMenu->insertItem(tr( "32 kB" ), ID_32KB);
	fULThrottleMenu->insertItem(tr( "64 kB" ), ID_64KB);
	fULThrottleMenu->insertItem(tr( "128 kB" ), ID_128KB);
	fULThrottleMenu->insertItem(tr( "256 kB" ), ID_256KB);
	fULThrottleMenu->insertItem(tr( "512 kB" ), ID_512KB);
	fULThrottleMenu->insertItem(tr( "1 MB" ), ID_1MB);
	fULThrottleMenu->insertItem(tr( "2 MB" ), ID_2MB);
	fULThrottleMenu->insertItem(tr( "4 MB" ), ID_4MB);
	fULThrottleMenu->insertItem(tr( "8 MB" ), ID_8MB);
	fULThrottleMenu->insertItem(tr( "16 MB" ), ID_16MB);
	fULThrottleMenu->insertItem(tr( "32 MB" ), ID_32MB);
	
	fULBanMenu = new QPopupMenu(fULPopup, "Ban Popup");
	CHECK_PTR(fULBanMenu);
	
	fULBanMenu->insertItem(tr("Unbanned"), ID_UNBAN);
	fULBanMenu->setItemChecked(ID_UNBAN, true);
	fULBan = ID_UNBAN;
	
	fULBanMenu->insertItem(tr("1 minute"), ID_BAN1);
	fULBanMenu->insertItem(tr("2 minutes"), ID_BAN2);
	fULBanMenu->insertItem(tr("5 minutes"), ID_BAN5);
	fULBanMenu->insertItem(tr("10 minutes"), ID_BAN10);
	fULBanMenu->insertItem(tr("15 minutes"), ID_BAN15);
	fULBanMenu->insertItem(tr("30 minutes"), ID_BAN30);
	fULBanMenu->insertItem(tr("1 hour"), ID_BAN1H);
	fULBanMenu->insertItem(tr("Infinite"), ID_BANINF);
	
	fULPacketMenu = new QPopupMenu(fULPopup, "Packet Size Menu");
	CHECK_PTR(fULPacketMenu);
	
	fULPacketMenu->setItemChecked(ID_PACKET8K, true);
	fULPacket = ID_PACKET8K;
	
	fULPacketMenu->insertItem(tr("1 kB"), ID_PACKET1K);
	fULPacketMenu->insertItem(tr("2 kB"), ID_PACKET2K);
	fULPacketMenu->insertItem(tr("4 kB"), ID_PACKET4K);
	fULPacketMenu->insertItem(tr("8 kB"), ID_PACKET8K);
	fULPacketMenu->insertItem(tr("16 kB"), ID_PACKET16K);
	fULPacketMenu->insertItem(tr("32 kB"), ID_PACKET32K);
	
	fULPopup->insertItem(tr("Move Up"), ID_MOVEUP);
	fULPopup->insertItem(tr("Move Down"), ID_MOVEDOWN);
	fULPopup->insertItem(tr("Ban IP"), ID_IGNORE);
	fULPopup->insertItem(tr("Clear Finished"), ID_CLEAR);
	fULPopup->insertItem(tr("Cancel"), ID_CANCEL);
	
	fULPopup->insertItem(tr("Throttle"), fULThrottleMenu, ID_THROTTLE);
	fULPopup->insertItem(tr("Block"), fULBanMenu, ID_BLOCK);
	fULPopup->insertItem(tr("Packet Size"), fULPacketMenu, ID_SETPACKET);
	
	connect(fULPopup, SIGNAL(activated(int)), this, SLOT(ULPopupActivated(int)));
	connect(fULThrottleMenu, SIGNAL(activated(int)), this, SLOT(ULPopupActivated(int)));
	connect(fULBanMenu, SIGNAL(activated(int)), this, SLOT(ULPopupActivated(int)));
	connect(fULPacketMenu, SIGNAL(activated(int)), this, SLOT(ULPopupActivated(int)));
	connect(fUploads, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(ULRightClicked(QListViewItem *, const QPoint &, int)));
	
	fULPopupItem = NULL;
}

WDownload::~WDownload()
{
	EmptyLists();
	emit Closed();
}

void
WDownload::EmptyLists()
{
	EmptyDownloads();
	EmptyUploads();
}

void
WDownload::EmptyDownloads()
{
	Lock();
	PRINT("Number of downloads: %d\n", fDownloadList.GetNumItems());
	DLPair pair;
	while (fDownloadList.RemoveHead(pair) == B_NO_ERROR)
	{
		// Put all files in resume list
		if (pair.first)
		{
			int n = pair.first->GetCurrentNum();
			
			if (n > -1) 
			{
				for (int i = n; i < pair.first->GetNumFiles(); i++)
				{
					emit FileInterrupted(pair.first->GetFileName(i), pair.first->GetLocalFileName(i), pair.first->GetRemoteUser());
				}
			}
			PRINT("Reseting download\n");
			pair.first->Reset();
			PRINT("Deleting download\n");
			delete pair.first;
		}
		if (pair.second)
		{
			PRINT("Deleting item\n");
			delete pair.second;
		}
	}
	Unlock();
}

void
WDownload::EmptyUploads()
{
	Lock();
	PRINT("Number of uploads: %d\n", fUploadList.GetNumItems());
	ULPair pair;
	while (fUploadList.RemoveHead(pair) == B_NO_ERROR)
	{
		if (pair.first)
		{
			PRINT("Reseting upload\n");
			pair.first->Reset();
			PRINT("Deleting upload\n");
			delete pair.first;
		}
		if (pair.second)
		{
			PRINT("Deleting item\n");
			delete pair.second;
		}
	}
	Unlock();
	// do this to set a load of 0
	SendSignal(UpLoad);
}

void
WDownload::AddDownload(QString * files, QString * lfiles, 
			int32 filecount, QString remoteSessionID, 
			uint32 remotePort, const QString & remoteIP, 
			uint64 /* remoteInstallID */, bool firewalled, 
			bool partial)
{
	WDownloadThread * nt = new WDownloadThread(this);
	CHECK_PTR(nt);

	QString ip = remoteIP;
	if (!firewalled) // Check for valid ip and port if not firewalled
	{
		int x;
		
		// Can't use localhost to download!!!
		
		if (remoteIP == "127.0.0.1")	
		{
			ip = gWin->fNetClient->GetServerIP();
			if (ip != "127.0.0.1")
			{
				for (x = 0; x < filecount; x++)
					gWin->PrintWarning(tr("Invalid address! Download address for file %1 replaced with %2, it might fail!").arg(files[x]).arg(remoteIP));
			}
		}
		
		// Detect uncommon remote port 
		
		if (!muscleInRange(remotePort, (uint32) 1, (uint32) 65535))
		{
			for (x = 0; x < filecount; x++)
				gWin->PrintWarning(tr("Download port for file %1 might be out of range, it might fail!").arg(files[x]));
		}
	}

	nt->SetFile(files, lfiles, filecount, ip, remoteSessionID, fLocalSID, remotePort, firewalled, partial);
	
	DLPair p;
	p.first = nt;
	p.second = new WTransferItem(fDownloads, "", "", "", "", "", "", "", "", "", "");
	CHECK_PTR(p.second);
		
	if (GetNumDownloads() < gWin->fSettings->GetMaxDownloads())
	{
		nt->InitSession();
		nt->SetLocallyQueued(false);
	}
	else
	{
		nt->SetLocallyQueued(true);
		p.second->setText(WTransferItem::Status, tr("Locally Queued."));
	}
	Lock();
	fDownloadList.AddTail(p);
	Unlock();
	SendSignal(DLRatings);
}

void
WDownload::AddDownloadList(Queue<QString> & fQueue, Queue<QString> & fLQueue, const WUserRef & user)
{
	int32 nFiles = fQueue.GetNumItems();
	QString * qFiles = new QString[nFiles];
	CHECK_PTR(qFiles);
	
	QString * qLFiles = new QString[nFiles];
	QString tmp, tmp2;
	int n = 0;
	while ((fQueue.RemoveHead(tmp) == B_NO_ERROR) && (fLQueue.RemoveHead(tmp2) == B_NO_ERROR))
	{
		// Remote name
		if (!tmp.isEmpty()) 
		{
			qFiles[n] = tmp;
		}
		else
		{
			qFiles[n] = QString::null;
		}
		
		// Local name
		if (!tmp2.isEmpty())
		{
			qLFiles[n] = tmp2;
		}
		else
		{
			qLFiles[n] = QString::null;
		}
		
		n++;
	}
	AddDownload(qFiles, qLFiles, n, user()->GetUserID(), user()->GetPort(), user()->GetUserHostName(), user()->GetInstallID(), user()->GetFirewalled(), user()->GetPartial());
}

void
WDownload::AddUpload(const QString & remoteIP, uint32 port)
{
	PRINT("WDownload::AddUpload(QString, uint32)\n");
	WUploadThread * ut = new WUploadThread(this);
	CHECK_PTR(ut);
	
	PRINT("Setting upload\n");
	ut->SetPacketSize(gWin->fSettings->GetPacketSize());
	ut->SetUpload(remoteIP, port, fSharedFiles);
	
	if (GetNumUploads() < gWin->fSettings->GetMaxUploads())
	{
		PRINT("Not queued\n");
		ut->SetLocallyQueued(false);
	}
	else
	{
		PRINT("Queued\n");
		ut->SetLocallyQueued(true);
	}
	
	PRINT("Init session\n");
	ut->InitSession();
	ULPair p;
	p.first = ut;
	p.second = new WTransferItem(fUploads, "", "", "", "", "", "", "", "", "", "");
	CHECK_PTR(p.second);
	
	if (ut->IsLocallyQueued())
	{
		PRINT("IsQueued\n");
		p.second->setText(WTransferItem::Status, tr("Queued."));
	}
	PRINT("Inserting\n");
	Lock();
	fUploadList.AddTail(p);
	Unlock();
	SendSignal(UpLoad);
	SendSignal(ULRatings);
}

void
WDownload::AddUpload(int socket, uint32 remoteIP, bool /* queued */)
{
	PRINT("WDownload::AddUpload(int, uint32, bool)\n");
	WUploadThread * ut = new WUploadThread(this);
	CHECK_PTR(ut);
	
	PRINT("Setting upload\n");
	ut->SetPacketSize(gWin->fSettings->GetPacketSize());
	ut->SetUpload(socket, remoteIP, fSharedFiles);
	
	if (GetNumUploads() < gWin->fSettings->GetMaxUploads())
	{
		PRINT("Not queued\n");
		ut->SetLocallyQueued(false);
	}
	else
	{
		PRINT("Queued\n");
		ut->SetLocallyQueued(true);
	}
	
	PRINT("Init session\n");
	ut->InitSession();
	
	ULPair p;
	p.first = ut;
	p.second = new WTransferItem(fUploads, "", "", "", "", "", "", "", "", "", "");
	CHECK_PTR(p.second);
	
	if (ut->IsLocallyQueued())
	{
		PRINT("IsQueued\n");
		p.second->setText(WTransferItem::Status, tr("Queued."));
	}
	PRINT("Inserting\n");
	Lock();
	fUploadList.AddTail(p);
	Unlock();
	SendSignal(UpLoad);
	SendSignal(ULRatings);
}

void
WDownload::DequeueULSessions()
{
	PRINT("WDownload::DequeueULSessions\n");
	
	bool found = true;
	
	int numNotQueued = 0;
	
	
	Lock();
	for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
	{
		ULPair pair;
		fUploadList.GetItemAt(i, pair);
		if (pair.first)
		{
			if (
				(pair.first->IsLocallyQueued() == false) && 
				(pair.first->IsBlocked() == false) && 
				(pair.first->IsActive() == true) &&
				(pair.first->IsFinished() == false)
				)
				numNotQueued++;
		}
		
		if (pair.second)
		{
			pair.second->setText(WTransferItem::QR, QString::number(i));
		}
	}
	Unlock();
	
	while (numNotQueued < gWin->fSettings->GetMaxUploads())
	{
		found = false;
		Lock();
		for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
		{
			ULPair pair;
			fUploadList.GetItemAt(i, pair);
			if (pair.first)
			{
				if (
					(pair.first->IsLocallyQueued() == true) && 
					(pair.first->IsManuallyQueued() == false) &&
					(pair.first->IsFinished() == false)
					)
				{
					found = true;
					pair.first->SetLocallyQueued(false);
					numNotQueued++;
				}
			}
			if (numNotQueued == gWin->fSettings->GetMaxUploads())
				break;
		}
		Unlock();
		
		if (!found)
			break;
	}
	SendSignal(UpLoad);
	PRINT("WDownload::DequeueULSessions OK\n");
}

void
WDownload::DequeueDLSessions()
{
	PRINT("WDownload::DequeueDLSessions\n");
	
	bool found = true;
	
	/*
	** OK, this is how I do queuing for downloads. Since the download count
	** WILL be greater than the amount of downloads allowed, we can't
	** dequeue based on this, right? So, run through the download list
	** and count the amount of downloads NOT queued. If this amount is less
	** than the allowed downloads, find a queued download, and unqueue it :)
	*/
	int numNotQueued = 0;
	int qr = 0;
	
	
	Lock();
	for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
	{
		DLPair pair;
		fDownloadList.GetItemAt(i, pair);
		if (pair.first)
		{
			if (
				(pair.first->IsLocallyQueued() == false) && 
				(pair.first->IsActive() == true)
				)
			{
				// not queued, not finished?
				numNotQueued++;
			}
		}
		
		if (pair.second)
		{
			pair.second->setText(WTransferItem::QR, QString::number(qr++));
		}
		
	}
	Unlock();
	// check the queued amount, vs the amount allowed
	while (numNotQueued < gWin->fSettings->GetMaxDownloads())
	{
		found = false;
		Lock();
		for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
		{
			DLPair pair;
			fDownloadList.GetItemAt(i, pair);
			if (pair.first)
			{
				if (
					(pair.first->IsLocallyQueued() == true) && 
					(pair.first->IsManuallyQueued() == false) &&
					(pair.first->IsFinished() == false)
					)
				{
					found = true;
					pair.first->SetLocallyQueued(false);
					pair.first->InitSession();
					numNotQueued++;
				}
			}
			if (numNotQueued == gWin->fSettings->GetMaxDownloads())
				break;
		}
		Unlock();
		if (!found)
			break;
	}
	PRINT("WDownload::DequeueDLSessions OK\n");
}

void
WDownload::customEvent(QCustomEvent * e)
{
	int t = (int) e->type();
	switch (t)
	{
	case WDownloadEvent::Type:
		{
			WDownloadEvent * d = dynamic_cast<WDownloadEvent *>(e);
			if (d)
			{
				downloadEvent(d);
			}
			break;
		}
	case WUploadEvent::Type:
		{
			WUploadEvent * u = dynamic_cast<WUploadEvent *>(e);
			if (u)
			{
				uploadEvent(u);
			}
			break;
		}
	case DequeueDownloads:
		{
			DequeueDLSessions();
			break;
		}
	case DequeueUploads:
		{
			DequeueULSessions();
			break;
		}
	case ClearDownloads:
		{
			ClearFinishedDL();
			break;
		}
	case ClearUploads:
		{
			ClearFinishedUL();
			break;
		}
	case DLRatings:
		{
			UpdateDLRatings();
			break;
		}
	case ULRatings:
		{
			UpdateULRatings();
			break;
		}
	case UpLoad:
		{
			UpdateLoad();
			break;
		}
	}
}

void
WDownload::downloadEvent(WDownloadEvent * d)
{
	MessageRef msg = d->Msg();
	WDownloadThread * dt = NULL;
	WTransferItem * item = NULL;
	DLPair foundIt;
	
	if (!msg())
		return; // Invalid MessageRef!
	
	if (msg()->FindPointer("sender", (void **)&dt) != B_OK)
		return;	// failed! ouch!
	
	if (!dt)
		return;
	
	Lock();
	for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
	{
		fDownloadList.GetItemAt(i, foundIt);
		if (foundIt.first == dt)
		{
			// found our thread
			item = foundIt.second;
			PRINT("\t\tFound dl!!!\n");
			break;
		}
	}
	Unlock();
	
	if (!item)
	{
		// <postmaster@raasu.org> 20021023 -- Add debug message
		PRINT("\t\tFailed to find file!!!\n");
		return;	// failed to find a item
	}
	
	switch (msg()->what)
	{
	case WDownloadEvent::Init:
		{
			PRINT("\tWDownloadEvent::Init\n");
			const char * filename, * user;
			if (
				(msg()->FindString("file", &filename) == B_OK) && 
				(msg()->FindString("user", &user) == B_OK)
				)
			{
				item->setText(WTransferItem::Filename, QString::fromUtf8(filename));
				item->setText(WTransferItem::User, GetUserName(dt));
				item->setText(WTransferItem::Index, FormatIndex(dt->GetCurrentNum(), dt->GetNumFiles()));
			}
			break;
		}
		
	case WDownloadEvent::FileQueued:
		{
			PRINT("\tWDownloadEvent::FileQueued\n");
			item->setText(WTransferItem::Status, tr("Remotely Queued."));
			item->setText(WTransferItem::Rate, "0.0");
			item->setText(WTransferItem::ETA, "");
			item->setText(WTransferItem::Elapsed, "");
			break;
		}
		
	case WDownloadEvent::FileBlocked:
		{
			PRINT("\tWDownloadEvent::FileBlocked\n");
			uint64 timeLeft = (uint64) -1;
			(void) msg()->FindInt64("timeleft", (int64 *) &timeLeft);
			if (timeLeft == (uint64) -1)
			{
				item->setText(WTransferItem::Status, tr("Blocked."));
			}
			else
			{
				item->setText(WTransferItem::Status, tr("Blocked for %1 minute(s).").arg((int) (timeLeft/60000000)));
			}
			item->setText(WTransferItem::Rate, "0.0");
			item->setText(WTransferItem::ETA, "");
			item->setText(WTransferItem::Elapsed, "");
			
			break;
		}
		
	case WDownloadEvent::ConnectBackRequest:
		{
			PRINT("\tWDownloadEvent::ConnectBackRequest\n");
			MessageRef cb(GetMessageFromPool(NetClient::CONNECT_BACK_REQUEST));
			if (cb())
			{
				String session;
				int32 port;
				if (
					(msg()->FindInt32("port", &port) == B_OK) && 
					(msg()->FindString("session", session) == B_OK)
					)
				{
					item->setText(WTransferItem::Status, tr("Waiting for incoming connection..."));
					String tostr = "/*/";
					tostr += session.Cstr();
					tostr += "/beshare";
					cb()->AddString(PR_NAME_KEYS, tostr);
					cb()->AddString("session", session);
					cb()->AddInt32("port", port);
					gWin->fNetClient->SendMessageToSessions(cb);
					break;
				}
			}
			dt->Reset();	// failed...
			break;
		}
		
	case WDownloadEvent::FileHashing:
		{
			PRINT("\tWDownloadEvent::FileHashing\n");
			item->setText(WTransferItem::Status, tr("Examining for resume..."));
			break;
		}
		
	case WDownloadEvent::ConnectInProgress:
		{
			PRINT("\tWDownloadEvent::ConnectInProgress\n");
			item->setText(WTransferItem::Status, tr("Connecting..."));
			break;
		}
		
	case WDownloadEvent::ConnectFailed:
		{
			PRINT("\tWDownloadEvent::ConnectFailed\n");
			String why, mFile;
			msg()->FindString("why", why);
			item->setText(WTransferItem::Status, tr("Connect failed: %1").arg(tr(why.Cstr())));
			dt->SetFinished(true);
			for (int n = dt->GetCurrentNum(); n < dt->GetNumFiles(); n++)
			{
				QString qFile = dt->GetFileName(n);
				QString qLFile = dt->GetLocalFileName(n);
				emit FileFailed(qFile, qLFile, dt->GetRemoteUser());
			}
			dt->Reset();
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearDownloads);
			}
			
			SendSignal(DequeueDownloads);
			break;
		}
		
	case WDownloadEvent::Connected:
		{
			PRINT("\tWDownloadEvent::Connected\n");
			item->setText(WTransferItem::Status, tr("Negotiating..."));
			break;
		}
		
	case WDownloadEvent::Disconnected:
		{
			PRINT("\tWDownloadEvent::Disconnected\n");
			if (item->text(0) != tr("Finished."))
				item->setText(WTransferItem::Status, tr("Disconnected."));
			
			if (dt->IsManuallyQueued())
			{
				item->setText(WTransferItem::Status, tr("Manually Queued."));
				item->setText(WTransferItem::Rate, "0.0");
				item->setText(WTransferItem::ETA, "");
				item->setText(WTransferItem::Elapsed, "");
			}
			else
			{
				dt->SetFinished(true);
				// emit FileFailed signal(s), so we can record the filename and remote username for resuming later
				bool f;
				if ((msg()->FindBool("failed", &f) == B_OK) && f)
				{
					// "failed" == true only, if the transfer has failed
					if (dt->GetCurrentNum() != -1)
					{
						for (int n = dt->GetCurrentNum(); n < dt->GetNumFiles(); n++)
						{
							QString qFile = dt->GetFileName(n);
							QString qLFile = dt->GetLocalFileName(n);
							emit FileFailed(qFile, qLFile, dt->GetRemoteUser());
						}
					}
				}
				else
				{
					item->setText(WTransferItem::Status, tr("Finished."));
					item->setText(WTransferItem::ETA, "");
				}
			}
			dt->Reset();
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearDownloads);
			}
			
			SendSignal(DequeueDownloads);
			PRINT("\tWDownloadEvent::Disconnected OK\n");
			break;
		}
		
	case WDownloadEvent::FileDone:
		{
			PRINT("\tWDownloadEvent::FileDone\n");
			bool d;
			if (msg()->FindBool("done", &d) == B_OK)
			{
				PRINT("\tFound done\n");
				if (dt->IsLastFile())
				{
					dt->Reset();
				}
				item->setText(WTransferItem::Status, tr("Finished."));
				item->setText(WTransferItem::ETA, "");
			}
			else
			{
				item->setText(WTransferItem::Status, tr("Waiting..."));
				item->setText(WTransferItem::Filename, tr("Waiting for next file..."));
				item->setText(WTransferItem::Received, "");
				item->setText(WTransferItem::Total, "");
				item->setText(WTransferItem::Rate, "0.0");
				item->setText(WTransferItem::ETA, "");
				// v- don't erase the user name
				// item->setText(WTransferItem::User, "");	
			}
			PRINT("\tWDownloadEvent::FileDone OK\n");
			break;
		}
		
	case WDownloadEvent::FileFailed:
		{
			PRINT("\tWDownloadEvent::FileFailed\n");
			dt->SetFinished(true);
			if (dt->GetCurrentNum() != -1)
			{
				for (int n = dt->GetCurrentNum(); n < dt->GetNumFiles(); n++)
				{
					QString qFile = dt->GetFileName(n);
					QString qLFile = dt->GetLocalFileName(n);
					emit FileFailed(qFile, qLFile, dt->GetRemoteUser());
				}
			}
			dt->Reset();
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearDownloads);
			}
			
			SendSignal(DequeueDownloads);

			break;
		}
		
	case WDownloadEvent::FileStarted:
		{
			PRINT("\tWDownloadEvent::FileStarted\n");
			String file;
			uint64 start;
			uint64 size;
			String user;
			
			if (
				(msg()->FindString("file", file) == B_OK) && 
				(msg()->FindInt64("start", (int64 *)&start) == B_OK) &&
				(msg()->FindInt64("size", (int64 *)&size) == B_OK) && 
				(msg()->FindString("user", user) == B_OK)
				)
			{
				QString uname = GetUserName(dt);
				
				WString wID(QString::fromUtf8(user.Cstr()));
				WString wUser(uname);
				PRINT("USER ID  : %S\n", wID.getBuffer());
				PRINT("USER NAME: %S\n", wUser.getBuffer());
				
				item->setText(WTransferItem::Status, tr("Waiting for stream..."));
				item->setText(WTransferItem::Filename, QString::fromUtf8( file.Cstr() ) ); // <postmaster@raasu.org> 20021023 -- Unicode fix
				// rec, total, rate
				item->setText(WTransferItem::Received, QString::number((int) start));
				item->setText(WTransferItem::Total, QString::number((int) size));
				item->setText(WTransferItem::Rate, "0.0");
				item->setText(WTransferItem::ETA, "");
				item->setText(WTransferItem::User, uname);
				item->setText(WTransferItem::Index, FormatIndex(dt->GetCurrentNum(), dt->GetNumFiles()));
			}
			dt->fLastData.start();
			break;
		}
		
	case WDownloadEvent::UpdateUI:
		{
			PRINT("\tWDownloadEvent::UpdateUI\n");
			const char * id;
			if (msg()->FindString("id", &id) == B_OK)
			{
				item->setText(WTransferItem::User, GetUserName(dt));
			}
			break;
		}
		
	case WDownloadEvent::FileError:
		{
			PRINT("\tWDownloadEvent::FileError\n");
			String why;
			String file;
			msg()->FindString("why", why);
			if (msg()->FindString("file", file) == B_OK)
				item->setText(WTransferItem::Filename, QString::fromUtf8(file.Cstr()) );
			item->setText(WTransferItem::Status, tr("Error: %1").arg(tr(why.Cstr())));
			item->setText(WTransferItem::Index, FormatIndex(dt->GetCurrentNum(), dt->GetNumFiles()));
			// <postmaster@raasu.org> 20021023 -- Add debug message
			// <postmaster@raasu.org> 20030815 -- Use WString for Unicode
			WString wf(QString::fromUtf8(file.Cstr()));
			PRINT("WGenericEvent::FileError: File %S\n", wf.getBuffer()); 
			break;
		}
		
	case WDownloadEvent::FileDataReceived:
		{
			uint64 offset, size;
			bool done;
			String mFile;
			uint32 got;
			
			if (
				(msg()->FindInt64("offset", (int64 *)&offset) == B_OK) && 
				(msg()->FindInt64("size", (int64 *)&size) == B_OK) &&
				(msg()->FindInt32("got", (int32 *)&got) == B_OK)
				)
			{
				PRINT("\tWGenericEvent::FileDataReceived\n");
#ifdef DEBUG2
# ifdef WIN32
				PRINT("\tOffset: %I64u\n", offset);
				PRINT("\tSize  : %I64u\n", size);
# else
				PRINT("\tOffset: %llu\n", offset);
				PRINT("\tSize  : %llu\n", size);
# endif
				PRINT("\tGot   : %lu\n", got);
#endif
				gWin->UpdateReceiveStats(got);
				
				double secs = 0.0f;
				
				if (dt->fLastData.elapsed() > 0)
				{
					secs = (double)((double)dt->fLastData.elapsed() / 1000.0f);
				}
				
				double gotk = 0.0f;
				
				if (got > 0)
				{
					gotk = (double)((double)got / 1024.0f);
				}
				
				double kps = 0.0f;
				
				if ( (gotk > 0) && (secs > 0) )
				{
					kps = gotk / secs;
				}
				
				item->setText(WTransferItem::Status, tr("Downloading: [%1%]").arg(ComputePercentString(offset, size)));
				item->setText(WTransferItem::Received, QString::number((int) offset));
				// <postmaster@raasu.org> 20021104, 20030217, 20030622
				// elapsed time >= 1 s?
				if (secs >= 1.0f)
				{
					dt->SetMostRecentRate(kps);
					dt->fLastData.restart();
				}
				else
				{
					dt->SetPacketCount(gotk);
				}
				// <postmaster@raasu.org> 20021026 -- Too slow transfer rate?
				double gcr = dt->GetCalculatedRate();
				
				uint64 _fileStarted = dt->GetStartTime();
				if (_fileStarted != 0)
				{
					uint64 _now = GetRunTime64();
					if (_now >= _fileStarted)
					{
						uint64 _elapsed = _now - _fileStarted;
						if (_elapsed > 0) 
							_elapsed /= 1000000;	// convert microseconds to seconds
						item->setText(WTransferItem::Elapsed, QString::number((ulong) _elapsed));
					}
				}
				
				item->setText(WTransferItem::ETA, dt->GetETA(offset / 1024, size / 1024, gcr));
				
				item->setText(WTransferItem::Rate, QString::number(gcr*1024.0f));
				
				if (msg()->FindBool("done", &done) == B_OK)
				{
					item->setText(WTransferItem::Status, tr("File finished."));
					item->setText(WTransferItem::ETA, "");
					
					if (dt->IsFinished())
					{
						dt->Reset();
					}
				}
				PRINT("\tWGenericEvent::FileDataReceived OK\n");
			}
			break;
		}
	}
}

void
WDownload::uploadEvent(WUploadEvent *u)
{
	MessageRef msg = u->Msg();
	WUploadThread * ut = NULL;
	WTransferItem * item = NULL;
	ULPair foundIt;
	
	if (!msg())
		return; // Invalid MessageRef!
	
	if (msg()->FindPointer("sender", (void **)&ut) != B_OK)
		return;	// failed! ouch!
	
	if (!ut)
		return;
	
	Lock();
	for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
	{
		fUploadList.GetItemAt(i, foundIt);
		if (foundIt.first == ut)
		{
			// found our thread
			item = foundIt.second;
			PRINT("\t\tFound ul!!!\n");
			break;
		}
	}
	Unlock();
	
	if (!item)
	{
		// <postmaster@raasu.org> 20021023 -- Add debug message
		PRINT("\t\tFailed to find file!!!\n");
		return;	// failed to find a item
	}

	switch (msg()->what)
	{
	case WUploadEvent::Init:
		{
			PRINT("\tWUploadEvent::Init\n");
			const char * filename, * user;
			if (
				(msg()->FindString("file", &filename) == B_OK) && 
				(msg()->FindString("user", &user) == B_OK)
				)
			{
				item->setText(WTransferItem::Filename, QString::fromUtf8(filename));
				item->setText(WTransferItem::User, GetUserName(ut));
				item->setText(WTransferItem::Index, FormatIndex(ut->GetCurrentNum(), ut->GetNumFiles()));
			}
			break;
		}
		
	case WUploadEvent::FileQueued:
		{
			PRINT("\tWUploadEvent::FileQueued\n");
			item->setText(WTransferItem::Status, tr("Queued."));
			item->setText(WTransferItem::Rate, "0.0");
			item->setText(WTransferItem::ETA, "");
			item->setText(WTransferItem::Elapsed, "");
			break;
		}
		
	case WUploadEvent::FileBlocked:
		{
			PRINT("\tWUploadEvent::FileBlocked\n");
			uint64 timeLeft = (uint64) -1;
			(void) msg()->FindInt64("timeleft", (int64 *) &timeLeft);
			if (timeLeft == (uint64) -1)
			{
				item->setText(WTransferItem::Status, tr("Blocked."));
			}
			else
			{
				item->setText(WTransferItem::Status, tr("Blocked for %1 minute(s).").arg((int) (timeLeft/60000000)));
			}
			item->setText(WTransferItem::Rate, "0.0");
			item->setText(WTransferItem::ETA, "");
			item->setText(WTransferItem::Elapsed, "");

			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearUploads);
			}
			
			SendSignal(DequeueUploads);
			
			break;
		}
		
	case WUploadEvent::FileHashing:
		{
			PRINT("\tWUploadEvent::FileHashing\n");
			item->setText(WTransferItem::Status, tr("Examining for resume..."));
			break;
		}
		
	case WUploadEvent::ConnectInProgress:
		{
			PRINT("\tWUploadEvent::ConnectInProgress\n");
			item->setText(WTransferItem::Status, tr("Connecting..."));
			break;
		}
		
	case WUploadEvent::ConnectFailed:
		{
			PRINT("\tWUploadEvent::ConnectFailed\n");
			String why, mFile;
			msg()->FindString("why", why);
			item->setText(WTransferItem::Status, tr("Connect failed: %1").arg(tr(why.Cstr())));

			// ut->SetFinished(true);
			ut->Reset();
			
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearUploads);
			}
			
			SendSignal(DequeueUploads);
			break;
		}
		
	case WUploadEvent::Connected:
		{
			PRINT("\tWUploadEvent::Connected\n");
			item->setText(WTransferItem::Status, tr("Negotiating..."));
			break;
		}
		
	case WUploadEvent::Disconnected:
		{
			PRINT("\tWUploadEvent::Disconnected\n");
			item->setText(WTransferItem::ETA, "");
		
			bool f;
			if ((msg()->FindBool("failed", &f) == B_OK) && f)
			{
				// "failed" == true only, if the transfer has failed
				item->setText(WTransferItem::Status, tr("Disconnected."));
			}
			else
			{
				item->setText(WTransferItem::Status, tr("Finished."));
			}
			
			ut->SetFinished(true);
			// ut->SetLocallyQueued(false);
			
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearUploads);
			}
			
			SendSignal(DequeueUploads);
			PRINT("\tWUploadEvent::Disconnected OK\n");
			break;
		}
		
	case WUploadEvent::FileDone:
		{
			PRINT("\tWUploadEvent::FileDone\n");
			bool d;
			if (msg()->FindBool("done", &d) == B_OK)
			{
				PRINT("\tFound done\n");
				if (ut->IsLastFile())
				{
					ut->Reset();
				}
				item->setText(WTransferItem::Status, tr("Finished."));
				item->setText(WTransferItem::ETA, "");
			}
			PRINT("\tWUploadEvent::FileDone OK\n");
			break;
		}
		
	case WUploadEvent::FileStarted:
		{
			PRINT("\tWUploadEvent::FileStarted\n");
			String file;
			uint64 start;
			uint64 size;
			String user;
			
			if (
				(msg()->FindString("file", file) == B_OK) && 
				(msg()->FindInt64("start", (int64 *)&start) == B_OK) &&
				(msg()->FindInt64("size", (int64 *)&size) == B_OK) && 
				(msg()->FindString("user", user) == B_OK)
				)
			{
				QString uname = GetUserName(ut);
				
				WString wID(QString::fromUtf8(user.Cstr()));
				WString wUser(uname);
				PRINT("USER ID  : %S\n", wID.getBuffer());
				PRINT("USER NAME: %S\n", wUser.getBuffer());
				
				item->setText(WTransferItem::Status, tr("Waiting for stream..."));
				item->setText(WTransferItem::Filename, QString::fromUtf8( file.Cstr() ) ); // <postmaster@raasu.org> 20021023 -- Unicode fix
				// rec, total, rate
				item->setText(WTransferItem::Received, QString::number((int) start));
				item->setText(WTransferItem::Total, QString::number((int) size));
				item->setText(WTransferItem::Rate, "0.0");
				item->setText(WTransferItem::ETA, "");
				item->setText(WTransferItem::User, uname);
				item->setText(WTransferItem::Index, FormatIndex(ut->GetCurrentNum(), ut->GetNumFiles()));
			}
			ut->fLastData.start();
			break;
		}
		
	case WUploadEvent::UpdateUI:
		{
			PRINT("\tWUploadEvent::UpdateUI\n");
			const char * id;
			if (msg()->FindString("id", &id) == B_OK)
			{
				item->setText(WTransferItem::User, GetUserName(ut));
			}
			break;
		}
		
	case WUploadEvent::FileError:
		{
			PRINT("\tWUploadEvent::FileError\n");
			String why;
			String file;
			msg()->FindString("why", why);
			if (msg()->FindString("file", file) == B_OK)
				item->setText(WTransferItem::Filename, QString::fromUtf8(file.Cstr()) );
			item->setText(WTransferItem::Status, tr("Error: %1").arg(tr(why.Cstr())));
			item->setText(WTransferItem::Index, FormatIndex(ut->GetCurrentNum(), ut->GetNumFiles()));
			// <postmaster@raasu.org> 20021023 -- Add debug message
			// <postmaster@raasu.org> 20030815 -- Use WString for Unicode
			WString wf(QString::fromUtf8(file.Cstr()));
			PRINT("WGenericEvent::FileError: File %S\n", wf.getBuffer()); 
			break;
		}
		
	case WUploadEvent::FileDataSent:
		{
			uint64 offset, size;
			bool done;
			String mFile;
			uint32 got;
			
			if (
				(msg()->FindInt64("offset", (int64 *)&offset) == B_OK) && 
				(msg()->FindInt64("size", (int64 *)&size) == B_OK) &&
				(msg()->FindInt32("sent", (int32 *)&got) == B_OK)
				)
			{
#ifdef DEBUG2
				PRINT("\tWUploadEvent::FileDataSent\n");
# ifdef WIN32
				PRINT("\tOffset: %I64u\n", offset);
				PRINT("\tSize  : %I64u\n", size);
# else
				PRINT("\tOffset: %llu\n", offset);
				PRINT("\tSize  : %llu\n", size);
# endif
				PRINT("\tSent  : %lu\n", got);
#endif
				gWin->UpdateTransmitStats(got);
				
				double secs = 0.0f;
				
				if (ut->fLastData.elapsed() > 0)
				{
					secs = (double)((double)ut->fLastData.elapsed() / 1000.0f);
				}
				
				double gotk = 0.0f;
				
				if (got > 0)
				{
					gotk = (double)((double)got / 1024.0f);
				}
				
				double kps = 0.0f;
				
				if ( (gotk > 0) && (secs > 0) )
				{
					kps = gotk / secs;
				}
				
				item->setText(WTransferItem::Status, tr("Uploading: [%1%]").arg(ComputePercentString(offset, size)));
				item->setText(WTransferItem::Received, QString::number((int) offset));
				// <postmaster@raasu.org> 20021104, 20030217, 20030622
				// elapsed time >= 1 s?
				if (secs >= 1.0f) 
				{
					ut->SetMostRecentRate(kps);
					ut->fLastData.restart();
				}
				else
				{
					ut->SetPacketCount(gotk);
				}
				
				// <postmaster@raasu.org> 20021026 -- Too slow transfer rate?
				double gcr = ut->GetCalculatedRate();
				
				uint64 _fileStarted = ut->GetStartTime();
				if (_fileStarted != 0)
				{
					uint64 _now = GetRunTime64();
					if (_now >= _fileStarted)
					{
						uint64 _elapsed = _now - _fileStarted;
						if (_elapsed > 0)
							_elapsed /= 1000000;	// convert microseconds to seconds
						item->setText(WTransferItem::Elapsed, QString::number((ulong) _elapsed));
					}
				}
				
				
				item->setText(WTransferItem::ETA, ut->GetETA(offset / 1024, size / 1024, gcr));
				
				item->setText(WTransferItem::Rate, QString::number(gcr*1024.0f));
				
				if (msg()->FindBool("done", &done) == B_OK)
				{
					item->setText(WTransferItem::Status, tr("Finished."));
					item->setText(WTransferItem::ETA, "");
				}
#ifdef DEBUG2
				PRINT("\tWUploadEvent::FileDataSent OK\n");
#endif
			}
			break;
		}
	}
}

void
WDownload::KillLocalQueues()
{
	Lock();
	unsigned int j = 0;
	while (j < fDownloadList.GetNumItems())
	{
		DLPair pair;
		fDownloadList.GetItemAt(j, pair);
		if (pair.first)
		{
			if (pair.first->IsLocallyQueued())	// found queued item?
			{
				// Put all files in resume list
				int n = pair.first->GetCurrentNum();
				
				if (n > -1) 
				{
					for (int i = n; i < pair.first->GetNumFiles(); i++)
					{
						emit FileInterrupted(pair.first->GetFileName(i), pair.first->GetLocalFileName(i), pair.first->GetRemoteUser());
					}
				}
				
				// free it
				delete pair.second;
				pair.first->Reset();
				delete pair.first;
				fDownloadList.RemoveItemAt(j);
			}
			else
			{
				j++;
			}
		}
	}
	Unlock();
}

QString
WDownload::GetUserName(WDownloadThread *dt)
{
	QString sid = dt->GetRemoteID();
	QString name = dt->GetRemoteUser();
	QString ret;
	ret = tr("%1 (%2)").arg(name).arg(sid);
	return ret;
}

QString
WDownload::GetUserName(WUploadThread *ut)
{
	QString sid = ut->GetRemoteID();
	QString name = ut->GetRemoteUser();
	QString ret;
	ret = tr("%1 (%2)").arg(name).arg(sid);
	return ret;
}


void
WDownload::UpdateLoad()
{
	PRINT("WDownload::UpdateLoad\n");
	if (gWin->fNetClient)
	{
		int mu = 0;
		if (gWin->fSettings)
		{
			mu = gWin->fSettings->GetMaxUploads();
		}
		gWin->fNetClient->SetLoad(GetUploadQueue(), gWin->fSettings->GetMaxUploads());
	}
	PRINT("WDownload::UpdateLoad OK\n");
}

/*
*
* Block users that disconnect if user has chosen that feature in settings
*
*/
void
WDownload::UserDisconnected(const QString &sid, const QString & /* name */)
{
	if (gWin->fSettings->GetBlockDisconnected())
	{
		Lock();
		for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
		{
			ULPair pair;
			fUploadList.GetItemAt(i, pair);
			if (pair.first)
			{
				if (pair.first->GetRemoteID() == sid)
				{
					// Only block transfers that are active
					if (
						(pair.first->IsBlocked() == false) &&
						(pair.first->IsActive() == true)
						)
					{
						pair.first->SetBlocked(true);
						pair.first->Reset();
					}
				}
			}
		}
		Unlock();
	}
}

QString
WDownload::FormatIndex(int32 cur, int32 num)
{
	return QObject::tr("%1 of %2").arg(cur+1).arg(num);
}

void
WDownload::DLPopupActivated(int id)
{
	DLPair pair;
	unsigned int index;
	WDownloadThread * dt = NULL;
	bool findRet;
	
	findRet = FindDLItem(index, fDLPopupItem);
	
	if (findRet)
	{
		fDownloadList.GetItemAt(index, pair);
	}
	else
	{
		PRINT("Download item not found!");
		return;
	}
	
	dt = pair.first;
	WASSERT(dt != NULL, "Download Thread is invalid!");
	
	switch (id)
	{
	case ID_QUEUE:
		{
			if (!dt->IsLocallyQueued())
			{
				dt->SetLocallyQueued(true);
				dt->SetManuallyQueued(true);
				dt->Reset();
			}
			else
			{
				dt->SetLocallyQueued(false);
				dt->SetManuallyQueued(false);
				dt->InitSession();
			}
			break;
		}
		
	case ID_MOVEUP:
		{
			if (dt->IsLocallyQueued() || dt->IsRemotelyQueued())
				DLMoveUp(index);
			break;
		}
		
	case ID_MOVEDOWN:
		{
			if (dt->IsLocallyQueued() || dt->IsRemotelyQueued())
				DLMoveDown(index);
			break;
		}
		
	case ID_CLEAR:
		{
			SendSignal(ClearDownloads);
			break;
		}
		
	case ID_CANCEL:
		{
			Lock();
			// found our item, stop it
			fDLPopupItem->setText(WTransferItem::Status, tr("Canceled."));
			dt->SetFinished(true);
			dt->Reset();
			Unlock();
			
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearDownloads);
			}
			
			SendSignal(DequeueDownloads);
			break;
		}
		
	case ID_NO_LIMIT:
		{
			dt->SetRate(0);
			break;
		}
		
	case ID_64:
		{
			dt->SetRate(64);
			break;
		}
		
	case ID_128:
		{
			dt->SetRate(128);
			break;
		}
		
	case ID_256:
		{
			dt->SetRate(256);
			break;
		}
		
	case ID_512:
		{
			dt->SetRate(512);
			break;
		}
		
	case ID_1KB:
		{
			dt->SetRate(1024);
			break;
		}
		
	case ID_2KB:
		{
			dt->SetRate(2 * 1024);
			break;
		}
		
	case ID_4KB:
		{
			dt->SetRate(4 * 1024);
			break;
		}
		
	case ID_6KB:
		{
			dt->SetRate(6 * 1024);
			break;
		}
		
	case ID_8KB:
		{
			dt->SetRate(8 * 1024);
			break;
		}
		
	case ID_10KB:
		{
			dt->SetRate(10 * 1024);
			break;
		}
		
	case ID_12KB:
		{
			dt->SetRate(12 * 1024);
			break;
		}
		
	case ID_14KB:
		{
			dt->SetRate(14 * 1024);
			break;
		}
		
	case ID_16KB:
		{
			dt->SetRate(16 * 1024);
			break;
		}
		
	case ID_32KB:
		{
			dt->SetRate(32 * 1024);
			break;
		}
		
	case ID_64KB:
		{
			dt->SetRate(64 * 1024);
			break;
		}
		
	case ID_128KB:
		{
			dt->SetRate(128 * 1024);
			break;
		}
		
	case ID_256KB:
		{
			dt->SetRate(256 * 1024);
			break;
		}
		
	case ID_512KB:
		{
			dt->SetRate(512 * 1024);
			break;
		}
		
	case ID_1MB:
		{
			dt->SetRate(1024 * 1024);
			break;
		}
		
	case ID_2MB:
		{
			dt->SetRate(2 * 1024 * 1024);
			break;
		}
		
	case ID_4MB:
		{
			dt->SetRate(4 * 1024 * 1024);
			break;
		}
		
	case ID_8MB:
		{
			dt->SetRate(8 * 1024 * 1024);
			break;
		}
		
	case ID_16MB:
		{
			dt->SetRate(16 * 1024 * 1024);
			break;
		}
		
	case ID_32MB:
		{
			dt->SetRate(32 * 1024 * 1024);
			break;
		}
	default:
		{
			// Handle "Run..." items here...
			
			if (id & ID_RUN)
			{
				int n = id - ID_RUN - 1;
				RunCommand(dt->GetLocalFileName(n));
			}
		}
		
	}
	SendSignal(DLRatings);
}

void
WDownload::ULPopupActivated(int id)
{
	ULPair pair;
	unsigned int index;
	WUploadThread * ut = NULL;
	bool findRet;
	
	findRet = FindULItem(index, fULPopupItem);
	
	if (findRet)
	{
		fUploadList.GetItemAt(index, pair);
	}
	else
	{
		PRINT("Upload item not found!");
		return;
	}
	
	ut = pair.first;
	WASSERT(ut != NULL, "Upload Thread is invalid!");
	
	switch (id)
	{
	case ID_QUEUE:
		{
			if (!ut->IsLocallyQueued())
			{
				ut->SetLocallyQueued(true);
				ut->SetManuallyQueued(true);
			}
			else
			{
				ut->SetLocallyQueued(false);
				ut->SetManuallyQueued(false);
			}
			break;
		}
		
	case ID_MOVEUP:
		{
			if (ut->IsLocallyQueued())
				ULMoveUp(index);
			break;
		}
		
	case ID_MOVEDOWN:
		{
			if (ut->IsLocallyQueued())
				ULMoveDown(index);
			break;
		}
		
	case ID_CLEAR:
		{
			SendSignal(ClearUploads);
			break;
		}	
		
	case ID_CANCEL:
		{
			Lock();
			// found our item, stop it
			fULPopupItem->setText(WTransferItem::Status, tr("Canceled."));
			ut->SetFinished(true);
			ut->Reset();
			Unlock();
			
			if (gWin->fSettings->GetAutoClear())
			{
				SendSignal(ClearUploads);
			}
			
			SendSignal(DequeueUploads);
			break;
		}
		
	case ID_NO_LIMIT:
		{
			ut->SetRate(0);
			break;
		}
		
	case ID_64:
		{
			ut->SetRate(64);
			break;
		}
		
	case ID_128:
		{
			ut->SetRate(128);
			break;
		}
		
	case ID_256:
		{
			ut->SetRate(256);
			break;
		}
		
	case ID_512:
		{
			ut->SetRate(512);
			break;
		}
		
	case ID_1KB:
		{
			ut->SetRate(1024);
			break;
		}
		
	case ID_2KB:
		{
			ut->SetRate(2 * 1024);
			break;
		}
		
	case ID_4KB:
		{
			ut->SetRate(4 * 1024);
			break;
		}
		
	case ID_6KB:
		{
			ut->SetRate(6 * 1024);
			break;
		}
		
	case ID_8KB:
		{
			ut->SetRate(8 * 1024);
			break;
		}
		
	case ID_10KB:
		{
			ut->SetRate(10 * 1024);
			break;
		}
		
	case ID_12KB:
		{
			ut->SetRate(12 * 1024);
			break;
		}
		
	case ID_14KB:
		{
			ut->SetRate(14 * 1024);
			break;
		}
		
	case ID_16KB:
		{
			ut->SetRate(16 * 1024);
			break;
		}
		
	case ID_32KB:
		{
			ut->SetRate(32 * 1024);
			break;
		}
		
	case ID_64KB:
		{
			ut->SetRate(64 * 1024);
			break;
		}
		
	case ID_128KB:
		{
			ut->SetRate(128 * 1024);
			break;
		}
		
	case ID_256KB:
		{
			ut->SetRate(256 * 1024);
			break;
		}
		
	case ID_512KB:
		{
			ut->SetRate(512 * 1024);
			break;
		}
		
	case ID_1MB:
		{
			ut->SetRate(1024 * 1024);
			break;
		}
		
	case ID_2MB:
		{
			ut->SetRate(2 * 1024 * 1024);
			break;
		}
		
	case ID_4MB:
		{
			ut->SetRate(4 * 1024 * 1024);
			break;
		}
		
	case ID_8MB:
		{
			ut->SetRate(8 * 1024 * 1024);
			break;
		}
		
	case ID_16MB:
		{
			ut->SetRate(16 * 1024 * 1024);
			break;
		}
		
	case ID_32MB:
		{
			ut->SetRate(32 * 1024 * 1024);
			break;
		}
		
	case ID_UNBAN:
		{
			ut->SetBlocked(false);
			break;
		}
		
	case ID_BAN1:
		{
			ut->SetBlocked(true, 60000000);
			break;
		}
		
	case ID_BAN2:
		{
			ut->SetBlocked(true, 120000000);
			break;
		}
		
	case ID_BAN5:
		{
			ut->SetBlocked(true, 300000000);
			break;
		}
		
	case ID_BAN10:
		{
			ut->SetBlocked(true, 600000000);
			break;
		}
		
	case ID_BAN15:
		{
			ut->SetBlocked(true, 900000000);
			break;
		}
		
	case ID_BAN30:
		{
			ut->SetBlocked(true, 1800000000);
			break;
		}
		
	case ID_BAN1H:
		{
#if defined(__LINUX__) || defined(linux) || defined(__FreeBSD__)
			ut->SetBlocked(true, 3600000000LL);
#else
			ut->SetBlocked(true, 3600000000UL);
#endif
			break;
		}
		
	case ID_BANINF:
		{
			ut->SetBlocked(true, -1);
			break;
		}
		
	case ID_PACKET1K:
		{
			ut->SetPacketSize(1);
			break;
		}
		
	case ID_PACKET2K:
		{
			ut->SetPacketSize(2);
			break;
		}
		
	case ID_PACKET4K:
		{
			ut->SetPacketSize(4);
			break;
		}
		
	case ID_PACKET8K:
		{
			ut->SetPacketSize(8);
			break;
		}
		
	case ID_PACKET16K:
		{
			ut->SetPacketSize(16);
			break;
		}
		
	case ID_PACKET32K:
		{
			ut->SetPacketSize(32);
			break;
		}
		
	case ID_IGNORE:
		{
			if ( gWin->IsIgnoredIP( ut->GetRemoteIP() ) )
			{
				gWin->RemoveIPIgnore(ut->GetRemoteIP());
				if (ut->IsBlocked())
					ut->SetBlocked(false, -1);
			}
			else
			{
				gWin->AddIPIgnore(ut->GetRemoteIP());
				if (!ut->IsBlocked())
					ut->SetBlocked(true, -1);
			}
			break;
		}
	}
	SendSignal(ULRatings);
}

void
WDownload::ULRightClicked(QListViewItem * item, const QPoint & p, int)
{
	if (item)
	{
		fULPopupItem = item;
		
		unsigned int index;
		if (FindULItem(index, item))
		{
			ULPair pair;
			fUploadList.GetItemAt(index, pair);
			if ( !pair.first )
			{
				PRINT("Generic Thread is invalid!");
				return;
			}
			
			// Disable items not applicable to current thread state
			
			// Not Locally Queued
			
			if (pair.first->IsLocallyQueued() == false)
			{
				fULPopup->setItemEnabled(ID_MOVEUP, false);
				fULPopup->setItemEnabled(ID_MOVEDOWN, false);
			}
			else
			{
				fULPopup->setItemEnabled(ID_MOVEUP, true);
				fULPopup->setItemEnabled(ID_MOVEDOWN, true);
			}
			
			// Already finished
			
			if (pair.first->IsFinished() == true)
			{
				fULPopup->setItemEnabled(ID_QUEUE, false);
				fULPopup->setItemEnabled(ID_BLOCK, false);
				fULPopup->setItemEnabled(ID_THROTTLE, false);
				fULPopup->setItemEnabled(ID_SETPACKET, false);
				fULPopup->setItemEnabled(ID_CANCEL, false);
			}
			else
			{
				fULPopup->setItemEnabled(ID_QUEUE, true);
				fULPopup->setItemEnabled(ID_BLOCK, true);
				fULPopup->setItemEnabled(ID_THROTTLE, true);
				fULPopup->setItemEnabled(ID_SETPACKET, true);
				fULPopup->setItemEnabled(ID_CANCEL, true);
			}
			
			fULPopup->setItemChecked(ID_QUEUE, pair.first->IsLocallyQueued());
			fULPopup->setItemChecked(ID_IGNORE, gWin->IsIgnoredIP( pair.first->GetRemoteIP() ));
			int fNewRate = pair.first->GetRate();
			fULThrottleMenu->setItemChecked(fULThrottle, false);
			
			switch (fNewRate)
			{
			case 0: 	
				{
					fULThrottle = ID_NO_LIMIT;	
					break;
				}
				
			case 64:
				{
					fULThrottle = ID_64;
					break;
				}
				
			case 128:	
				{
					fULThrottle = ID_128;		
					break;
				}
				
			case 256:	
				{
					fULThrottle = ID_256;		
					break;
				}
				
			case 512:	
				{
					fULThrottle = ID_512;		
					break;
				}
				
			case 1024:	
				{
					fULThrottle = ID_1KB;		
					break;
				}
				
			case 2 * 1024:	
				{
					fULThrottle = ID_2KB;	
					break;
				}
				
			case 4 * 1024:	
				{
					fULThrottle = ID_4KB;	
					break;
				}
				
			case 6 * 1024:
				{
					fULThrottle = ID_6KB;
					break;
				}
				
			case 8 * 1024:	
				{
					fULThrottle = ID_8KB;	
					break;
				}
				
			case 10 * 1024:
				{
					fULThrottle = ID_10KB;
					break;
				}
				
			case 12 * 1024:
				{
					fULThrottle = ID_12KB;
					break;
				}
				
			case 14 * 1024:
				{
					fULThrottle = ID_14KB;
					break;
				}
				
			case 16 * 1024:	
				{
					fULThrottle = ID_16KB; 
					break;
				}
				
			case 32 * 1024: 
				{
					fULThrottle = ID_32KB;	
					break;
				}
				
			case 64 * 1024:	
				{
					fULThrottle = ID_64KB;	
					break;
				}
				
			case 128 * 1024:	
				{
					fULThrottle = ID_128KB; 
					break;
				}
				
			case 256 * 1024:	
				{
					fULThrottle = ID_256KB; 
					break;
				}
				
			case 512 * 1024:	
				{
					fULThrottle = ID_512KB;	
					break;
				}
				
			case 1048576:		
				{
					fULThrottle = ID_1MB;	
					break;
				}
				
			case 2 * 1048576:	
				{
					fULThrottle = ID_2MB;	
					break;
				}
				
			case 4 * 1048576:	
				{
					fULThrottle = ID_4MB;	
					break;
				}
				
			case 8 * 1048576:	
				{
					fULThrottle = ID_8MB;	
					break;
				}
				
			case 16 * 1048576:	
				{
					fULThrottle = ID_16MB;	
					break;
				}
				
			case 32 * 1048576:	
				{
					fULThrottle = ID_32MB; 
					break;
				}
			}
			
			fULThrottleMenu->setItemChecked(fULThrottle, true);
			
			int fNewBan = pair.first->GetBanTime();
			fULBanMenu->setItemChecked(fULBan, false);
			
			switch (fNewBan)
			{
			case -1:
				{
					fULBan = ID_BANINF;
					break;
				}
			case 0:
				{
					fULBan = ID_UNBAN;
					break;
				}
			case 1:
				{
					fULBan = ID_BAN1;
					break;
				}
			case 2:
				{
					fULBan = ID_BAN2;
					break;
				}
			case 5:
				{
					fULBan = ID_BAN5;
					break;
				}
			case 10:
				{
					fULBan = ID_BAN10;
					break;
				}
			case 15:
				{
					fULBan = ID_BAN15;
					break;
				}
			case 30:
				{
					fULBan = ID_BAN30;
					break;
				}
			case 60:
				{
					fULBan = ID_BAN1H;
					break;
				}
			}
			fULBanMenu->setItemChecked(fULBan, true);
			
			int fNewPacket = pair.first->GetPacketSize();
			fULPacketMenu->setItemChecked(fULPacket, false);
			
			switch (fNewPacket)
			{
			case 1:
				{
					fULPacket = ID_PACKET1K;
					break;
				}
			case 2:
				{
					fULPacket = ID_PACKET2K;
					break;
				}
			case 4:
				{
					fULPacket = ID_PACKET4K;
					break;
				}
			case 8:
				{
					fULPacket = ID_PACKET8K;
					break;
				}
			case 16:
				{
					fULPacket = ID_PACKET16K;
					break;
				}
			case 32:
				{
					fULPacket = ID_PACKET32K;
					break;
				}
			}
			fULPacketMenu->setItemChecked(fULPacket, true);
			
			fULPopup->popup(p);
		}
	}
}

void
WDownload::DLRightClicked(QListViewItem * item, const QPoint & p, int)
{
	if (item)
	{
		fDLPopupItem = item;
		
		unsigned int index;
		if (FindDLItem(index, item))
		{
			DLPair pair;
			fDownloadList.GetItemAt(index, pair);
			if ( !pair.first )
			{
				PRINT("Generic Thread is invalid!");
				return;
			}
			
			// Disable items not applicable to current thread state
			
			// Not Locally Queued
			
			if (pair.first->IsLocallyQueued() == false)
			{
				fDLPopup->setItemEnabled(ID_MOVEUP, false);
				fDLPopup->setItemEnabled(ID_MOVEDOWN, false);
			}
			else
			{
				fDLPopup->setItemEnabled(ID_MOVEUP, true);
				fDLPopup->setItemEnabled(ID_MOVEDOWN, true);
			}
			
			// Already finished
			
			if (pair.first->IsFinished() == true)
			{
				fDLPopup->setItemEnabled(ID_THROTTLE, false);
				fDLPopup->setItemEnabled(ID_QUEUE, false);
				fDLPopup->setItemEnabled(ID_RUN, true);
				fDLPopup->setItemEnabled(ID_CANCEL, false);
			}
			else
			{
				fDLPopup->setItemEnabled(ID_THROTTLE, true);
				fDLPopup->setItemEnabled(ID_RUN, false);
				fDLPopup->setItemEnabled(ID_CANCEL, true);

				// Still connecting?

				if (pair.first->IsConnecting() == true)
				{
					fDLPopup->setItemEnabled(ID_QUEUE, false);
				}
				else
				{
					fDLPopup->setItemEnabled(ID_QUEUE, true);
				}
			}
			
			fDLPopup->setItemChecked(ID_QUEUE, pair.first->IsLocallyQueued());
			int fNewRate = pair.first->GetRate();
			fDLThrottleMenu->setItemChecked(fDLThrottle, false);
			switch (fNewRate)
			{
			case 0: 	
				{
					fDLThrottle = ID_NO_LIMIT;	
					break;
				}
				
			case 64:
				{
					fDLThrottle = ID_64;
					break;
				}
				
			case 128:	
				{
					fDLThrottle = ID_128;		
					break;
				}
				
			case 256:	
				{
					fDLThrottle = ID_256;		
					break;
				}
				
			case 512:	
				{
					fDLThrottle = ID_512;		
					break;
				}
				
			case 1024:	
				{
					fDLThrottle = ID_1KB;		
					break;
				}
				
			case 2 * 1024:	
				{
					fDLThrottle = ID_2KB;	
					break;
				}
				
			case 4 * 1024:	
				{
					fDLThrottle = ID_4KB;	
					break;
				}
				
			case 6 * 1024:
				{
					fDLThrottle = ID_6KB;
					break;
				}
				
			case 8 * 1024:	
				{
					fDLThrottle = ID_8KB;	
					break;
				}
				
			case 10 * 1024:
				{
					fDLThrottle = ID_10KB;
					break;
				}
				
			case 12 * 1024:
				{
					fDLThrottle = ID_12KB;
					break;
				}
				
			case 14 * 1024:
				{
					fDLThrottle = ID_14KB;
					break;
				}
				
			case 16 * 1024:	
				{
					fDLThrottle = ID_16KB; 
					break;
				}
				
			case 32 * 1024: 
				{
					fDLThrottle = ID_32KB;	
					break;
				}
				
			case 64 * 1024:	
				{
					fDLThrottle = ID_64KB;	
					break;
				}
				
			case 128 * 1024:	
				{
					fDLThrottle = ID_128KB; 
					break;
				}
				
			case 256 * 1024:	
				{
					fDLThrottle = ID_256KB; 
					break;
				}
				
			case 512 * 1024:	
				{
					fDLThrottle = ID_512KB;	
					break;
				}
				
			case 1048576:		
				{
					fDLThrottle = ID_1MB;	
					break;
				}
				
			case 2 * 1048576:	
				{
					fDLThrottle = ID_2MB;	
					break;
				}
				
			case 4 * 1048576:	
				{
					fDLThrottle = ID_4MB;	
					break;
				}
				
			case 8 * 1048576:	
				{
					fDLThrottle = ID_8MB;	
					break;
				}
				
			case 16 * 1048576:	
				{
					fDLThrottle = ID_16MB;	
					break;
				}
				
			case 32 * 1048576:	
				{
					fDLThrottle = ID_32MB; 
					break;
				}
			}
			fDLThrottleMenu->setItemChecked(fDLThrottle, true);
			
			// Add "Run..." items...
			
			fDLRunMenu->clear();
			int n = pair.first->GetNumFiles();
			for (int r = 0; r < n; r++)
			{
				if (pair.first->GetLocalFileName(r) != QString::null)
					fDLRunMenu->insertItem(pair.first->GetLocalFileName(r), ID_RUN+1+r);
			}
			fDLPopup->popup(p);
		}
	}
}

bool
WDownload::FindDLItem(unsigned int & index, QListViewItem * s)
{
	PRINT("WDownload::FindDLItem()\n");
	bool success = false;
	
	Lock();
	
	for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
	{
		DLPair p;
		fDownloadList.GetItemAt(i, p);
		if (p.second == s)
		{
			index = i;
			success = true;
			break;
		}
	}
	
	Unlock();
	
	if (!success)
	{
		PRINT("WDownload::FindDLItem() : Item not found!\n");
	}
	return success;
}

bool
WDownload::FindULItem(unsigned int & index, QListViewItem * s)
{
	PRINT("WDownload::FindULItem()\n");
	bool success = false;
	
	Lock();
	
	for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
	{
		ULPair p;
		fUploadList.GetItemAt(i, p);
		if (p.second == s)
		{
			index = i;
			success = true;
			break;
		}
	}
	
	Unlock();
	
	if (!success)
	{
		PRINT("WDownload::FindULItem() : Item not found!\n");
	}
	return success;
}

void
WDownload::DLMoveUp(unsigned int index)
{
	if (index == 0)
		return;
	
	DLPair wp;
	
	Lock();
	
	unsigned int index2 = index;
	fDownloadList.RemoveItemAt(index, wp);
	Unlock();
	
	Lock();
	while (index2 > 0) 
	{
		index2--;
		DLPair p;
		fDownloadList.GetItemAt(index2, p);
		if (p.first->IsLocallyQueued())
			break;
	}
	fDownloadList.InsertItemAt(index2, wp);
	
	Unlock();
}

void
WDownload::ULMoveUp(unsigned int index)
{
	if (index == 0)
		return;
	
	ULPair wp;
	
	Lock();
	
	unsigned int index2 = index;
	fUploadList.RemoveItemAt(index, wp);
	Unlock();
	
	Lock();
	while (index2 > 0) 
	{
		index2--;
		ULPair p;
		fUploadList.GetItemAt(index2, p);
		if (p.first->IsLocallyQueued())
			break;
	}
	fUploadList.InsertItemAt(index2, wp);
	
	Unlock();
}

void
WDownload::DLMoveDown(unsigned int index)
{
	if (index == fDownloadList.GetNumItems() - 1)
		return;
	
	DLPair wp;
	
	Lock();
	
	unsigned int index2 = index;
	fDownloadList.RemoveItemAt(index, wp);
	Unlock();
	
	Lock();
	while (index2 < (fDownloadList.GetNumItems() - 2))
	{
		index2++;
		DLPair p;
		fDownloadList.GetItemAt(index2, p);
		if (p.first->IsLocallyQueued())
			break;
	}
	
	index2++;
	
	fDownloadList.InsertItemAt(index2, wp);
	
	Unlock();
}

void
WDownload::ULMoveDown(unsigned int index)
{
	if (index == fUploadList.GetNumItems() - 1)
		return;
	
	ULPair wp;
	
	Lock();
	
	unsigned int index2 = index;
	fUploadList.RemoveItemAt(index, wp);
	Unlock();
	
	Lock();
	while (index2 < (fDownloadList.GetNumItems() - 2))
	{
		index2++;
		ULPair p;
		fUploadList.GetItemAt(index2, p);
		if (p.first->IsLocallyQueued())
			break;
	}
	
	index2++;
	
	fUploadList.InsertItemAt(index2, wp);
	
	Unlock();
}

void
WDownload::UpdateULRatings()
{
	PRINT("\tWDownload::UpdateULRatings\n");
	if ( fUploadList.IsEmpty() )
	{
		PRINT("\tUpload list is empty!\n");
		return;
	}
	
	Lock();
	
	for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
	{
		ULPair pair;
		fUploadList.GetItemAt(i, pair);
		if (pair.second)
		{
			WString wFile(pair.second->text(WTransferItem::Filename));
			if (wFile.length() > 0)
				PRINT("Item %d: %S\n", i, wFile.getBuffer() );
			else
				PRINT("Item %d\n", i);
			
			pair.second->setText(WTransferItem::QR, QString::number(i));
		}
	}
	//	fUploads->triggerUpdate();
	
	Unlock();
	
	PRINT("\tWDownload::UpdateULRatings OK\n");
}

void
WDownload::UpdateDLRatings()
{
	PRINT("\tWDownload::UpdateDLRatings\n");
	if ( fDownloadList.IsEmpty() )
	{
		PRINT("\tDownload list is empty!\n");
		return;
	}
	
	Lock();
	for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
	{
		DLPair pair;
		fDownloadList.GetItemAt(i, pair);
		if (pair.second)
		{
			pair.second->setText(WTransferItem::QR, QString::number(i));
		}
	}
	//	fDownloads->triggerUpdate();
	
	Unlock();
	
	PRINT("\tWDownload::UpdateDLRatings OK\n");
}

void
WDownload::TransferCallBackRejected(const QString &qFrom, int64 timeLeft, uint32 port)
{
	Lock();
	for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
	{
		DLPair pair;
		fDownloadList.GetItemAt(i, pair);
		if (pair.first)
		{
			if (
				(pair.first->IsActive() == false) &&
				(pair.first->GetRemoteUser() == qFrom) && 
				(pair.first->GetRemotePort() == port)
				)
			{
				pair.first->SetBlocked(true, timeLeft);
				break;
			}
		}
	}
	Unlock();
}

// Clear all finished downloads from listview
void
WDownload::ClearFinishedDL()
{
	PRINT("\tWDownload::ClearFinishedDL\n");
	
	if (fDownloadList.GetNumItems() > 0)
	{
		Lock();
		unsigned int i = 0;
		while (i < fDownloadList.GetNumItems())
		{
			DLPair pair;
			fDownloadList.GetItemAt(i, pair);
			if ((pair.first->IsFinished() == true) && (pair.first->IsManuallyQueued() == false))
			{
				// found finished item, erase it
				fDownloadList.RemoveItemAt(i);
				
				// free resources
				pair.first->Reset();
				delete pair.first;
				delete pair.second;
			}
			else
			{
				i++;
			}
		}
		Unlock();
	}
	
	SendSignal(DLRatings);
	
	PRINT("\tWDownload::ClearFinishedDL OK\n");
}

// Clear all finished uploads from listview
void
WDownload::ClearFinishedUL()
{
	PRINT("\tWDownload::ClearFinishedUL\n");
	
	if (fUploadList.GetNumItems() > 0)
	{
		Lock();
		unsigned int i = 0;
		while (i < fUploadList.GetNumItems())
		{
			ULPair pair;
			fUploadList.GetItemAt(i, pair);
			if (pair.first->IsFinished() == true)
			{
				// found finished item, erase it
				fUploadList.RemoveItemAt(i);
				// free resources
				pair.first->Reset();
				delete pair.first;
				delete pair.second;
			}
			else
			{
				i++;
			}
		}
		Unlock();
	}
	
	SendSignal(ULRatings);
	
	PRINT("\tWDownload::ClearFinishedUL OK\n");
}

int
WDownload::GetNumDownloads()
{
	PRINT("\tWDownload::GetNumDownloads\n");
	int n = 0;
	if ( fDownloadList.GetNumItems() > 0)
	{
		Lock();
		for (unsigned int i = 0; i < fDownloadList.GetNumItems(); i++)
		{
			DLPair pair;
			fDownloadList.GetItemAt(i, pair);
			if (pair.first)
			{
				if (
					(pair.first->IsLocallyQueued() == false) &&
					(pair.first->IsRemotelyQueued() == false) &&
					(pair.first->IsBlocked() == false) &&
					(pair.first->IsFinished() == false)
					)
				{
					n++;
				}
			}
		}
		Unlock();
	}
	return n;
}

int
WDownload::GetNumUploads()
{
	PRINT("\tWDownload::GetNumUploads\n");
	int n = 0;
	if ( fUploadList.GetNumItems() > 0)
	{
		Lock();
		for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
		{
			ULPair pair;
			fUploadList.GetItemAt(i, pair);
			if (pair.first)
			{
				if (
					(pair.first->IsLocallyQueued() == false) &&
					(pair.first->IsBlocked() == false) &&
					(pair.first->IsFinished() == false)
					)
				{
					n++;
				}
			}
		}
		Unlock();
	}
	return n;
}

int
WDownload::GetUploadQueue()
{
	PRINT("\tWDownload::GetUploadQueue\n");
	int n = 0;
	if ( fUploadList.GetNumItems() > 0)
	{
		Lock();
		for (unsigned int i = 0; i < fUploadList.GetNumItems(); i++)
		{
			ULPair pair;
			fUploadList.GetItemAt(i, pair);
			if (pair.first)
			{
				if ((pair.first->IsFinished() == false) && (pair.first->IsBlocked() == false))
				{
					n++;
				}
			}
		}
		Unlock();
	}
	return n;
}

void
WDownload::SetLocalID(QString sid)
{
	fLocalSID = sid;
}

void
WDownload::SendSignal(int signal)
{
	QCustomEvent *qce = new QCustomEvent(signal);
	if (qce) 
		QApplication::postEvent(this, qce);
}

