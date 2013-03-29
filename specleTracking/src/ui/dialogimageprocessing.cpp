#include "dialogimageprocessing.h"
#include "ui_dialogimageprocessing.h"

#include <QDebug>
#include <QInputDialog>

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
    ImageFilterBase *filter = createImageFilter(selected);
    QListWidgetItem *item = new QListWidgetItem(filter->getInfo(), ui->lstFilters);
    filter->setParent(this);
    QVariant v = QVariant::fromValue((QObject*)filter);
    item->setData(Qt::UserRole, v);
}

void DialogImageProcessing::on_btnRemove_clicked()
{
    QList<QListWidgetItem*> selectedItems = ui->lstFilters->selectedItems();
    qDeleteAll(selectedItems);
}

ImageFilterBase *DialogImageProcessing::createImageFilter(const QString &name)
{
    int kSize, windowSize, templateSize;
    float regulation, alfa, beta;
    switch(filterNames.indexOf(name))
    {
    case 0:
        kSize = QInputDialog::getInt(this, "Kernel size", "Kernel size", 7, 1, 31, 2);
        return new ImageFilterMedian(kSize);
    case 1:
        return new ImageFilterHistEq();
    case 2:
        regulation = QInputDialog::getDouble(this, "Regulation", "Regulation", 3, 0.1, 10, 2);
        windowSize = QInputDialog::getInt(this, "Window size", "Window size", 21, 1, 31, 2);
        templateSize = QInputDialog::getInt(this, "Template size", "Template size", 7, 1, 31, 2);
        return new ImageFilterNlMeansDenoise(regulation, windowSize, templateSize);
    case 3:
        alfa = QInputDialog::getDouble(this, "Alfa", "Alfa", 2, 0.1, 10, 2);
        beta = QInputDialog::getDouble(this, "Beta", "Beta", 0, -5, 5, 2);
        return new ImageFilterContrast(alfa, beta);
    default:
        kSize = QInputDialog::getInt(this, "Kernel size", "Kernel size", 11, 1, 31, 2);
        return new ImageFilterEdge(kSize);
    }
}

ImageFilterBase *getFilterFromListWidgetItem(QListWidgetItem *item)
{
    QObject *obj = qvariant_cast<QObject *>(item->data(Qt::UserRole));
    return qobject_cast<ImageFilterBase *>(obj);
}

void DialogImageProcessing::on_btnApply_clicked()
{
    ui->gv->scene()->clear();
    Mat8 clone = image.clone();

    int n = ui->lstFilters->count();
    for (int i = 0; i < n; i++)
    {
        ImageFilterBase *filter = getFilterFromListWidgetItem(ui->lstFilters->item(i));
        filter->process(clone);
    }

    QPixmap pixmap = UIUtils::Mat8ToQPixmap(clone);
    ui->gv->scene()->addPixmap(pixmap);
}

QList<ImageFilterBase*> DialogImageProcessing::getFilters()
{
    QList<ImageFilterBase*> result;
    int n = ui->lstFilters->count();
    for (int i = 0; i < n; i++)
    {
        ImageFilterBase *filter = getFilterFromListWidgetItem(ui->lstFilters->item(i));
        filter->setParent(0);
        result.append(filter);
    }
    return result;
}
