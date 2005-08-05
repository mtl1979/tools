#ifdef WIN32
#pragma warning(disable: 4786)
#endif

#include "util.h"
#include "tokenizer.h"
#include "formatting.h"
#include "global.h"
#include "winsharewindow.h"
#include "settings.h"
#include "wfile.h"
#include "wstring.h"

#include "util/Queue.h"
#include "util/StringTokenizer.h"
using namespace muscle;

#include <qregexp.h>
#include <qapplication.h>
#include <qdns.h>
#include <qfile.h>
#include <qstringlist.h>

QString
ParseChatText(const QString & str)
{
	// <postmaster@raasu.org> 20021106,20021114 -- Added support for URL labels, uses QStringTokenizer now ;)
	Queue<QString> qUrls;
	Queue<QString> qLabels;  // in this list, "" means no label
	
	QString qText = str;
	bool lastWasURL = false, inLabel = false;
	QStringTokenizer qTok(qText, " \t\n");
	QString qToken;
	
	while ((qToken = qTok.GetNextToken()) != QString::null)
	{
		bool bInTag = false;	// <postmaster@raasu.org> 20021012,20021114
		
		if (inLabel)			// label contains space(s) ???
		{
			if (qToken.right(1) == "]")
			{
				qLabels.Tail() += qToken.left(qToken.length() - 1);
				inLabel = false;
			}
			else if (qToken.find("]") >= 0)
			{
				qLabels.Tail() += qToken.left(qToken.find("]") - 1);
				inLabel = false;
			}
			else 
				qLabels.Tail() += qToken + " ";
		} 
		else if (IsURL(qToken))
		{
			if (
				(qToken.lower().startsWith("beshare:") == false) && 
				(qToken.lower().startsWith("share:") == false)
				)
			{
				while (qToken.length() > 1)
				{
					QCharRef last = qToken.at(qToken.length() - 1);
					
					// <postmaster@raasu.org> 20021012,20021114,20030203
					//

					if (last == '>')					
					{
						bInTag = true;

						// Skip html tags that are just after the url
						//
						while (bInTag)
						{
							if (qToken.right(1) == "<")
								bInTag = false;

							qToken.truncate(qToken.length() - 1);

							if (qToken.right(1) == ">") // another tag?
								bInTag = true;
						}

						last = qToken.at(qToken.length() - 1);
					}

					// <postmaster@raasu.org> Apr 11th 2004 
					// Make sure there is same amount of ( and ) characters
					//

					if (qToken.right(1) == ")")
					{
						if (qToken.contains("(") == qToken.contains(")"))
							break;
					}

					if 	(
						muscleInRange(last.unicode(), (unichar) '0', (unichar) '9') ||
						muscleInRange(last.unicode(), (unichar) 'a', (unichar) 'z') ||
						muscleInRange(last.unicode(), (unichar) 'A', (unichar) 'Z') ||
						(last == '/')
						)
					{
						break;
					}
					else
						qToken.truncate(qToken.length() - 1);
				}
			}
			else
			{
				// Remove html tag after the url...
				if (qToken.right(1) == ">")
				{
					bool bInTag = true;
					while (bInTag)
					{
						if (qToken.right(1) == "<")
							bInTag = false;
						qToken.truncate(qToken.length() - 1);
						if (qToken.right(1) == ">") // another tag?
							bInTag = true;
					}
				}
				// ...and ensure that URL doesn't end with a dot, comma or colon
				bool cont = true;
				while (!qToken.isEmpty() && cont)
				{
					unsigned int pos = qToken.length() - 1;
					switch ((QChar) qToken.at(pos))
					{
					case '.':
					case ',':
					case ':':
							qToken.truncate(pos);
							break;
					default:
							cont = false;
							break;
					}
				}
			}
			if (IsURL(qToken))
			{
				qUrls.AddTail(qToken);
				qLabels.AddTail("");
				lastWasURL = true;
			}
		}
		else if (lastWasURL)
		{
			lastWasURL = false; // clear in all cases, might contain trash between url and possible label
			
			if (qToken.startsWith("[")) // Start of label?
			{
				if (qToken.right(1) == "]") 
					qLabels.Tail() = qToken.mid(1, qToken.length() - 2);
				else if (qToken.find("]") >= 0)
					qLabels.Tail() = qToken.mid(1, qToken.find("]") - 1);
				else
				{
					qLabels.Tail() += qToken.mid(1) + " ";
					inLabel = true;
				}
			}
		}
	}
	
	if (inLabel) 
		qText += "]";
	
	if (qUrls.GetNumItems() > 0)
	{
		QString output = QString::null;
		
		QString qUrl;
		QString qLabel;
		
		while ((qUrls.RemoveHead(qUrl) == B_OK) && (qLabels.RemoveHead(qLabel) == B_OK))
		{
			int urlIndex = qText.find(qUrl); // position in QString
			
			if (urlIndex > 0) // Not in start of string?
			{
				output += qText.left(urlIndex);
				qText = qText.mid(urlIndex);
			}
			
			// now the url...
			QString urltmp = "<a href=\"";
			if ( qUrl.startsWith("www.") )		urltmp += "http://";
			if ( qUrl.startsWith("ftp.") )		urltmp += "ftp://";
			if ( qUrl.startsWith("beshare.") )	urltmp += "server://";
			if ( qUrl.startsWith("irc.") )		urltmp += "irc://";
			urltmp += qUrl;
			urltmp += "\">";
			// Display URL label or link address, if label doesn't exist
			int lb = qText.find("\n"); // check for \n between url and label (Not allowed!!!)
			int le = qText.find(qLabel);
			if ( qLabel.isEmpty() || muscleInRange(lb, 0, le) )
				urltmp += qUrl; 		
			else
				urltmp += qLabel.stripWhiteSpace(); // remove surrounding spaces before adding
			urltmp += "</a>";
			QString urlfmt = WFormat::URL(urltmp);
			output += urlfmt;
			// strip url from original text
			qText = qText.mid(qUrl.length());
			// strip label from original text, if exists
			if (!qLabel.isEmpty())
			{
				lb = qText.find("\n");
				le = qText.find("]");
				if (!muscleInRange(lb, 0, le))
				{
					qText = qText.mid(le + 1);
					if (qText.left(1) == "]")	// Fix for ']' in end of label
						qText = qText.mid(1);
				}
			}
			
		}
		// Still text left?
		if (!qText.isEmpty())
			output += qText;
		
		return output;		// <postmaster@raasu.org> 20021107,20021114 -- Return modified string
	}
	else
		return str; 		// <postmaster@raasu.org> 20021107 -- Return unmodified 
}

