#include "dialogshapemodel.h"
#include "ui_dialogshapemodel.h"

#include <QVBoxLayout>

DialogShapeModel::DialogShapeModel(BackProjectionBase *backProjectionBase, ShapeTracker *tracker, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogShapeModel), backProjectionBase(backProjectionBase), tracker(tracker)
{
    ui->setupUi(this);
    ui->widgetResult->setTracker(tracker);

    int modesCount = backProjectionBase->getModes();
    QVBoxLayout *layout = new QVBoxLayout();
    for (int i = 0; i < modesCount; i++)
    {
        QSlider *slider = new QSlider(Qt::Horizontal);
        slider->setMinimum(0);
        slider->setMaximum(100);
        slider->setPageStep(10);
        slider->setValue(50);
        layout->addWidget(slider);
        ui->scrollAreaWidgetContents->setLayout(layout);
        sliders << slider;

        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(drawShapeModel()));
    }

    drawShapeModel();
}

void DialogShapeModel::drawShapeModel()
{
    int modesCount = backProjectionBase->getModes();
    MatF params = MatF::zeros(modesCount, 1);
    for (int i = 0; i < modesCount; i++)
    {
        QSlider *slider = sliders[i];
        float val = (slider->value() - 50.0f) / 100.0f * 3.0f * sqrt(backProjectionBase->getVariance(i));
        params(i) = val;
    }

    MatF backProjected = backProjectionBase->backProject(params);
    int r = backProjected.rows;
    for (int i = 0; i < r; i++)
    {
        backProjected(i) = backProjected(i) * r * 5;
    }

    Points resultPoints = Common::matFToPoints(backProjected);
    ui->widgetResult->setResultPoints(resultPoints, 0);
}

DialogShapeModel::~DialogShapeModel()
{
    delete ui;
}
