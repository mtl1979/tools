#include "winsharewindow.h"
#include "combo.h"
#include "debugimpl.h"
#include "downloadimpl.h"
#include "settings.h"
#include "wstatusbar.h"
#include "util.h"
#include "wstring.h"
#include "searchitem.h"
#include "netclient.h"

#include <qapplication.h>

void
WinShareWindow::AddFile(const QString &sid, const QString &filename, bool firewalled, MessageRef file)
{
	PRINT("ADDFILE called\n");
	
#ifdef _DEBUG
	WString wFileName(filename);
	WString wSID(sid);
	PRINT("ADDFILE: filename=%S (%s) [%S]\n", wFileName.getBuffer(), firewalled ? "firewalled" : "hackable", wSID.getBuffer());
#endif

	// Workaround for StringMatcher bug (for now!)
	if (fCurrentSearchPattern.Length() == 0)
		return;

	fSearchLock.lock();
	// see if the filename matches our file regex
	// AND that the session ID matches our session ID regex
	if (fFileRegExp.Match((const char *) filename.utf8()))
	{
		PRINT("ADDFILE: file Regex ok\n");
		if (fUserRegExp.Match((const char *) sid.utf8()))
		{
			// yes!
			WUserIter uit = fNetClient->Users().find(sid);
			if (uit != fNetClient->Users().end())	// found our user
			{
				WUserRef user = (*uit).second;

				// Both aren't firewalled or
				// Only one is firewalled or
				// Both are firewalled but other party supports tunneling
				if (!firewalled || !fSettings->GetFirewalled() || user()->GetTunneling())
				{
					
					// We need to check file properties before allocating new WFileInfo, saves time and one unnecessary
					// delete command.
					
					String path, kind;
					uint64 size = 0;
					int32 mod = 0;
					
					file()->FindString("beshare:Kind", kind);
					file()->FindString("beshare:Path", path);
					file()->FindInt32("beshare:Modification Time", (int32 *)&mod);
					file()->FindInt64("beshare:File Size", (int64 *)&size);
					
					WFileInfo * info = new WFileInfo;
					CHECK_PTR(info);
					info->fiUser = user;
					info->fiFilename = filename;
					info->fiSize = size;
					info->fiRef = file;
					info->fiFirewalled = firewalled;

					fQueryBytes += size;
					
					// name, size, type, modified, path, user
					QString qkind	= QString::fromUtf8(kind.Cstr());
					QString qsize	= fromULongLong(size); 
					QString qmod	= QString::number(mod); // <postmaster@raasu.org> 20021126
					QString qpath	= QString::fromUtf8(path.Cstr());
					QString quser	= user()->GetUserName();
					
					info->fiListItem = new WSearchListItem(fSearchList, filename, qsize, qkind, qmod, qpath, quser);
					CHECK_PTR(info->fiListItem);
					
#ifdef WIN32
					PRINT("Setting key to %I64i\n", size);
#else
					PRINT("Setting key to %lli\n", size);
#endif
					
					// The map is based on _filename's_, not session ID's.
					// And as filename's can be duplicate, we use a multimap
					WFIPair pair = MakePair(filename, info);
					fFileList.insert(fFileList.end(), pair);
				}
			}
		}
		
	}
	fSearchLock.unlock();
	SetResultsMessage();
}

void
WinShareWindow::RemoveFile(const QString &sid, const QString &filename)
{
	fSearchLock.lock();
	PRINT("WSearch::RemoveFile\n");
	// go through our multi map and find the item that matches the sid and filename
	WFIIter iter = fFileList.begin();
	WFileInfo * info;

#ifdef _DEBUG
	WString wSID(sid);
	WString wFilename(filename);
	PRINT("Sid = %S, filename = %S\n", wSID.getBuffer(), wFilename.getBuffer());
#endif

	while (iter != fFileList.end())
	{
		info = (*iter).second;
		if (info->fiFilename == filename && info->fiUser()->GetUserID() == sid)	// we found it
		{
			fQueryBytes -= info->fiSize;
			delete info->fiListItem;	// remove it from the list view
			delete info;
			info = NULL; // <postmaster@raasu.org> 20021027

			fFileList.erase(iter);
			break;	// break from the loop
		}
		// not found, continue looking
		iter++;
	}
	fSearchLock.unlock();
	SetResultsMessage();
}

