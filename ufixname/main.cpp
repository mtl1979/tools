#include <stdio.h>
#include <stdlib.h>
#include <QByteArray>
#include <qstring.h>
#include <qtextcodec.h>
#include <qfile.h>
#include <qcoreapplication.h>

int main(int argc, char* argv[])
{
	QCoreApplication app(argc, argv);
	if (argc != 2)
	{
		printf("Usage: %s <input file>\n", argv[0]);
		return -1;
	}
	else
	{
		QTextCodec *uc = QTextCodec::codecForName("UTF-8");
		QString out = uc->toUnicode(argv[1], (int) strlen(argv[1]));
		QString in = QString::fromLocal8Bit(argv[1]);
		QByteArray test = uc->fromUnicode(out);
		if (test != argv[1])
		{
			printf("Decoder error!\n");
			return -1;
		}
		
		uint sz = 0;
		char buf[1024];
		if (in != out)
		{
			QFile ifile(in);
			QFile ofile(out);
			
			if (ifile.open(QIODevice::ReadOnly) == false)
			{
				printf("Error opening input file!");
				return -1;
			}
			
			sz = ifile.size();
			
			if (ofile.open(QIODevice::WriteOnly) == false)
			{
				printf("Error opening output file!");
				ifile.close();
				return -1;
			}
			int r;
			__int64 numbytes = 0;
			while ((r = ifile.read(buf, 1024)) > 0)
			{
				ofile.write(buf, r);
				numbytes += r;
			}
			printf("Read %u bytes.\n", sz);
#ifdef WIN32
			printf("Wrote %I64i bytes.\n", numbytes);
#else
			printf("Wrote %Li bytes.\n", numbytes);
#endif
			ifile.close();
			ofile.close();
			if (sz == numbytes)
			{
				ifile.remove();
			}
		}
	}
	return 0;
}