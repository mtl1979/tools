#ifndef USER_H
#define USER_H

#ifdef WIN32
#pragma warning(disable: 4786)
#endif

#include <map>
using std::map;
using std::pair;
using std::iterator;

#include "message/Message.h"
#include "util/RefCount.h"
using namespace muscle;

#include <qstring.h>

class QListView;
class QListViewItem;

/** A work in progress class
  *	As the user message becomes more revealed, add more
  * methods to it.
  */
class WUser : public RefCountable
{
public:
	WUser(QString sid);
	~WUser();

	// <postmaster@raasu.org> 20021001
	void SetUserName(QString name) {fUserName = name; }
	void SetStatus(QString s) {fUserStatus = s; } 
	void SetUserID(QString id) { fUserID = id; }
	void SetUserHostName(QString h) { fHostName = h; }
	
	void SetCurUploads(uint32 c) { fCurUploads = c; }
	void SetMaxUploads(uint32 m) { fMaxUploads = m; }
	void SetBandwidthLabel(const char * s) { fBandwidthLabel = s; }
	void SetBandwidthBPS(uint32 bps) { fBandwidthBPS = bps; }
	void SetFileCount(int32 fc) { fFileCount = fc; }
	void SetFirewalled(bool f);
	void SetClient(const char * c) { fClient = c; }
	void SetPartial(bool p) { fPartial = p; }

	QString GetUserName() const { return fUserName; }
	QString GetStatus() const { return fUserStatus; }
	QString GetUserID() const { return fUserID; }
	QString GetUserHostName() const { return fHostName; }

	uint32 GetCurUploads() const { return fCurUploads; }
	uint32 GetMaxUploads() const { return fMaxUploads; }
	QString GetBandwidthLabel() const { return fBandwidthLabel; }
	uint32 GetBandwidthBPS() const { return fBandwidthBPS; }
	int32 GetFileCount() const { return fFileCount; }
	bool GetFirewalled() const { return fFirewalled; }
	bool GetPartial() const { return fPartial; }

	QString GetClient() const { return fClient; }

	// check state only...
	bool IsBot() const { return fBot; }
	uint32 GetPort() const { return fPort; }
	uint64 GetInstallID() const { return fInstallID; }

	void InitName(const Message * msg);
	void InitStatus(const Message * msg);
	void InitUploadStats(const Message * msg);
	void InitBandwidth(const Message * msg);
	void InitFileCount(const Message * msg);
	
	void PingResponse(const Message * msg);
	void AddToListView(QListView * view);
	void RemoveFromListView(QListView * view = NULL);	// if NULL, remove from all list views

	bool NeedPing() const { return fNeedPing; }

private:
	QString fHostName;
	QString fUserID;
	QString fUserStatus;
	QString fUserName;

	uint32 fCurUploads, fMaxUploads;
	QString fBandwidthLabel;
	uint32 fBandwidthBPS;
	int32 fFileCount;
	bool fFirewalled;
	bool fPartial;

	QString fClient;

	bool fBot;
	uint32 fPort;
	uint64 fInstallID;

	bool fNeedPing;

	typedef map<QListView *, QListViewItem *> WListMap;	// holds a list of QListView/QListViewItem pairs
	typedef WListMap::iterator WListIter;
	typedef pair<QListView *, QListViewItem *> WListPair;

	WListMap fLists;

	// private inline method for generating a pair<>
	WListPair 
	MakeListPair(QListView * v, QListViewItem * i)
	{
		WListPair p;
		p.first = v;
		p.second = i;
		return p;
	}

};

typedef Ref<WUser> WUserRef;

/** Used for name matching for /ping's and /msg's etc
  */
typedef struct
{
	WUserRef user;
	QString text;
} WUserSearch;


/** The map is in the following format:
  *	The key QString, is /host/sessionid
  * The value is its WUser clas
  */
typedef map<QString, WUserRef> WUserMap;
typedef pair<QString, WUserRef> WUserPair;
typedef WUserMap::iterator WUserIter;

typedef map<QString, WUserSearch> WUserSearchMap;
typedef pair<QString, WUserSearch> WUserSearchPair;
typedef WUserSearchMap::iterator WUserSearchIter;

inline WUserPair
MakePair(QString s, WUserRef & w)
{
	WUserPair p;
	p.first = s;
	p.second = w;
	return p;
}

inline WUserSearchPair
MakePair(QString s, WUserRef & w, QString txt)
{
	WUserSearch wus;
	wus.user = w;
	wus.text = txt;

	WUserSearchPair p;
	p.first = s;
	p.second = wus;
	return p;
}



#endif