void
ParseString(QString & str)
{
	bool space = true;
	bool first = false; // make first always be &nbsp;

	// Remove trailing line feeds
	while (str.right(1) == "\n")
		str.truncate(str.length() - 1);

	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ')
		{
			if (space)
			{
				// alternate inserting non-breaking space and real space
				if (first)
					first = false;
				else 
				{
					str.replace(i, 1, "&nbsp;");
					i += 5;
					first = true;
				}
			}
		}
		else if (str[i] == '<')
			space = false;
		else if (str[i] == '>')
			space = true;
		else if (str[i] == '\n')
		{
			// change newlines to <br> (html)
			if (space)
			{
				str.replace(i, 1, "<br>");
				i += 3;
			}
		}
		else if (str[i] == '\t')
		{
			if (space)
			{
				// <postmaster@raasu.org> 20030623 -- follow 'first' used in spaces here too...
				if (first)
					str.replace(i, 1, " &nbsp; &nbsp;");
				else
					str.replace(i, 1, "&nbsp; &nbsp; ");

				i += 12;
			}
		}
		else
		{	
			// other character
			first = true;	
		}
	}
}

QString
ParseStringStr(const QString & str)
{
	QString s = str;
	ParseString(s);
	return s;
}

void
EscapeHTML(QString & str)
{
	// we don't want to show html...
	str.replace(QRegExp("<"), "&lt;");
	str.replace(QRegExp(">"), "&gt;");
}

QString
EscapeHTMLStr(const QString & str)
{
	QString s = str;
	EscapeHTML(s);
	return s;
}

QString
FixStringStr(const QString & str)
{
	QString s = str;
	EscapeHTML(s);
	s = ParseChatText(s);
	ParseString(s);
	return s;
}

void
FixString(QString & str)
{
	EscapeHTML(str);
	str = ParseChatText(str);
	ParseString(str);
}

QString 
GetParameterString(const QString & qCommand)
{
	QString qParameters;
	int sPos = qCommand.find(" ")+1;
	if (sPos > 0)
	{
		while (qCommand[sPos].unicode() == 10) 
			sPos++;
		qParameters = qCommand.mid(sPos);
	}
	return qParameters;
}

// <postmaster@raasu.org> 20021103 -- Added GetCommandString() and CompareCommand()
//

QString 
GetCommandString(const QString & qCommand)
{
	QString qCommand2 = qCommand.lower();
	int sPos = qCommand2.find(" ");  // parameters should follow after <space> so they should be stripped off
	int sPos2 = qCommand2.find("/"); // Is / first letter?
	if ((sPos > 0) && (sPos2 == 0))
	{
		qCommand2.truncate(sPos);
	}
	return qCommand2;
}

bool 
CompareCommand(const QString & qCommand, const QString & cCommand)
{
	QString com = GetCommandString(qCommand);
#ifdef DEBUG2
	WString c1(com);
	WString c2(cCommand);
	PRINT2("Compare String: qCommand=\'%S\'\n", c1.getBuffer());
	PRINT2("                cCommand=\'%S\'\n", c2.getBuffer());
#endif
	return ((com == cCommand) ? true : false);
}

