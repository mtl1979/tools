// Swedish Strings

#ifndef LANG_SWEDISH_H
#define LANG_SWEDISH_H

//
// Search Window
//

#define MSG_SW_CSEARCH        "S�k:"
#define MSG_SW_FILENAME       "Filnamn"
#define MSG_SW_FILESIZE       "Filstorlek"
#define MSG_SW_FILETYPE       "Filtyp"
#define MSG_SW_MODIFIED       "�ndrad"
#define MSG_SW_PATH           "S�kv�g"
#define MSG_SW_USER           "Anv�ndare"
#define MSG_SW_DOWNLOAD       "Ladda ner"
#define MSG_SW_CLOSE          "Avst�nga"
#define MSG_SW_CLEAR          "Rensa"
#define MSG_SW_STOP           "Stoppa"

#define MSG_IDLE			"Ledig."
#define MSG_WF_RESULTS		"Resultat visade: %1"
#define MSG_SEARCHING		"S�kas: \"%1\"."
#define MSG_WAIT_FOR_FST	"V�ntar att fils�kningstr�dd har slutat..."

// Main Window

#define NAME				"Unizone (Svenska)"				// Application Title

#define MSG_CSTATUS			"Status:"
#define MSG_CSERVER			"Server:"
#define MSG_CNICK			"Namn:"

#define MSG_CONNECTING      "Ansluter till servern %1."
#define MSG_CONNECTFAIL		"Anslutning till server misslyckades!"
#define MSG_CONNECTED       "Anslutat."
#define MSG_DISCONNECTED	"Kopplat ner fr�n servern."
#define MSG_NOTCONNECTED    "Inte anslutat."
#define MSG_RESCAN          "S�ker igen delade filer..."
#define MSG_SCAN_ERR1		"S�ker redan!"
#define MSG_NOTSHARING		"Fildeladen inte aktiverad."
#define MSG_NONICK			"Ingen namn angiven."
#define MSG_NOMESSAGE		"Ingen meddelande att s�nda."
#define MSG_AWAYMESSAGE		"Borta-meddelande satt till %1."
#define MSG_HEREMESSAGE		"H�r-meddelane satt till %1."
#define MSG_UNKNOWNCMD		"Ok�nt kommando!"
#define MSG_NOUSERS			"Ingen anv�ndaren avgiven."
#define MSG_NO_INDEX		"Ingen index avgiven."
#define MSG_INVALID_INDEX	"Fel index."
#define MSG_USERSNOTFOUND   "Anv�ndaren(-rna) finns inte!"
#define MSG_PINGSENT		"Ping s�nt till anv�ndare #%1 (alias <font color=\"%3\">%2</font>)."
#define MSG_NEWVERSION		" %1 finns nu f�r http://www.raasu.org/tools/windows/."
#define MSG_SCANSHARES		"S�ker delade filer..."
#define MSG_SHARECOUNT		"%1 fil(er) deladen."
#define MSG_NAMECHANGED		"Du heter nu <font color=\"%2\">%1</font>."
#define MSG_UNKNOWN			"Ok�nt"
#define MSG_USERHOST		"<font color=\"%3\">%1</font>'s IP address �r %2."
#define MSG_NUM_USERS		"Det finns %1 anv�ndraren\n"

#define MSG_HERE			"h�r"
#define MSG_AWAY			"borta"
#define MSG_STATUS_IDLE		"ledig"
#define MSG_STATUS_BUSY		"upptagen"
#define MSG_AT_WORK			"i arbete"
#define MSG_AROUND			"omkring"
#define MSG_SLEEPING		"sovande"
//
// Menus
//

// Menu bar

#define MSG_FILE			"&Arkiv"
#define MSG_AFILE			CTRL+Key_A
#define MSG_EDIT			"�&ndra"
#define MSG_AEDIT			CTRL+Key_N
#define MSG_OPTIONS			"&Preferenser"
#define MSG_AOPTIONS		CTRL+Key_P
#define MSG_HELP			"&Hj�lp"
#define MSG_AHELP			CTRL+Key_H

// File menu

#define MSG_ABOUT			"&Om "
#define MSG_AABOUT			CTRL+Key_O

#define MSG_CONNECT			"&Anslut"
#define MSG_ACONNECT		CTRL+SHIFT+Key_A
#define MSG_DISCONNECT		"&Koppla ner"
#define MSG_ADISCONNECT		CTRL+SHIFT+Key_K
#define MSG_OPEN_SHARED		"�ppna &Delade Filer Katalogen"
#define MSG_AOPEN_SHARED	CTRL+Key_D
#define MSG_OPEN_DOWNLOAD	"�ppna &Nerladdade Filer Katalogen"
#define MSG_AOPEN_DOWNLOAD	CTRL+Key_N
#define MSG_OPEN_LOGFOLDER	"�ppna &Logg Katalogen"
#define MSG_AOPEN_LOGFOLDER CTRL+Key_L
#define MSG_CLEAR_CHATLOG	"R&ensa Chatt Logg"
#define MSG_ACLEAR_CHATLOG	CTRL+Key_E
#define MSG_SEARCH			"S�k"
#define MSG_ASEARCH			CTRL+ALT+Key_S
#define MSG_EXIT			"A&vsluta"
#define MSG_AEXIT			CTRL+Key_V

