#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "downloaddialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_downloadButton_clicked()
{
    DownloadDialog* downloadDlg = new DownloadDialog(this);

    if(downloadDlg)
    {
        downloadDlg->exec();

        delete downloadDlg;
        downloadDlg = 0;
    }
}
