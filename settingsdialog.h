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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    void initialize();

    void setMinecraftServerPath(const QString& mcServerPath) {m_mcServerPath = mcServerPath;}
    QString getMinecraftServerPath() {return m_mcServerPath;}

    void setCustomJavaPath(const QString& customJavaPath) {m_customJavaPath = customJavaPath;}
    QString getCustomJavaPath() {return m_customJavaPath;}

    void setIsCustomJavaPath(bool isCustomJavaPath) {m_isCustomJavaPath = isCustomJavaPath;}
    bool isCustomJavaPath() {return m_isCustomJavaPath;}

private slots:
    void on_downloadButton_clicked();
    void on_javaBrowseButton_clicked();
    void on_mcServerBrowseButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::SettingsDialog *ui;

    QString m_customJavaPath;
    QString m_mcServerPath;
    bool m_isCustomJavaPath;
};

#endif // SETTINGSDIALOG_H
