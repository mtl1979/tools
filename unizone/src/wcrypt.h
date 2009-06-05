#ifndef WCRYPT_H
#define WCRYPT_H

class QString;

QByteArray wencrypt(const QString &, int * = NULL);
QString wdecrypt(const QByteArray &, int);

// Armoured versions
QString wencrypt2(const QString &);
QString wdecrypt2(const QString &);
#endif
