#ifndef PREFS_H
#define PREFS_H
#include "prefs.h"

class WPrefs : public WPrefsBase
{ 
    Q_OBJECT

public:
    WPrefs( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~WPrefs();

private slots:
	void OK();
	void Cancel();
	void StyleSelected(int index);
	void ColorSelected(int index);
	void ChangeColor();
	void AwaySelected(int index);

private:
	void UpdateDescription(int);
	void InitLanguage();
	
	int fCurColorIndex;
	QString fColor[11];
};

#endif // PREFS_H
