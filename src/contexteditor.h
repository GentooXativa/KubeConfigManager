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
    explicit ContextEditor(KubeContext *context, KubeConfig *kConfig, bool isNew = false, QWidget *parent = nullptr);
    ~ContextEditor();

signals:
    void contextSaved(KubeContext *);

private slots:
    void onPushButtonSaveClicked();
    void onPushButtonCancelClicked();

    void checkContextName();

private:
    void setIconsAndActions();
    Ui::ContextEditor *ui;
    KubeContext *context;
    KubeConfig *config;

    bool isNewContext;
};

#endif // CONTEXTEDITOR_H
