#include "dialogimageprocessing.h"
#include "ui_dialogimageprocessing.h"

#include <QDebug>

#include "uiutils.h"

DialogImageProcessing::DialogImageProcessing(const Mat8 &frame, CoordSystemBase *coordSystem, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImageProcessing)
{
    ui->setupUi(this);
    QPushButton* applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(on_btnApply_clicked()));

    image = coordSystem->transform(frame);

    filterNames << "Median" << "Histogram EQ" << "Denoise" << "Contrast" << "Edge detection";
    ui->cmbFilters->addItems(filterNames);

    ui->gv->setScene(new QGraphicsScene(ui->gv));
    ui->gv->scene()->addPixmap(UIUtils::Mat8ToQPixmap(image));
}

DialogImageProcessing::~DialogImageProcessing()
{
    delete ui;
}

void DialogImageProcessing::on_btnAdd_clicked()
{
    QString selected = ui->cmbFilters->currentText();
    ui->lstFilters->addItem(selected);
}

void DialogImageProcessing::on_btnRemove_clicked()
{
    QList<QListWidgetItem*> selectedItems = ui->lstFilters->selectedItems();
    qDeleteAll(selectedItems);
}

ImageFilterBase *DialogImageProcessing::createImageFilter(const QString &name)
{
    switch(filterNames.indexOf(name))
    {
    case 0:
        return new ImageFilterMedian();
    case 1:
        return new ImageFilterHistEq();
    case 2:
        return new ImageFilterNlMeansDenoise();
    case 3:
        return new ImageFilterContrast(2);
    default:
        return new ImageFilterEdge(11);
    }
}

void DialogImageProcessing::on_btnApply_clicked()
{
    ui->gv->scene()->clear();
    Mat8 clone = image.clone();

    int n = ui->lstFilters->count();
    for (int i = 0; i < n; i++)
    {
        QString filterName = ui->lstFilters->item(i)->text();
        ImageFilterBase *filter = createImageFilter(filterName);

        filter->process(clone);

        delete filter;
    }

    QPixmap pixmap = UIUtils::Mat8ToQPixmap(clone);
    ui->gv->scene()->addPixmap(pixmap);
}
