/**********************************************************************
**   Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
**   finddialog.cpp
**
**   This file is part of Qt Linguist.
**
**   See the file LICENSE included in the distribution for the usage
**   and distribution terms.
**
**   The file is provided AS IS with NO WARRANTY OF ANY KIND,
**   INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR
**   A PARTICULAR PURPOSE.
**
**********************************************************************/

/*  TRANSLATOR FindDialog

  Go to Edit > Find...  The dialog that pops up is a FindDialog.
*/

#include "finddialog.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

FindDialog::FindDialog( QWidget *parent, const char *name, bool modal )
    : QDialog( parent, name, modal )
{
    QGridLayout *gl = new QGridLayout( this, 3, 4, 11, 11,
				       "find outer layout" );
    QVBoxLayout *cl = new QVBoxLayout( 6, "find checkbox layout" );
    QVBoxLayout *bl = new QVBoxLayout( 6, "find button layout" );

    led = new QLineEdit( this, "find line edit" );
    QLabel *findWhat = new QLabel( led, tr("Fi&nd what:"), this, "find what" );
    sourceText = new QCheckBox( tr("&Source texts"), this,
				"find in source texts" );
    sourceText->setChecked( TRUE );
    translations = new QCheckBox( tr("&Translations"), this,
				  "find in translations" );
    translations->setChecked( TRUE );
    comments = new QCheckBox( tr("&Comments"), this, "find in comments" );
    comments->setChecked( TRUE );
    matchCase = new QCheckBox( tr("&Match case"), this, "find match case" );
    matchCase->setChecked( FALSE );
    QPushButton *findNxt = new QPushButton( tr("&Find Next"), this,
					    "find next" );
    findNxt->setDefault( TRUE );
    QPushButton *cancel = new QPushButton( tr("Cancel"), this, "cancel find" );

    gl->addWidget( findWhat, 0, 0 );
    gl->addMultiCellWidget( led, 0, 0, 1, 2 );
    gl->addWidget( matchCase, 1, 2 );
    gl->addMultiCell( bl, 0, 2, 3, 3 );
    gl->addMultiCell( cl, 1, 2, 0, 1 );
    gl->setColStretch( 0, 0 );
    gl->addColSpacing( 1, 40 );
    gl->setColStretch( 2, 1 );
    gl->setColStretch( 3, 0 );

    cl->addWidget( sourceText );
    cl->addWidget( translations );
    cl->addWidget( comments );
    cl->addStretch( 1 );

    bl->addWidget( findNxt );
    bl->addWidget( cancel );
    bl->addStretch( 1 );

    resize( 400, 1 );
    setMaximumHeight( height() );

    led->setFocus();

    connect( findNxt, SIGNAL(clicked()), this, SLOT(emitFindNext()) );
    connect( cancel, SIGNAL(clicked()), this, SLOT(reject()) );

    QWhatsThis::add( this, tr("This window allows you to search for some text"
			      " in the message file.") );
    QWhatsThis::add( led, tr("Type in the text to search for.") );
    QWhatsThis::add( sourceText, tr("Source texts are searched when"
				    " checked.") );
    QWhatsThis::add( translations, tr("Translations are searched when"
				      " checked.") );
    QWhatsThis::add( comments, tr("Comments and contexts are searched when"
				  " checked.") );
    QWhatsThis::add( matchCase, tr("Texts such as 'TeX' and 'tex' are"
				   " considered as different when checked.") );
    QWhatsThis::add( findNxt, tr("Click here to find the next occurrence of the"
				 " text you typed in.") );
    QWhatsThis::add( cancel, tr("Click here to close this window.") );
}

void FindDialog::emitFindNext()
{
    int where = ( sourceText->isChecked() ? SourceText : 0 ) |
		( translations->isChecked() ? Translations : 0 ) |
		( comments->isChecked() ? Comments : 0 );
    emit findNext( led->text(), where, matchCase->isChecked() );
}