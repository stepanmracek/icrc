#include <QApplication>

#include "batchstraintracker.h"
#include "createmodel.h"
#include "testbeatdetection.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //BatchStrainTracker::extractStrains("/mnt/data/Dropbox/projekty/icrc/test2", "nem");
    //BatchStrainTracker::stats("zdr");
    //BatchStrainTracker::stats("nem");
    //BatchStrainTracker::exportValues("zdr");
    //BatchStrainTracker::exportValues("nem");

    //BatchStrainTracker::evaluate("nem", "zdr");
    //CreateModel::create("/mnt/data/strain/potkani", "potkani");

    //TestBeatDetection::test();
    TestBeatDetection::eval();

    return 0;
}