String
StripURL(const String & strip)
{
	int sp;
	if (IsURL(strip))
	{
		sp = strip.IndexOf(" ");

		while (strip.Substring(sp + 1, sp + 2) == " ")
			sp++;
		
		if (sp > 0)
		{
			int left = strip.IndexOf('[');	// see if it contains a label..
			if (left == (sp + 1))
			{
				left++;
				int right = strip.IndexOf(']');
				if (right > left)	// make sure right is than greater left :D
				{
					String label = strip.Substring(left, right).Trim();
					if ((right + 1) < (int) strip.Length())
					{
						String rest = strip.Substring(right + 1);
						if (rest.StartsWith(" "))
						{
							label += " ";
							rest = rest.Substring(1);
						}
						return label + StripURL(rest);
					}
					else
						return label;
				}
				else if (right == -1) // ']' is missing?
				{
					String label = strip.Substring(left).Trim();
					return label;
				}
			}
		}
	}
	// Recurse ;)
	sp = strip.IndexOf(" ");
	if (sp > 0)
	{
		String s1 = strip.Substring(0,sp+1); // include space in s1 ;)
		String s2;
		if ((sp + 1) < (int) strip.Length())
		{
			s2 = StripURL(strip.Substring(sp+1));
			return s1 + s2;
		}
		else
			return s1;
	}
	else
		return strip;	// not a url
}

QString
StripURL(const QString & u)
{
	int sp;
	if (IsURL(u))
	{
		sp = u.find(" ");

		while (u.at(sp + 1) == ' ')
			sp++;

		if (sp > 0)
		{
			int left = u.find('[');	// see if it contains a label..
			if (left == (sp + 1))
			{
				left++;
				int right = u.find(']');
				if (right > left)	// make sure right is than greater left :D
				{
					QString label = u.mid(left, (right - left)).stripWhiteSpace();
					if ((right + 1) < (int) u.length())
					{
						QString rest = u.mid(right + 1);
						if (rest.startsWith(" "))
						{
							label += " ";
							rest = rest.mid(1);
						}
						return label + StripURL(rest);
					}
					else
						return label;
				}
				else if (right == -1) // ']' is missing?
				{
					QString label = u.mid(left).stripWhiteSpace();
					return label;
				}
			}
		}
	}
	// Recurse ;)
	sp = u.find(" ");
	if (sp > 0)
	{
		QString s1 = u.left(sp+1); // include space in s1 ;)
		QString s2;
		if ((sp + 1) < (int) u.length())
		{
			s2 = StripURL(u.mid(sp+1));
			return s1 + s2;
		}
		else
			return s1;
	}
	else
		return u;	// not a url
}

String 
StripURL(const char * c)
{
	String s(c);
	return StripURL(s);
}

// URL Prefixes

const char * urlPrefix[] = {
	"file://",
	"http://",
	"https://",
	"mailto:",
	"ftp://",
	"audio://",
	"mms://",
	"h323://",
	"callto://",
	"beshare:",		// BeShare Search
	"share:",		//    ----"----
	"server://",	// BeShare Server
	"priv:",		// BeShare Private Chat
	"irc://",		// Internet Relay Chat
	"ttp://",		// Titanic Transfer Protocol
	"ed2k:",		// eDonkey2000
	"magnet:",		// Magnet
	"gnutella:",	// Gnutella
	"mp2p:",		// Piolet
	NULL
};

bool
IsURL(const String & url)
{
	QString u = QString::fromUtf8( url.Cstr() );
	return IsURL( u );
}

bool 
IsURL(const char * url)
{
	QString u = QString::fromUtf8(url);
	return IsURL( u );
}

bool
IsURL(const QString & url)
{
	QString u = url.lower();

	// Add default protocol prefixes

	if (u.startsWith("www.") && !u.startsWith("www.."))		
		u.prepend("http://");
	if (u.startsWith("ftp.") && !u.startsWith("ftp.."))		
		u.prepend("ftp://");
	if ((u.startsWith("beshare.") && !u.startsWith("beshare..")) && u.length() > 12)	
		u.prepend("server://");
	if (u.startsWith("irc.") && !u.startsWith("irc.."))		
		u.prepend("irc://");

	if (u.length() > 9)
	{
		const char *prefix;
		for (unsigned int i = 0; (prefix = urlPrefix[i]) != NULL; i++)
		{
			if (u.startsWith(prefix))
			{
				if (u.right(3) != "://")
				{
					return true;
				}
			}
		}
	}
	return false;
}

