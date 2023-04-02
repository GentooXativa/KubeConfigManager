#ifndef SWITCHCONTEXTFORM_H
#define SWITCHCONTEXTFORM_H

#include <QWidget>

namespace Ui {
class SwitchContextForm;
}

class SwitchContextForm : public QWidget
{
    Q_OBJECT

public:
    explicit SwitchContextForm(QWidget *parent = nullptr);
    ~SwitchContextForm();

private:
    Ui::SwitchContextForm *ui;
};

#endif // SWITCHCONTEXTFORM_H