// Edit menu

#define MSG_PREFERENCES		"&Inst�llningar"
#define MSG_APREFERENCES	CTRL+Key_I

// Preferences window
//

#define MSG_PR_PREFERENCES		"Inst�llningar"

// Tab names

#define MSG_GENERAL				"Allm�n"
#define MSG_CONNECTION			"Anslutning"
#define MSG_DISPLAY				"Visning"
#define MSG_COLORS				"F�rger"
#define MSG_STYLE				"Stil"
#define MSG_FILE_SHARING		"Fildeladen"
#define MSG_URL_LAUNCHING		"URL �ppning"
#define MSG_CHTTP_LAUNCHER		"HTTP �ppnaren:"
#define MSG_CFTP_LAUNCHER		"FTP �ppnaren:"
#define MSG_CMAILTO_LAUNCHER	"Mailto: �ppnaren:"
#define MSG_THROTTLING			"Restriktioner"

// General

#define MSG_CAUTOAWAY		"Auto Borta:"
#define MSG_DISABLED		"Avaktiverad"
#define MSG_2_MINUTES		"2 Minuter"
#define MSG_5_MINUTES		"5 Minuter"
#define MSG_10_MINUTES		"10 Minuter"
#define MSG_15_MINUTES		"15 Minuter"
#define MSG_20_MINUTES		"20 Minuter"
#define MSG_30_MINUTES		"30 Minuter"
#define MSG_1_HOUR			"1 Timme"
#define MSG_2_HOURS			"2 Timmar"
#define MSG_AUTOUPDATE		"Uppdatera Serverlistan Automatiskt"
#define MSG_CHECK_NEW		"Checka f�r nya versioner"
#define MSG_LOGIN_ON_START	"Logga in vid Uppstart"
#define MSG_ENABLE_LOGGING	"Aktivera Loggning"
#define MSG_MULTI_COLOR_LISTVIEWS "M�ngf�rgad ListView"

#define MSG_OK				"OK"
#define MSG_CANCEL			"Avbruta"
#define MSG_TOGGLE_BLOCK	"(Av)sp�rra"

// Connection

#define MSG_CUPLOAD_BAND	"Uppladdningsbandbredd:"
#define MSG_FIREWALLED		"Jag �r bakom en Brandv�gg"

// Display 

#define MSG_TIMESTAMPS		"Tidst�mplar"
#define MSG_USEREVENTS		"Anv�ndarh�ndelser"
#define MSG_UPLOADS			"Uppladdnings"
#define MSG_CHAT			"Chatt"
#define MSG_PRIVATE_MSGS	"Privata Meddelanden"
#define MSG_INFO_MSGS		"Infomeddelanden"
#define MSG_WARNING_MSGS	"Varningmeddelanden"
#define MSG_ERROR_MSGS		"Felmeddelanden"
#define MSG_FLASH_WINDOW	"Antyda F�nster N�r N�mnas"
#define MSG_FLASH_PRIVATE	"Antyda Privat F�nstrar"
#define MSG_FONT_SIZE		"Fontstorlek"

// Colors

#define MSG_CDESCRIPTION	"Beskrivning:"
#define MSG_CPREVIEW		"F�r�verse:"
#define MSG_CHANGE			"Andra"
#define MSG_LOCAL_NAME		"Egen Namn"
#define MSG_HLOCAL_NAME		"Den h�r �r f�rgen av ditt namn."
#define MSG_REMOTE_NAME		"Andra Namner"
#define MSG_HREMOTE_NAME	"Den h�r �r f�rgen av andra anv�ndarens namnar."
#define MSG_REGULAR_TEXT	"Ordin�r Text"
#define MSG_HREGULAR_TEXT	"Den h�r �r f�rgen av text s�nt av du och andra anv�ndraren."
#define MSG_SYSTEM_TEXT		"System Text"
#define MSG_HSYSTEM_TEXT	"Den h�r �r f�rgen av \"System\"."
#define MSG_PING_TEXT		"Ping Text"
#define MSG_HPING_TEXT		"Den h�r �r f�rgen av ping svaret."
#define MSG_ERROR_TEXT		"Fel Text"
#define MSG_HERROR_TEXT		"Den h�r �r f�rgen av \"Fel\"."
#define MSG_ERRORMSG_TEXT	"Fel Meddelande Text"
#define MSG_HERRORMSG_TEXT	"Den h�r �r f�rgen av fel meddelanden."
#define MSG_PRIVATE_TEXT	"Privat Text"
#define MSG_HPRIVATE_TEXT	"Den h�r �r f�rgen av privat text."
#define MSG_ACTION_TEXT		"H�ndelse Text"
#define MSG_HACTION_TEXT	"Den h�r �r f�rgen av \"H�ndelse\"."
#define MSG_URL_TEXT		"URL Text"
#define MSG_HURL_TEXT		"Den h�r �r f�rgen av URLs."
#define MSG_NAME_SAID_TEXT	"'Namn Sade' Text"
#define MSG_HNAME_SAID_TEXT "Den h�r �r f�rgen n�r ditt namn har sade i andra anv�ndrarens text i general chatt."
#define MSG_HSTYLE			"Selekterat stil ska aktiveras n�r det har selekterad."