QString 
MakeSizeString(uint64 s)
{
	QString result, postFix;
	double n = (int64) s;
	postFix = "B";
	if (n > 1024.0f)	// > 1 kB?
	{
		n /= 1024.0f;
		postFix = "kB"; // we're in kilobytes now, <postmaster@raasu.org> 20021024 KB -> kB
		
		if (n > 1024.0f)	// > 1 MB?
		{
			n /= 1024.0f;
			postFix = "MB";
			
			if (n > 1024.0f)	// > 1 GB?
			{
				n /= 1024.0f;
				postFix = "GB";
			}
		}
	}
	result.sprintf("%.2f ", n);
	result += postFix;
	return result;	
}

struct ConPair
{
	const char *id;
	uint32 bw;
};

ConPair Bandwidths[] = {
	{QT_TRANSLATE_NOOP("Connection", "300 baud"),	75},		// 300 down / 75 up
	{QT_TRANSLATE_NOOP("Connection", "14.4 kbps"),	14400},
	{QT_TRANSLATE_NOOP("Connection", "28.8 kbps"),	28800},
	{QT_TRANSLATE_NOOP("Connection", "33.6 kbps"),	33600},
	{QT_TRANSLATE_NOOP("Connection", "36.6 kbps"),	33600},		// Misspelled 33.6 kbps
	{QT_TRANSLATE_NOOP("Connection", "57.6 kbps"),	57600},
	{QT_TRANSLATE_NOOP("Connection", "ISDN-64k"),	64000},
	{QT_TRANSLATE_NOOP("Connection", "ISDN-128k"),	128000},
	{QT_TRANSLATE_NOOP("Connection", "DSL-256k"),	256000},
	{QT_TRANSLATE_NOOP("Connection", "DSL"),		384000},
	{QT_TRANSLATE_NOOP("Connection", "DSL-384k"),	384000},
	{QT_TRANSLATE_NOOP("Connection", "DSL-512k"),	512000},
	{QT_TRANSLATE_NOOP("Connection", "Cable"),		768000},
	{QT_TRANSLATE_NOOP("Connection", "DSL-1M"),		1024000},	// Was 1000000
	{QT_TRANSLATE_NOOP("Connection", "T1"),			1500000},
	{QT_TRANSLATE_NOOP("Connection", "T3"),			4500000},
	{QT_TRANSLATE_NOOP("Connection", "OC-3"),		155520000}, // 3  * 51840000
	{QT_TRANSLATE_NOOP("Connection", "OC-12"),		622080000}, // 12 * 51840000
	{"?", 0},													// Dummy entry
	{NULL, ULONG_MAX}
};

uint32
BandwidthToBytes(const QString & connection)
{
	uint32 bps = 0;
	int p = connection.find(",");
	QString conn;
	if (p > 0)
	{
		conn = connection.left(p);
		QString spd = connection.mid(p + 1);
		bps = spd.toULong();
	}
	else
	{
		conn = connection;
	}
	if (bps == 0)
	{
		ConPair bw;
		int n = 0;
		do
		{
			bw = Bandwidths[n++];
			if (
				( conn == bw.id ) || 
				( conn == qApp->translate("Connection", bw.id) )
				)
			{
				bps = bw.bw;
				break;
			}
		} while (bw.bw != ULONG_MAX);
	}
#ifdef DEBUG2
	WString wconn(conn);
	PRINT2("Connection = '%S', bps = %lu\n", wconn.getBuffer(), bps);
#endif
	return bps;
}

QString
BandwidthToString(uint32 bps)
{
	switch (bps)
	{
	case 75:		
	case 300:			
		return qApp->translate("Connection", "300 baud");
	case 14400: 		
		return qApp->translate("Connection", "14.4 kbps");
	case 28800: 		
		return qApp->translate("Connection", "28.8 kbps");
	case 33600: 		
		return qApp->translate("Connection", "33.6 kbps");
	case 57600: 		
		return qApp->translate("Connection", "57.6 kbps");
	case 64000: 		
		return qApp->translate("Connection", "ISDN-64k");
	case 128000:		
		return qApp->translate("Connection", "ISDN-128k");
	case 256000:		
		return qApp->translate("Connection", "DSL-256k");
	case 384000:		
		return qApp->translate("Connection", "DSL-384k");
	case 512000:		
		return qApp->translate("Connection", "DSL-512k");
	case 768000:		
		return qApp->translate("Connection", "Cable");
	case 1000000:		
	case 1024000:		
		return qApp->translate("Connection", "DSL-1M");
	case 1500000:		
		return qApp->translate("Connection", "T1");
	case 4500000:		
		return qApp->translate("Connection", "T3");
	case 155520000:	
		return qApp->translate("Connection", "OC-3");
	case 622080000: 
		return qApp->translate("Connection", "OC-12");
	default:			
		return qApp->translate("Connection", "Unknown");
	}
}

QString
GetServerName(const QString & server)
{
	int pColon = server.find(":");
	if (pColon >= 0)
	{
		return server.left(pColon);
	}
	else
	{
		return server;
	}
}

