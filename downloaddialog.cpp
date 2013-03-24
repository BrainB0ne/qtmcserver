#include "downloaddialog.h"
#include "ui_downloaddialog.h"

DownloadDialog::DownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadDialog)
{
    ui->setupUi(this);

    m_saveLocation = "";
}

DownloadDialog::~DownloadDialog()
{
    delete ui;
}

void DownloadDialog::on_downloadButton_clicked()
{

}

void DownloadDialog::on_buttonBox_accepted()
{
    m_saveLocation = ui->saveLineEdit->text();

    accept();
}

void DownloadDialog::on_buttonBox_rejected()
{
    reject();
}
