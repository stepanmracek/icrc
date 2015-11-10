#include <QCoreApplication>
#include <QCommandLineParser>

#include "batchstraintracker.h"
#include "createmodel.h"
#include "testbeatdetection.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Strain CLI");
    app.setApplicationVersion("0.99a");
    
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("sourceDir", "Source directory");
    parser.addPositionalArgument("modelName", "Model Name");
    parser.addOptions(CreateModel::Settings::getCommandLineOptions());
    parser.process(app);
    
    QStringList args = parser.positionalArguments();
    if (args.length() < 2) parser.showHelp();
    
    QString sourceDir = args.at(0);
    QString modelName = args.at(1);
    
    CreateModel::Settings settings = CreateModel::Settings::get(parser);
    CreateModel::create(sourceDir, modelName, settings);
    
    //BatchStrainTracker::extractStrains("/mnt/data/Dropbox/projekty/icrc/test2", "nem");
    //BatchStrainTracker::stats("zdr");
    //BatchStrainTracker::stats("nem");
    //BatchStrainTracker::exportValues("zdr");
    //BatchStrainTracker::exportValues("nem");

    //BatchStrainTracker::evaluate("nem", "zdr");
    //CreateModel::create("/mnt/data/strain/potkani", "potkani");

    //TestBeatDetection::test();
    //TestBeatDetection::eval();

    return 0;
}
