#ifndef KUBECONFIGMERGER_H
#define KUBECONFIGMERGER_H

#include <QWidget>

namespace Ui {
class KubeConfigMerger;
}

class KubeConfigMerger : public QWidget
{
    Q_OBJECT

public:
    explicit KubeConfigMerger(QWidget *parent = nullptr);
    ~KubeConfigMerger();

private:
    Ui::KubeConfigMerger *ui;
};

#endif // KUBECONFIGMERGER_H