void
WinShareWindow::StopSearch()
{
	fSearchLock.lock();
	if (fCurrentSearchPattern != "")	// we actually have an old search pattern,
	{
		// cancel it
		// fCurrentSearchPattern contains a fully formatted subscription string
		fNetClient->RemoveSubscription(fCurrentSearchPattern);
		// also dump pending results
		MessageRef cancel(GetMessageFromPool(PR_COMMAND_JETTISONRESULTS));
		if (cancel())
		{
			// since "fCurrentSearchPattern" is in a /*/*/beshare/fi*es/*
			// pattern, if I grab the third '/' and add one, that is my
			// cancel PR_NAME_KEYS
			// <postmaster@raasu.org> 20021023 -- Use temporary variable to help debugging
			const char * cancelStr = strchr(fCurrentSearchPattern.Cstr(), '/') + 1;
			cancelStr = strchr(cancelStr, '/') + 1;
			cancelStr = strchr(cancelStr, '/') + 1;
			cancel()->AddString(PR_NAME_KEYS, cancelStr);
			fNetClient->SendMessageToSessions(cancel);
		}

		fCurrentSearchPattern = "";
		fFileRegExp.SetPattern("");
		fUserRegExp.SetPattern("");
	}
	fSearchLock.unlock();
	SetSearchStatus(tr("Idle."));
	SetSearchStatus("", 2);
}

// This method locks the list, so be sure the mutex is unlocked before calling it!
void
WinShareWindow::ClearList()
{
	fSearchLock.lock();
	// go through and empty the list
	WFIIter it = fFileList.begin();
	while (it != fFileList.end())
	{
		WFileInfo * info = (*it).second;
		// don't delete the list items here
		delete info;
		info = NULL; // <postmaster@raasu.org> 20021027
		fFileList.erase(it);
		it = fFileList.begin();
	}
	// delete them NOW
	fSearchList->clear();
	fQueryBytes = 0;
	fSearchLock.unlock();
	SetSearchStatus("", 1);
}

int
WinShareWindow::SplitQuery(const String &fileExp)
{
	// Test if all but one @ is escaped (don't change string length)
	{
		String temp(fileExp);
		temp.Replace("\\@", "??");
		if (temp.GetNumInstancesOf("@") == 1)
		{
			return temp.IndexOf("@");
		}
	}
	//
	WUserMap &users = fNetClient->Users();
	WUserIter it = users.begin();
	if (fileExp.StartsWith("*@"))
	{
		return 1;
	}
	if (fileExp.EndsWith("@*"))
	{
		return fileExp.LastIndexOf("@");
	}
	while (it != users.end())
	{	
		// User ID?
		String user((const char *) (*it).first.utf8());
		user = user.Prepend("@");
		if (fileExp.EndsWith(user))
		{
			return fileExp.LastIndexOf(user);
		}
		// User Name?
		QString name = (*it).second()->GetUserName().lower();
		name = StripURL(name);

		// Convert fileExp to Unicode and try to compare end of it against stripped user name
		
		QString temp = QString::fromUtf8(fileExp.Cstr());
		if (temp.right(name.length()).lower() == name)
		{
			temp.truncate(temp.length() - name.length());
			if (temp.right(1) == "@")
			{
				String tmp((const char *) temp.utf8());
				return tmp.Length() - 1;
			}
		}
		it++;
	}
	// No exact match?
	return fileExp.LastIndexOf("@");
}