uint16
GetServerPort(const QString & server)
{
	int pColon = server.find(":");
	if (pColon >= 0)
	{
		return server.mid(pColon+1).toUShort();
	}
	else
	{
		return 2960;
	}
}

// Is reserved regex token, but isn't wildcard token (* or ? or ,)
// Skip \ now, it's a special case
bool IsRegexToken2(QChar c, bool isFirstCharInString)
{
   switch(c)
   {
      case '[': case ']': case '|': case '(': case ')': case '=': case '^': case '+': case '$': case '{':  case '}': 
	  case ':': case '-': case '.':
        return true;

      case '<': case '~':   // these chars are only special if they are the first character in the string
         return isFirstCharInString; 
 
      default:
         return false;
   }
}

bool IsRegexToken2(char c, bool isFirstCharInString)
{
	return IsRegexToken2(QChar(c), isFirstCharInString);
}

// Converts basic wildcard pattern to valid regex
void ConvertToRegex(String & s)
{
	const char * str = s.Cstr();
	
	String ret;
	
	bool isFirst = true;
	while(*str)
	{
		if (*str == '\\')			// skip \c
		{
			const char * n = (str + 1);
			if (n)
			{
				if (*n != '@')	// convert \@ to @
					ret += *str;
				str++;
				ret += *n;
				str++;
			}
		}
		else
		{
			if (IsRegexToken2(*str, false)) ret += '\\';
			ret += *str;
			str++;
		}

		// reset
		if (isFirst)
			isFirst = false;
	}
	s = ret;
}

void ConvertToRegex(QString & s, bool simple)
{
	unsigned int x = 0;	
	QString ret;
	
	bool isFirst = true;
	while(x < s.length())
	{
		if (s[x] == '\\')			// skip \c
		{
			if (x + 1 < s.length())
			{
				if (s[x + 1] != '@')	// convert \@ to @
					ret += s[x];
				x++;
				ret += s[x];
			}
		}
		else if (s[x] == '*')
		{
			ret += simple ? "*" : ".*";
		}
		else if (s[x] == '?')
		{
			ret += simple ? "?" : ".";
		}
		else if (s[x] == ',')
		{
			ret += "|";
		}
		else
		{
			if (IsRegexToken2(s[x], false)) ret += '\\';
			ret += s[x];
		}

		x++;
		// reset
		if (isFirst)
			isFirst = false;
	}
	s = ret;
}

bool HasRegexTokens(const QString & str)
{
   bool isFirst = true;
   unsigned int x = 0;
   while(x < str.length())
   {
      if (IsRegexToken2(str[x], isFirst)) return true;
	  else if (str[x] == "*") return true;
	  else if (str[x] == "?") return true;
      else 
      {
         x++;
         isFirst = false;
      }
   }
   return false;
}

const char * MonthNames[12] = {
								QT_TRANSLATE_NOOP( "Date", "Jan" ),
								QT_TRANSLATE_NOOP( "Date", "Feb" ),
								QT_TRANSLATE_NOOP( "Date", "Mar" ),
								QT_TRANSLATE_NOOP( "Date", "Apr" ),
								QT_TRANSLATE_NOOP( "Date", "May" ),
								QT_TRANSLATE_NOOP( "Date", "Jun" ),
								QT_TRANSLATE_NOOP( "Date", "Jul" ),
								QT_TRANSLATE_NOOP( "Date", "Aug" ),
								QT_TRANSLATE_NOOP( "Date", "Sep" ),
								QT_TRANSLATE_NOOP( "Date", "Oct" ),
								QT_TRANSLATE_NOOP( "Date", "Nov" ),
								QT_TRANSLATE_NOOP( "Date", "Dec" )
};

QString TranslateMonth(const QString & m)
{
	return qApp->translate("Date", m.local8Bit());
}

const char * DayNames[7] = {
								QT_TRANSLATE_NOOP( "Date", "Mon" ),
								QT_TRANSLATE_NOOP( "Date", "Tue" ),
								QT_TRANSLATE_NOOP( "Date", "Wed" ),
								QT_TRANSLATE_NOOP( "Date", "Thu" ),
								QT_TRANSLATE_NOOP( "Date", "Fri" ),
								QT_TRANSLATE_NOOP( "Date", "Sat" ),
								QT_TRANSLATE_NOOP( "Date", "Sun" )
};

QString TranslateDay(const QString & d)
{
	return qApp->translate("Date", d.local8Bit());
}

QString
InitTimeStamp()
{
	time_t currentTime = time(NULL);
	return QString::fromLocal8Bit( ctime(&currentTime) );
}

QString
GetTimeStampAux(const QString &stamp)
{
	QString qCurTime;

	qCurTime = stamp;
	qCurTime = qCurTime.left(qCurTime.findRev(" ", -1));
	qCurTime = qCurTime.mid(qCurTime.findRev(" ", -1) + 1);
	
	return qCurTime;
}

