#include "dialogvideodataclipmetadata.h"
#include "ui_dialogvideodataclipmetadata.h"

DialogVideoDataClipMetadata::DialogVideoDataClipMetadata(VideoDataClipMetadata *metadata, QWidget *parent) :
    QDialog(parent), metadata(metadata), ui(new Ui::DialogVideoDataClipMetadata)
{
    ui->setupUi(this);

    model = new ModelListOfInts(this->metadata->beatIndicies, this);
    ui->listView->setModel(model);
}

DialogVideoDataClipMetadata::~DialogVideoDataClipMetadata()
{
    delete ui;
}

void DialogVideoDataClipMetadata::on_btnAdd_clicked()
{
    int i = ui->listView->model()->rowCount();
    model->insertRow(i);
    QModelIndex index = model->index(i);
    model->setData(QModelIndex(index), ui->sbNewValue->value());
}
