// lat2utf.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <QByteArray>
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <qcoreapplication.h>

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	wchar_t warg[256];
	int len;
	if (app.arguments().count() != 3)
	{
		len = app.arguments().at(0).toWCharArray(warg);
		warg[len] = 0;
		printf("Usage: %S input output\n", warg);
		return -1;
	}

	QFile fIn(app.arguments().at(1)), fOut(app.arguments().at(2));
	if (fIn.open(QIODevice::ReadOnly) == false)
	{
		len = app.arguments().at(1).toWCharArray(warg);
		warg[len] = 0;
		printf("Error opening input file %S!", warg);
		return -1;
	}
	if (fOut.open(QIODevice::WriteOnly) == false)
	{
		len = app.arguments().at(2).toWCharArray(warg);
		warg[len] = 0;
		printf("Error opening output file %S!", warg);
		fIn.close();
		return -1;
	}

	char qTemp1[256];
	QString qTemp2("");
	QByteArray qTemp3("");
 	QTextCodec * ic = QTextCodec::codecForName( "ISO-8859-1" );
	QTextCodec * oc = QTextCodec::codecForName( "UTF-8" );
	do
	{
		int bytes = fIn.readLine(qTemp1, 255);
		if (bytes == -1)
			break;
		qTemp2 = ic->toUnicode(qTemp1);
		qTemp3 = oc->fromUnicode(qTemp2);
		fOut.write(qTemp3, qTemp3.length());
	} while (1);
	fOut.close();
	fIn.close();
	return 0;
}
