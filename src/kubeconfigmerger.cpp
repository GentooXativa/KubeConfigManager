#include "kubeconfigmerger.h"
#include "ui_kubeconfigmerger.h"

KubeConfigMerger::KubeConfigMerger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KubeConfigMerger)
{
    ui->setupUi(this);
}

KubeConfigMerger::~KubeConfigMerger()
{
    delete ui;
}
