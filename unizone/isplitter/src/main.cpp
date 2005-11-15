#include "uenv.h"
#include "mainwindowimpl.h"

#include <qapplication.h>
#include <qfile.h>
#include <qfiledialog.h>

#if !defined(QT_NO_STYLE_PLATINUM)
# include <qplatinumstyle.h>
#endif

QString MakePath(const QString &dir, const QString &file)
{
	QString ret = QDir::convertSeparators(dir);
	if (!ret.endsWith(QChar(QDir::separator())))
		ret += QDir::separator();

	ret += file;

	return ret;
}

int 
main( int argc, char** argv )
{
	QApplication app( argc, argv );
	QTranslator qtr( 0 );
	QTranslator qtr2( 0 );

	// Load language file
	QFile lang("isplitter.lng");
	QString lfile;
	if (!lang.exists())
	{
		lfile = QFileDialog::getOpenFileName( QString::null, "isplitter_*.qm", NULL );
		if (!lfile.isEmpty())
		{
			// Save selected language's translator filename
			if ( lang.open(IO_WriteOnly) )
			{
				QCString clang = lfile.utf8();
				lang.writeBlock(clang, clang.length());
				lang.close();
			}
		}
	}

	// (Re-)load translator filename
	if ( lang.open(IO_ReadOnly) ) 
	{    
		// file opened successfully
		char plang[255];
		lang.readLine(plang, 255);
		lfile = QString::fromUtf8(plang);
		lang.close();
    }


	// Install translator ;)
	if (!lfile.isEmpty())
	{
		if (QFile::exists(lfile))
		{
			if (qtr.load(lfile))
			{
#if (QT_VERSION >= 0x030000)
				qDebug("Application Translation File: %S", QDir::convertSeparators(lfile).ucs2());
#endif
				app.installTranslator( &qtr );
			}
		}
		// Qt's own translator file
		QFileInfo qfi(lfile);
		QString langfile = qfi.fileName().replace("isplitter", "qt");
		QString qt_lang = QString::null;
		QString qtdir = EnvironmentVariable("QTDIR");
		if (qtdir != QString::null)
		{
			QString tr_dir = MakePath(qtdir, "translations");
			qt_lang = MakePath(tr_dir, langfile);
			if (!QFile::exists(qt_lang))
				qt_lang = QString::null;
		}
		if (qt_lang == QString::null)
		{
			// Try using same directory as Image Splitters translations
			qt_lang = MakePath(qfi.dirPath(true), langfile);
		}

		if (QFile::exists(qt_lang))
		{
			if (qtr2.load(qt_lang))
			{
#if (QT_VERSION >= 0x030000)
				qDebug("Qt Translation File: %S", qt_lang.ucs2());
#endif
				app.installTranslator( &qtr2 );
			}
		}
	}
	
#if !defined(QT_NO_STYLE_PLATINUM)
	// Set style
	app.setStyle(new QPlatinumStyle);
#endif

	ImageSplitter * window = new ImageSplitter(NULL);
	CHECK_PTR(window);

	app.setMainWidget(window);

	window->show();

	return app.exec();
}