void
WinShareWindow::GoSearch()
{
	// cancel current search (if there is one)
	StopSearch();	// these methods lock
	ClearList();

	QString pattern = fSearchEdit->currentText().stripWhiteSpace();
	if (pattern.isEmpty())	// no search string
		return;

	if (fNetClient->IsConnected() == false)
	{
		fStatus->message(tr("Not connected."));
		return;
	}

	// here we go with the new search pattern

	// now we lock
	fSearchLock.lock();

	// parse the string for the '@' if it exists
	String fileExp(pattern.utf8());
	String userExp;

	fileExp = fileExp.Trim();
	int32 atIndex = SplitQuery(fileExp);
	if (atIndex >= 0)
	{
		if ((uint32)atIndex < fileExp.Length())
		{
			userExp = fileExp.Substring(atIndex + 1);

			if (!HasRegexTokens(userExp.Cstr()))
			{
				bool nonNumericFound = false;
				const char * check = userExp.Cstr();
				while (*check)
				{
					if ((*check != ',') && ((*check < '0') || (*check > '9')))
					{
						nonNumericFound = true;
						break;
					}
					check++;
				}
				if (nonNumericFound)
					userExp = userExp.Prepend("*").Append("*");
			}
			ConvertToRegex(userExp);
			MakeRegexCaseInsensitive(userExp);
			StringMatcher match(userExp.Cstr());
			WUserMap & users = fNetClient->Users();
			WUserIter it = users.begin();
			WUserRef user;
			String ulist;
			while (it != users.end())
			{
				user = (*it).second;
				String username = StripURL((const char *) user()->GetUserName().utf8());
				String userid = (const char *) user()->GetUserID().utf8();
				if (match.Match(username.Cstr()))
				{
					AddToList(ulist, userid);
				}
				else if (match.Match(userid.Cstr()))
				{
					AddToList(ulist, userid);
				}
				it++;
			}
			if (ulist.Length() == 0)
			{
				SetSearchStatus(tr("User(s) not found!"));
				fSearchLock.unlock();
				return;
			}
			userExp	= ulist;
		}

		if (atIndex > 0)
		{
			fileExp = fileExp.Substring(0, atIndex);
		}
		else
			fileExp = "";
	}

	if (!HasRegexTokens(fileExp.Cstr()) && (userExp.Length() > 0 || fileExp.Length() > 0))
		fileExp = fileExp.Prepend("*").Append("*");

	fileExp.Replace('/', '?');
	userExp.Replace('/', '?');

	ConvertToRegex(fileExp);
	MakeRegexCaseInsensitive(fileExp);
	
	fSearchLock.unlock();	// unlock before StartQuery();

	StartQuery(userExp.Length() > 0 ? userExp.Cstr() : "*", fileExp);
}

void
WinShareWindow::StartQuery(const String & sidRegExp, const String & fileRegExp)
{
	fSearchLock.lock();
	String tmp("SUBSCRIBE:/*/");
	tmp += sidRegExp;
	tmp += "/beshare/fi?es/";
	tmp += fileRegExp;
	fCurrentSearchPattern = tmp;
	// <postmaster@raasu.org> 20021023 -- Fixed typo

#ifdef _DEBUG
	WString wCurrentSearchPattern(fCurrentSearchPattern.Cstr());
	WString wSIDRegExp(sidRegExp.Cstr());
	WString wFileRegExp(fileRegExp.Cstr());
	PRINT("Current Search Pattern = %S, fUserRegExp = %S, fFileRegExp = %S\n", wCurrentSearchPattern.getBuffer(), wSIDRegExp.getBuffer(), wFileRegExp.getBuffer());
#endif

	fUserRegExp.SetPattern(sidRegExp);
	fUserRegExpStr = sidRegExp;
	fFileRegExp.SetPattern(fileRegExp);
	fFileRegExpStr = fileRegExp;

	if (!fGotResults)
	{
		SetSearchStatus(tr("Initializing..."));
		while (!fGotResults)
		{
			qApp->processEvents();
		}
	}

	fGotResults = false;

	fNetClient->AddSubscription(tmp); // <postmaster@raasu.org> 20021026
	// Test when initial results have been returned
	fNetClient->SendMessageToSessions(GetMessageFromPool(PR_COMMAND_PING)); 

	fSearchLock.unlock();

	SetSearchStatus(tr("Searching for: \"%1\".").arg(QString::fromUtf8(fileRegExp.Cstr())));
	SetSearchStatus(tr("active"), 2);
}

