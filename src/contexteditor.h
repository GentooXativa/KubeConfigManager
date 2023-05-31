#ifndef CONTEXTEDITOR_H
#define CONTEXTEDITOR_H

#include <QDialog>
#include <QStringListModel>

#include "KubeConfManager.h"
#include "kubeconfigutils.h"

namespace Ui
{
    class ContextEditor;
}

class ContextEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ContextEditor(KubeContext *context, KubeConfig *kConfig, QWidget *parent = nullptr);
    ~ContextEditor();

signals:
    void contextSaved(KubeContext *);

private slots:
    void on_pushButtonSave_clicked();
    void on_pushButtonCancel_clicked();

    void checkContextName();

private:
    Ui::ContextEditor *ui;
    KubeContext *context;
    KubeConfig *config;
};

#endif // CONTEXTEDITOR_H
