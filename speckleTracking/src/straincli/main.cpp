#include "batchstraintracker.h"
#include "strainui/dialogstrainstatistics.h"

int main(int argc, char *argv[])
{
    //BatchStrainTracker::extractStrains("/mnt/data/Dropbox/projekty/icrc/test2");
    BatchStrainTracker::evaluate("nem", "zdr");
    return 0;
}
