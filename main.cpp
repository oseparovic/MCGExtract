/*
 * File:   main.cpp
 *
 * Created on June 2, 2013, 9:02 AM
 */

#include <QtGui/QApplication>
#include "MainScreen.h"

int main(int argc, char *argv[]) {
    // initialize resources, if needed
    // Q_INIT_RESOURCE(resfile);

    QApplication app(argc, argv);

    // create and show your widgets here
    MainScreen window;
    window.show();

    return app.exec();
}