void
WinShareWindow::Download()
{
	fSearchLock.lock();
	fDownload->setEnabled(false);
	if (!fFileList.empty())
	{
		WFIIter it = fFileList.begin();
		while (it != fFileList.end())
		{
			WFileInfo * fi = (*it).second;

#ifdef _DEBUG
			WString wFile = fi->fiListItem->text(0);
			WString wUser = fi->fiListItem->text(5);
			PRINT("Checking: %S, %S\n", wFile.getBuffer(), wUser.getBuffer());
#endif

			if (fi->fiListItem->isSelected())
			{
				PRINT("DOWNLOAD: Found item\n");
				WUserRef user = fi->fiUser;
				
				QueueDownload(fi->fiFilename, user);
			}					
			it++;
		}
	}
	EmptyQueues();
	fDownload->setEnabled(true);
	fSearchLock.unlock();
}

void
WinShareWindow::SetResultsMessage()
{
	QString qmsg = tr("Results: %1").arg(fFileList.size());
	qmsg += " (";
	qmsg += MakeSizeString(fQueryBytes);
	qmsg += ")";
	SetSearchStatus(qmsg, 1);
}

void
WinShareWindow::SetSearchStatus(const QString & status, int index)
{
	fStatus->setText(status, index);
}

void
WinShareWindow::SetSearch(const QString & pattern)
{
	fTabs->showPage(fSearchWidget);
	// Is already on history?
	for (int i = 0; i < fSearchEdit->count(); i++)
	{
		if (fSearchEdit->text(i) == pattern)
		{
			fSearchEdit->setCurrentItem(i);
			GoSearch();
			return;
		}
	}
	fSearchEdit->insertItem(pattern, 0);
	fSearchEdit->setCurrentItem(0);
	GoSearch();
}

void
WinShareWindow::QueueDownload(const QString & file, const WUserRef & user)
{
	int32 i = 0;
	String mUser = (const char *) user()->GetUserID().utf8();
	if (fQueue()->FindInt32(mUser, &i) == B_OK)
		fQueue()->RemoveName(mUser);
	fQueue()->AddInt32(mUser, ++i);
	mUser = mUser.Prepend("_");
	fQueue()->AddString(mUser, (const char *) file.utf8());
}

void
WinShareWindow::EmptyQueues()
{
	String mUser, mFile;
	QString user;
	QString * files;
	WUserRef u;
	int32 numItems;
	MessageFieldNameIterator iter = fQueue()->GetFieldNameIterator(B_INT32_TYPE);
	while (iter.GetNextFieldName(mUser) == B_OK)
	{
		user = QString::fromUtf8(mUser.Cstr());
		u = FindUser(user);
		if (u() != NULL)
		{
			fQueue()->FindInt32(mUser, &numItems);
			files = new QString[numItems];
			CHECK_PTR(files);
			mUser = mUser.Prepend("_");
			for (int32 i = 0; i < numItems; i++)
			{
				fQueue()->FindString(mUser, i, mFile);
				files[i] = QString::fromUtf8(mFile.Cstr());
			}
			OpenDownload();
			if (u()->GetFirewalled() && u()->GetTunneling() && fSettings->GetFirewalled())
				fDLWindow->CreateTunnel(files, NULL, numItems, u);
			else
				fDLWindow->AddDownload(files, NULL, numItems, u()->GetUserID(), u()->GetPort(), u()->GetUserHostName(), u()->GetInstallID(), u()->GetFirewalled(), u()->GetPartial());
		}
	}
	fQueue()->Clear();
}


void
WinShareWindow::ClearHistory()
{
	fSearchEdit->clear();
}
