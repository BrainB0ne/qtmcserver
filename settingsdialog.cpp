/*
 * Qt Minecraft Server
 * Copyleft 2013
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        if(downloadDlg->exec() == QDialog::Accepted)
        {
            QString saveLocation = downloadDlg->getSaveLocation();

            if(!saveLocation.isEmpty())
            {
                ui->mcServerFileLineEdit->setText(saveLocation);
            }
        }

        delete downloadDlg;
        downloadDlg = 0;
    }
}
