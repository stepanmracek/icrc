#include "batchstraintracker.h"
#include "createmodel.h"

int main(int argc, char *argv[])
{
    //BatchStrainTracker::extractStrains("/mnt/data/Dropbox/projekty/icrc/test2", "nem");
    BatchStrainTracker::stats("nem");
    BatchStrainTracker::stats("zdr");

    //BatchStrainTracker::evaluate("nem", "zdr");
    //CreateModel::create("/mnt/data/strain/potkani", "potkani");

    return 0;
}
