#ifndef DIALOGCREATECOORDSYSTEM_H
#define DIALOGCREATECOORDSYSTEM_H

#include <QDialog>

#include "strain/coordsystem.h"

class DialogCreateCoordSystem : public QDialog
{
    Q_OBJECT

public:
    DialogCreateCoordSystem(QWidget *parent = 0) : QDialog(parent) { }

    virtual CoordSystemBase *getNewCoordSystem() = 0;
};

#endif // DIALOGCREATECOORDSYSTEM_H