QString
GetDateStampAux(const QString &stamp)
{
	QString qCurTime = stamp;

	// Strip off year
	QString qYear = qCurTime.mid(qCurTime.findRev(" ") + 1);
	qYear.truncate(4);
	qCurTime.truncate(qCurTime.findRev(" "));			
	
	// ... and day of week
	QString qDOW = qCurTime.left(qCurTime.find(" "));
	qDOW = TranslateDay(qDOW);
	qCurTime = qCurTime.mid(qCurTime.find(" ") + 1);	
	
	// Strip Month and translate it
	QString qMonth = qCurTime.left(qCurTime.find(" "));
	qMonth = TranslateMonth(qMonth);
	
	qCurTime = qCurTime.mid(qCurTime.find(" ") + 1);

	// Strip Day
	QString qDay = qCurTime.left(qCurTime.find(" "));
	return qDOW + " " + qMonth + " " + qDay + " " + qYear;
}

QString
GetTimeStamp2()
{
	QString stamp = InitTimeStamp();
	return GetDateStampAux(stamp) + " " + GetTimeStampAux(stamp);
}

QString
GetTimeStamp()
{
	static QString _day = QString::null;
	
	QString ret;
	QString qCurTime;
	// Is this first time today?

	QString stamp = InitTimeStamp();
	QString qDate = GetDateStampAux(stamp);
	if (qDate != _day)
	{
		_day = qDate;
		qDate.prepend(" ");
		qDate.prepend(qApp->translate("Date", "Date:"));
		ret = WFormat::TimeStamp(qDate);
		ret += "<br>";
	}
	
	qCurTime = GetTimeStampAux(stamp);
	qCurTime.prepend("[").append("] ");

	ret += WFormat::TimeStamp(qCurTime);
	return ret;
}

QString
ComputePercentString(int64 cur, int64 max)
{
	QString ret;
	double p = 0.0f;
	
	if ( (cur > 0) && (max > 0) )
	{
		p = ((double)cur / (double)max) * 100.0f;
	}
	
	ret.sprintf("%.2f", p);
	return ret;
}


void
Reverse(QString &text)
{
	int start = 0;
	int end = text.length() - 1;
	while (start < end)
	{
		QChar c = text[end];
		text[end] = text[start];
		text[start] = c;
		start++;
		end--;
	}
}

void MakeNodePath(String &file)
{
	if (gWin->fSettings)
	{
		if (gWin->fSettings->GetFirewalled())
		{
			file = file.Prepend("fires/");
		}
		else
		{
			file = file.Prepend("files/");
		}
		file = file.Prepend("beshare/");
	}
}

String MakePath(const String &dir, const String &file)
{
	String ret(dir);
	if (!ret.EndsWith("/"))
		ret += "/";
				
	ret += file;
	
	return ret; 
}

QString MakePath(const QString &dir, const QString &file)
{
	QString ret(dir);
	if (ret.right(1) != "/")
		ret += "/";

	ret += file;

	return ret;
}

/*
 * Is received text action text or normal text
 *
 */

bool
IsAction(const QString &text, const QString &user)
{
	bool ret = false;

	if (text.startsWith(user + " "))
		ret = true;

	if (text.startsWith(user + "'s "))
		ret = true;

	return ret;
}


QString 
FixFileName(const QString & fixMe)
{
	// bad characters in Windows:
	//	/, \, :, *, ?, ", <, >, |
#ifdef WIN32
	QString ret(fixMe);
	for (unsigned int i = 0; i < ret.length(); i++)
	{
		switch ((QChar) ret.at(i))
		{
		case '/':
		case '\\':
		case ':':
		case '*':
		case '?':
		case '\"':
		case '<':
		case '>':
		case '|':
			ret.replace(i, 1, "_");
			break;
		}
	}
	return ret;
#else
	return fixMe;
#endif
}

const QString &
CheckIfEmpty(const QString & str, const QString & str2)
{
	if (str.isEmpty())
	{
		return str2;
	}
	else
	{
		return str;
	}
}

uint32
CalculateChecksum(const uint8 * data, size_t bufSize)
{
   uint32 sum = 0L;
   for (size_t i=0; i<bufSize; i++) sum += (*(data++)<<(i%24));
   return sum;
}

uint32
GetHostByName(const QString &name)
{
	return GetHostByName((const char *) name.local8Bit());
}

QString
UniqueName(const QString & file, int index)
{
	QString tmp, base, ext;
	int sp = file.findRev("/", -1); // Find last /
	if (sp > -1)
	{
		tmp = file.left(sp + 1);			// include slash
		base = file.mid(sp + 1);			// filename
		QString out(tmp);
		out += QString::number(index);
		out += " ";
		out += base;
		return out;
	}
	WASSERT(true, "Invalid download path!");
	return QString::null;
}

