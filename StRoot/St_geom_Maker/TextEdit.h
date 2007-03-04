/****************************************************************************
** $Id: TextEdit.h,v 1.2 2007/03/04 18:50:55 fine Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <qmainwindow.h>
#include <qmap.h>

class QAction;
class QComboBox;
class QTabWidget;
class QTextEdit;

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit( QWidget *parent = 0, const char *name = 0 );
    void load( const QString &f );
    
signals:
    void  textSaved(const QString &fileName); 

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    QTextEdit *currentEditor() const;
    void doConnections( QTextEdit *e );

private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void fileClose();
    void fileExit();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();

    void textBold();
    void textChanged();
    void textUnderline();
    void textItalic();
    void textFamily( const QString &f );
    void textSize( const QString &p );
    void textColor();
    void textAlign( QAction *a );

    void fontChanged( const QFont &f );
    void colorChanged( const QColor &c );
    void alignmentChanged( int a );
    void editorChanged( QWidget * );
    
    void searchActivated( const QString &);
    void searchContinue( const QString &);

private:
    QAction *actionTextBold,
	*actionTextUnderline,
	*actionTextItalic,
	*actionTextColor,
	*actionAlignLeft,
	*actionAlignCenter,
	*actionAlignRight,
	*actionAlignJustify;
    QComboBox
	*comboFont,
	*comboSize;
    QTabWidget *tabWidget;
    QMap<QTextEdit*, QString> filenames;

};


#endif
