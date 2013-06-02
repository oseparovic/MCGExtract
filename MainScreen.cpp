/*
 * File:   MainScreen.cpp
 *
 * Created on June 2, 2013, 10:34 AM
 */

#include "MainScreen.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct MCGFile {
  int size;
  vector<string> dir;
  
  MCGFile() {
      size = 0;
//      dir = NULL;
  }
};

map<string, MCGFile*> fileMap;

MainScreen::MainScreen() {
    
    widget.setupUi(this);
    widget.tableWidget->horizontalHeader()->setStretchLastSection(true); // stretch the column to width
    connect(widget.actionOpen, SIGNAL(triggered()), this, SLOT(actionOpen()));
    
    MCGFile *object2pak = new MCGFile();
    object2pak->size = 1189;
    object2pak->dir.push_back("mech - thor");
    object2pak->dir.push_back("mech - loki");
    object2pak->dir.push_back("mech - awesome");
    object2pak->dir.push_back("lazer fx");
    object2pak->dir.push_back("pulse lazer fx");
    object2pak->dir.push_back("sample explosion");
    object2pak->dir.push_back("betula tree");
    object2pak->dir.push_back("pine tree");
    object2pak->dir.push_back("palm tree");
    object2pak->dir.push_back("sample building");
    object2pak->dir.push_back("sample smoke");
    object2pak->dir.push_back("sample smoke");
    object2pak->dir.push_back("autocannon projectile");
    object2pak->dir.push_back("srm projectile");
    object2pak->dir.push_back("mech - uller");
    object2pak->dir.push_back("mech - hunchback");
    object2pak->dir.push_back("mech - cougar");
    object2pak->dir.push_back("mech - catapult");
    

    // FST files
//    fileMap["MISSION.FST"]=NULL;
//    fileMap["ART.FST"]=NULL;
//    fileMap["SHAPES.FST"]=NULL;
//    fileMap["MISC.FST"]=NULL;
//    fileMap["TERRAIN.FST"]=NULL;
//    // PAK files
//    fileMap["SHAPES90.PAK"]=NULL;
//    fileMap["SHAPES.PAK"]=NULL;
//    fileMap["SPRITES.PAK"]=NULL;
    fileMap["OBJECT2.PAK"]=object2pak;
//    fileMap["GTILES90.PAK"]=NULL;
//    fileMap["LARMS.PAK"]=NULL;
//    fileMap["RARMS.PAK"]=NULL;
//    fileMap["TORSOS.PAK"]=NULL;
//    fileMap["LEGS.PAK"]=NULL;
//    fileMap["LARMS90.PAK"]=NULL;
//    fileMap["RARMS90.PAK"]=NULL;
//    fileMap["TORSOS90.PAK"]=NULL;
//    fileMap["LEGS90.PAK"]=NULL;
//    fileMap["CURSORS.PAK"]=NULL;
//    fileMap["SOUND.PAK"]=NULL;
//    fileMap["TILES90.PAK"]=NULL;
//    fileMap["TILES.PAK"]=NULL;
//    fileMap["BETTY.PAK"]=NULL;
//    fileMap["BLIP.PAK"]=NULL;
//    fileMap["FEET.PAK"]=NULL;
//    fileMap["ART.PAK"]=NULL;
//    fileMap["EPILOT.PAK"]=NULL;
//    fileMap["CREWE.PAK"]=NULL;
//    fileMap["CREWD.PAK"]=NULL;
//    fileMap["CREWC.PAK"]=NULL;
//    fileMap["CREWB.PAK"]=NULL;
//    fileMap["CREWA.PAK"]=NULL;
//    fileMap["PILOTAD.PAK"]=NULL;
//    fileMap["PILOTAC.PAK"]=NULL;
//    fileMap["PILOTAB.PAK"]=NULL;
//    fileMap["PILOTAA.PAK"]=NULL;
//    fileMap["PILOTZ.PAK"]=NULL;
//    fileMap["PILOTY.PAK"]=NULL;
//    fileMap["PILOTW.PAK"]=NULL;
//    fileMap["PILOTV.PAK"]=NULL;
//    fileMap["PILOTU.PAK"]=NULL;
//    fileMap["PILOTT.PAK"]=NULL;
//    fileMap["PILOTS.PAK"]=NULL;
//    fileMap["PILOTR.PAK"]=NULL;
//    fileMap["PILOTQ.PAK"]=NULL;
//    fileMap["PILOTP.PAK"]=NULL;
//    fileMap["PILOTO.PAK"]=NULL;
//    fileMap["PILOTN.PAK"]=NULL;
//    fileMap["PILOTM.PAK"]=NULL;
//    fileMap["PILOTL.PAK"]=NULL;
//    fileMap["PILOTK.PAK"]=NULL;
//    fileMap["PILOTJ.PAK"]=NULL;
//    fileMap["PILOTI.PAK"]=NULL;
//    fileMap["PILOTH.PAK"]=NULL;
//    fileMap["PILOTG.PAK"]=NULL;
//    fileMap["PILOTF.PAK"]=NULL;
//    fileMap["PILOTE.PAK"]=NULL;
//    fileMap["PILOTD.PAK"]=NULL;
//    fileMap["PILOTC.PAK"]=NULL;
//    fileMap["PILOTB.PAK"]=NULL;
//    fileMap["PILOTA.PAK"]=NULL;
//    fileMap["SHADOW.PAK"]=NULL;
//    fileMap["NOISE.PAK"]=NULL;
//    fileMap["CRATER.PAK"]=NULL;
}

MainScreen::~MainScreen() {
}

void MainScreen::actionOpen() {
    // bring up the file opener
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));
    QFileInfo fileInfo = filePath;
    QString qFileName = fileInfo.fileName();
    string fileName = qFileName.toStdString();
    
    // display the contents list of the file
    if (fileMap.find(fileName) == fileMap.end()) {
        QMessageBox::warning(this, "Index mapping error", "No index mapping exists for the file: "+qFileName);
    } else {
        MCGFile *temp = fileMap.find(fileName)->second;
        int count = 0;
        char buffer [5];
        for(vector<string>::const_iterator i = temp->dir.begin();i != temp->dir.end(); ++i) {
            string fileName = *i;
//            QMessageBox::warning(this, "Index mapping error", fileName.c_str());
            widget.tableWidget->setHidden(false);
            widget.tableWidget->setRowCount(temp->size);
            widget.tableWidget->setColumnCount(1);
//            widget.tableWidget->setItem(count, 0, new QTableWidgetItem(itoa(count,buffer,10)));
            widget.tableWidget->setItem(count, 0, new QTableWidgetItem(fileName.c_str()));
            count++;
        }        
    }    
}