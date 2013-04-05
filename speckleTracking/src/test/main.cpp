#include "test.h"
#include "strain/batchtesting.h"

int main(int argc, char *argv[])
{
    //Test::testSerialization();
    //Test::testAnotation();
    //Test::testUniformSpline();
    //Test::testLearnShape("/home/stepo/ownCloud/icrc/test/test_shapemap_26");
    //Test::testTracking();
    //Test::testStatisticalShapeChanges();
    //Test::testAutomaticTracking();
    //Test::testStatistics();
    //Test::testOpticalFlowTracking();
    //Test::testImageProcessing();
    //Test::testOpticalFlowIntensityMap();
    //Test::testQtAnotation(argc, argv);
    //Test::testQtManager(argc, argv);
    BatchTesting::process();
    //Test::testLongitudinalStrain();
    //Test::testQtAnotationAndDisplay(argc, argv);
    //Test::testBeatToBeatVariance();
}
