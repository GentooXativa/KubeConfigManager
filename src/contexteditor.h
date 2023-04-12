#ifndef CONTEXTEDITOR_H
#define CONTEXTEDITOR_H

#include <QWidget>
#include "KubeConfManager.h"
#include "kubeconfigutils.h"
#include <QStringListModel>

namespace Ui {
class ContextEditor;
}

class ContextEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ContextEditor( KubeContext *context, KubeConfig *kConfig, QWidget *parent = nullptr);
    ~ContextEditor();

private:
    Ui::ContextEditor *ui;
    KubeContext *context;
    KubeConfig *config;
};

#endif // CONTEXTEDITOR_H
