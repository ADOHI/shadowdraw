#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include "drawwidget.h"
#include <QResizeEvent>
#include "seamcarving.h"
#include "energydistview.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *event);

signals:

    void sendEnergyDest(QImage &i);
private slots:
    void openAction();
    void saveAsAction();
    void removeSeamAction();

private:
    SeamCarving* sc;
    EnergyDistView ed_view;

    Ui::MainWindow *ui;
    DrawWidget *drawArea;

    QImage image;
};

#endif // MAINWINDOW_H