void
SavePicture(QString &file, const ByteBufferRef &buf)
{
	int n = 1;
	QString path("downloads/");
	path += FixFileName(file);
	QString nf = path;
	while (WFile::Exists(nf)) 
	{
		nf = UniqueName(path, n++);
	}
	WFile fFile;
	if (fFile.Open(nf, IO_WriteOnly))
	{
		uint64 bytes = fFile.WriteBlock((char *) buf()->GetBuffer(), buf()->GetNumBytes());
		fFile.Close();
		if (bytes == buf()->GetNumBytes())
		{
			file = nf;
			return;
		}
	}
	file = QString::null;
}

void CloseFile(WFile * & file)
{
	if (file)
	{
		file->Close();
		delete file;
		file = NULL;
	}
}

uint64 
toULongLong(const QString &in, bool *ok)
{
	uint64 out = 0;
	bool o = true;
	for (unsigned int x = 0; x < in.length(); x++)
	{
		QChar c = in.at(x);
		if (!muscleInRange(c.unicode(), (unichar) '0', (unichar) '9'))
		{
			o = false;
			break;
		}
		out *= 10;
		out += c - '0';
	}
	if (ok)
		*ok = o;
	return out;
}

QString 
fromULongLong(const uint64 &in)
{
	uint64 tmp;
	int n;
	QString out;

	if (in < 10)
		return QString(QChar(((int) in) + '0'));
	
	tmp = in;
	while (tmp > 0)
	{
		n = tmp % 10;
		out.prepend(QChar(n + '0'));
		tmp /= 10;
	}
	return out;
}

QString
hexFromULongLong(const uint64 &in, unsigned int length)
{
	uint64 tmp;
	int n;
	QString out;

	if (in < 10)
		out = QString(QChar(((int) in) + '0'));
	else if (in < 16)
		out = QString(QChar(((int) in) + 55));
	else
	{
		tmp = in;
		while (tmp > 0)
		{
			n = tmp % 16;
			out.prepend(QChar(n + ((n < 10) ? '0' : 55)));
			tmp /= 16;
		}
	}
	while (out.length() < length) 
		out.prepend("0");
	return out;
}

int64 toLongLong(const QString &in, bool *ok)
{
	int64 out = 0;
	bool o = true;
	bool negate = false;

	for (unsigned int x = 0; x < in.length(); x++)
	{
		QChar c = in.at(x);
		if ((x == 0) && (c.unicode() == '-'))
			negate = true;
		else if (!muscleInRange(c.unicode(), (unichar) '0', (unichar) '9'))
		{
			o = false;
			break;
		}
		out *= 10;
		out += c - '0';
	}

	if (negate)
		out = -out;

	if (ok)
		*ok = o;
	return out;
}

QString fromLongLong(const int64 &in)
{
	uint64 tmp;
	int n;
	bool negate = false;
	QString out;

	if (in < 0)
	{
		tmp = -in;
		negate = true;
	}
	else if (in < 10)
		return QString(QChar(((int) in) + '0'));
	else
		tmp = in;

	while (tmp > 0)
	{
		n = tmp % 10;
		out.prepend(QChar(n + '0'));
		tmp /= 10;
	}
	
	if (negate)
		out.prepend("-");

	return out;
}

QString hexFromLongLong(const int64 &in, unsigned int length)
{
	uint64 i;
	memcpy(&i, &in, sizeof(int64));
	return hexFromULongLong(i, length);
}

void
AddToList(String & slist, const String &item)
{
	if (slist.Length() == 0)
		slist = item.Trim();
	else
	{
		slist += ",";
		slist += item.Trim();
	}
}

void
AddToList(String & slist, const char *item)
{
	AddToList(slist, String(item));
}

void AddToList(QString &slist, const QString &entry)
{
	if (slist.isEmpty())
		slist = entry.stripWhiteSpace();
	else
	{
		slist += ",";
		slist += entry.stripWhiteSpace();
	}
}

void RemoveFromList(QString &slist, const QString &entry)
{
	if (slist == entry.stripWhiteSpace())
	{
		slist = QString::null;
		return;
	}

	QStringList list = QStringList::split(",", slist);
	QStringList::Iterator iter = list.begin();
	while (iter != list.end())
	{
		if ((*iter).lower() == entry.stripWhiteSpace().lower())
		{
			list.remove(iter);
			break;
		}
		iter++;
	}
	slist = list.join(",");
}

void RemoveFromList(String &slist, const String &entry)
{
	if (slist == entry.Trim())
	{
		slist = "";
		return;
	}

	StringTokenizer tok(slist.Cstr(), ",");
	String out;
	const char * tmp;
	while ((tmp = tok.GetNextToken()) != NULL)
	{
		if (!entry.Trim().EqualsIgnoreCase(tmp))
			AddToList(out, tmp);
	}
	slist = out;
}