// File Sharing

#define MSG_FS_ENABLED			"Fildelning Aktiverad?"
#define MSG_BINKYNUKE			"Blocka anonym anv�ndraren?"
#define MSG_BLOCK_DISCONNECTED	"Blocka nerkopplat anv�ndraren?"
#define MSG_CFS_MAXUP		"Maximum Samtidiga Uppladdningar:"
#define MSG_CFS_MAXDOWN		"Maximum Samtidiga Nerladdningar:"

// Throttling

#define MSG_CCHAT				"Chatt:"
#define MSG_TH_UPLOADS			"Uppladdningar (per upladdning):"
#define MSG_TH_DOWNLOADS		"Nerladdningar (per nerladdning):"
#define MSG_TH_BLOCKED			"Uppladdningar (per blockad):"
#define MSG_UNLIMITED			"Obegr�nsat"
#define MSG_NO_LIMIT			"Obegr�nsat"
#define MSG_BYTES				"byter"

//
// Nick List
//

#define MSG_NL_NAME				"Namn"
// ID is not translated Yet
#define MSG_NL_STATUS			"Status"
#define MSG_NL_FILES			"Filer"
#define MSG_NL_CONNECTION		"Anslutning"
#define MSG_NL_LOAD				"Belastning"
#define MSG_NL_CLIENT			"Klient"

#define MSG_NL_PRIVATECHAT		"Privat Chatt Med %1"
#define MSG_NL_LISTALLFILES		"Lista Alla Filer"
#define MSG_NL_GETIPADDR		"F�rs�ka IP Addressen"
#define MSG_NL_GETADDRINFO		"F�rs�ka Address Info"
#define MSG_REMOVE				"Ta bort"

//
// Format Strings
//

#define MSG_WF_USERCONNECTED		"Anv�ndrare #%1 �r nu ansluten."
#define MSG_WF_USERDISCONNECTED		"Anv�ndrare #%1 (alias <font color=\"%3\">%2</font>) har kopplat ner."
#define MSG_WF_USERNAMECHANGENO		"Anv�ndrare #%1 heter nu <font color=\"%3\">%2</font>."
#define MSG_WF_USERNAMECHANGED		"Anv�ndrare #%1 (alias <font color=\"%4\">%2</font>) heter nu <font color=\"%5\">%3</font>."
#define MSG_WF_USERSTATUSCHANGE		"Anv�ndrare #%1 (alias <font color=\"%4\">%2</font>) �r nu %3."
#define MSG_WF_USERSTATUSCHANGE2	"Anv�ndrare #%1 �r nu %2."
#define MSG_WF_STATUSCHANGED		"Du �r nu %1."

#define MSG_WF_SYSTEMTEXT       "<font color=\"%1\" size=\"%2\"><b>System:</b> </font>"
#define MSG_WF_PINGTEXT         "<font color=\"%1\" size=\"%2\">Pingen �terv�nde efter %3 millisekunden (%4)</font>"
#define MSG_WF_PINGUPTIME       "<font color=\"%1\" size=\"%2\"> (Uppetid: %3, Inloggad %4)</font>"
#define MSG_WF_ERROR			"<font color=\"%1\" size=\"%2\"><b>Fel:</b></font> "
#define MSG_WF_WARNING			"<font color=\"%1\" size=\"%2\"><b>Varning:</b></font> "
#define MSG_WF_ACTION           "<font color=\"%1\" size=\"%2\"><b>H�ndelse:</b></font> "
#define MSG_WF_GOTPINGED		"<font color=\"%1\" size=\"%2\">Anv�ndrare #%3 (alias <font color=\"%5\">%4</font>) pingad du.</font>"



//
// Transfer Window
//

#ifdef MSG_NL_STATUS
#define MSG_TX_STATUS			MSG_NL_STATUS
#endif

#define	MSG_TX_FILENAME			"Filnamn"
#define MSG_TX_RECEIVED			"Mottagit"
#define	MSG_TX_TOTAL			"Tillsammans"
#define	MSG_TX_RATE				"Hastighet"
#define MSG_TX_ETA				"ETA"
#define MSG_TX_USER				"Anv�ndare"
#define MSG_TX_SENT				"S�nt"
#define MSG_TX_QUEUE			"Queue"
#define MSG_TX_THROTTLE			"Throttle"
#define MSG_TX_MOVEUP			"Move Up"
#define MSG_TX_MOVEDOWN			"Move Down"
#define MSG_TX_CAPTION			"Filladdnings"

#define MSG_TX_ISDOWNLOADING	"%1 laddar ner %2."
#define MSG_TX_HASFINISHED		"%1 had nerladdad %2."
#define MSG_TX_FINISHED			"Du har nerladdad %2 fr�n %1."

#endif