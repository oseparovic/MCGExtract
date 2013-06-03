/* 
 * File:   MainScreen.h
 *
 * Created on June 2, 2013, 10:34 AM
 */

#ifndef _MAINSCREEN_H
#define	_MAINSCREEN_H

#include "ui_MainScreen.h"

class MainScreen : public QMainWindow {
    Q_OBJECT
public:
    MainScreen();
    virtual ~MainScreen();
public Q_SLOTS: //slots to receive action of menu trigger.
    void actionOpen();
    void actionExtract_All();
    void actionExtract_Index();
private:
    Ui::MainScreen widget;
};

#endif	/* _MAINSCREEN_H */