void HEXClean(QString &in)
{
	QString tmp;
	for (unsigned int x = 0; x < in.length(); x++)
	{
		QChar c = in[x].lower();
		if (
			muscleInRange(c.unicode(), (unichar) '0', (unichar) '9') || 
			muscleInRange(c.unicode(), (unichar) 'a', (unichar) 'f')
		   )
			tmp += c;
	}
	in = tmp;
}

void BINClean(QString &in)
{
	QString tmp, part;
	unsigned int s = 0, p;
	while (s < in.length())
	{
		// Skip initial garbage
		while (!muscleInRange(in[s].unicode(), (unichar) '0', (unichar) '1'))
		{
			s++;
			// avoid looping out of string...
			if (s == in.length())
			{
				if (tmp.isEmpty()) 
				{
					in = QString::null;
					return;
				}
				else
					break;
			}
		}
		part = QString::null;
		p = 0;
		while (p < 8)
		{
			if (s == in.length())
			  break;
			QChar c = in[s++];
			if (muscleInRange(c.unicode(), (unichar) '0', (unichar) '1'))
				part += c;
			else // garbage?
				break;
			p++;
		}
		if (p > 0)
		{
			while (part.length() < 8) 
				part.prepend("0");
			tmp += part;
		}
	}
	in = tmp;
}

void OCTClean(QString &in)
{
	QString tmp, part;
	unsigned int s = 0, p;
	while (s < in.length())
	{
		// Skip initial garbage
		while (!muscleInRange(in[s].unicode(), (unichar) '0', (unichar) '6'))
		{
			s++;
			// avoid looping out of string...
			if (s == in.length())
			{
				if (tmp.isEmpty()) 
				{
					in = QString::null;
					return;
				}
				else
					break;
			}
		}
		part = QString::null;
		p = 0;
		while (p < 3)
		{
			if (s == in.length())
			  break;
			QChar c = in[s++];
			if (muscleInRange(c.unicode(), (unichar) '0', (unichar) '6'))
				part += c;
			else // garbage?
				break;
			p++;
		}
		if (p > 0)
		{
			while (part.length() < 3) 
				part.prepend("0");
			tmp += part;
		}
	}
	in = tmp;
}

QString BINDecode(const QString &in)
{
	QCString out;

	if (in.length() % 8 != 0)
		return QString::null;

	for (unsigned int x = 0; x < in.length(); x += 8)
	{
		QString part = in.mid(x, 8);
		int xx = 1;
		int c = 0;
		for (int y = 7; y > -1; y--)
		{
			if (part[y] == '1')
				c = c + xx;
			xx *= 2;
		}
		out += (char) c;
	}
	return QString::fromUtf8(out);
}

QString BINEncode(const QString &in)
{
	QCString temp = in.utf8();
	QString out, part;
	for (unsigned int x = 0; x < temp.length(); x++)
	{
		unsigned char c = temp.at(x);
		part = QString::null;
		for (int xx = 0; xx < 8; xx++)
		{
			if (c % 2 == 1)
				part.prepend("1");
			else
				part.prepend("0");
			c /= 2;
		}
		out += part;
	}
	return out;
}

QString OCTDecode(const QString &in)
{
	QCString out;

	if (in.length() % 3 != 0)
		return QString::null;

	for (unsigned int x = 0; x < in.length(); x += 3)
	{
		QString part = in.mid(x, 3);
		int xx = 1;
		int c = 0;
		for (int y = 2; y > -1; y--)
		{
			c = c + (xx * (part[y].unicode() - '0'));
			xx *= 7;
		}
		out += (char) c;
	}
	return QString::fromUtf8(out);
}

QString OCTEncode(const QString &in)
{
	QCString temp = in.utf8();
	QString out, part;
	for (unsigned int x = 0; x < temp.length(); x++)
	{
		unsigned char c = temp.at(x);
		part = QString::null;
		for (int xx = 0; xx < 3; xx++)
		{
			part.prepend('0' + (c % 7));
			c /= 7;
		}
		out += part;
	}
	return out;
}

int 
Match(const QString &string, const QRegExp &exp)
{
	QString str;
	QRegExp e;
	if (exp.caseSensitive())
	{
		str = string;
		e = exp;
	}
	else
	{
		str = string.lower();
		e.setPattern(exp.pattern().lower());
	}
#if (QT_VERSION < 0x030000)
	if (e.pattern().contains("|"))
	{
		QStringTokenizer tok(e.pattern(), "|");
		QString t;
		while ((t = tok.GetNextToken()) != QString::null)
		{
			QRegExp r(t);
			int ret = str.find(r);
			if (ret >= 0)
				return ret;
		}
		return -1;
	}
#endif
	return str.find(e);
}

int64
ConvertPtr(void *ptr)
{
#if defined(WIN64) || defined(__osf__) || defined(__amd64__)
		return (int64) ptr;
#else
		return (int64) (int32) ptr;
#endif
}
