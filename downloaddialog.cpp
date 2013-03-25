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

#include "downloaddialog.h"
#include "ui_downloaddialog.h"

#define MCSERVER_URL "https://s3.amazonaws.com/MinecraftDownload/launcher/minecraft_server.jar"

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
    startDownload();
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

void DownloadDialog::doDownload(const QUrl &url)
{
    QString strUrl = url.toString();
}

QString DownloadDialog::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = QFileInfo(path).fileName();

    if (basename.isEmpty())
    {
        basename = "minecraft_server";
    }

    if (QFile::exists(basename))
    {
        // already exists, don't overwrite
        int i = 0;
        basename += '.';
        while (QFile::exists(basename + QString::number(i) + QString(".jar")))
        {
            ++i;
        }

        basename += QString::number(i);
        basename += QString(".jar");
    }

    return basename;
}

void DownloadDialog::startDownload()
{
    QUrl url = QUrl(MCSERVER_URL);
    doDownload(url);
}
