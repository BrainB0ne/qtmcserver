#ifndef DOWNLOADDIALOG_H
#define DOWNLOADDIALOG_H

#include <QDialog>

namespace Ui {
class DownloadDialog;
}

class DownloadDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DownloadDialog(QWidget *parent = 0);
    ~DownloadDialog();

    QString getSaveLocation() {return m_saveLocation;}

private slots:
    void on_downloadButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::DownloadDialog *ui;

    QString m_saveLocation;
};

#endif // DOWNLOADDIALOG_H
