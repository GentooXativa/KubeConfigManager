#include "createnewkubeconfigdialog.h"
#include "ui_createnewkubeconfigdialog.h"

CreateNewKubeConfigDialog::CreateNewKubeConfigDialog(QString currentFolder, bool defaultConfigExists, QWidget *parent) : QWidget(parent),
                                                                                                                         ui(new Ui::CreateNewKubeConfigDialog)
{
    ui->setupUi(this);
    ui->labelWarning->hide();
    ui->lineEditFolder->setText(currentFolder);

    if (!defaultConfigExists)
        ui->lineEditFilename->setText("config");
    else
        ui->lineEditFilename->setText("my-new-config");

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CreateNewKubeConfigDialog::checkFile);
    connect(ui->lineEditFilename, &QLineEdit::textChanged, this, [=](const QString &input)
            {
                QString filepath(QString("%1/%2").arg(currentFolder, input));

                QFile file(filepath);

                if (file.exists())
                {
                    ui->lineEditFilename->setStyleSheet("color: rgb(240, 40, 40)");
                    ui->labelWarning->setText(tr("**ERROR**: file already exists, please use a different name."));
                    ui->labelWarning->show();
                }
                else
                {
                    ui->lineEditFilename->setStyleSheet("");
                    ui->labelWarning->hide();
                } });
}

CreateNewKubeConfigDialog::~CreateNewKubeConfigDialog()
{
    delete ui;
}

void CreateNewKubeConfigDialog::checkFile()
{

    QString filepath(QString("%1/%2").arg(ui->lineEditFolder->text(), ui->lineEditFilename->text()));

    QFile file(filepath);

    if (file.exists())
    {
        ui->lineEditFilename->setStyleSheet("color: rgb(240, 40, 40)");
        ui->labelWarning->setText(tr("**ERROR**: file already exists, please use a different name."));
        ui->labelWarning->show();

        return;
    }
    else
    {
        ui->lineEditFilename->setStyleSheet("");
        ui->labelWarning->hide();
    }

    emit createFile(filepath);
}