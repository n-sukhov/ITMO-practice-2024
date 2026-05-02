#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QChartView>
#include <QSplineSeries>
#include <QLabel>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_pushButton_clicked();

    void on_verticalSlider_valueChanged(int value);

    void on_lineEdit_5_textChanged(const QString &arg1);

    void on_lineEdit_6_textChanged(const QString &arg1);

    void on_lineEdit_7_textChanged(const QString &arg1);

    void on_lineEdit_8_textChanged(const QString &arg1);

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;

    QLineSeries *series = new QLineSeries();
    QLineSeries *series2 = new QLineSeries();
    QLineSeries *series3 = new QLineSeries();
    QLineSeries *seriesk = new QLineSeries();
    QLineSeries *series4 = new QLineSeries();

    QChart *chart = new QChart();
    QChart *chart2 = new QChart();
    QChart *chart3 = new QChart();
    QChart *chart4 = new QChart();

    QChartView *chartview = new QChartView(chart);
    QChartView *chartview2 = new QChartView(chart2);
    QChartView *chartview3 = new QChartView(chart3);
    QChartView *chartview4 = new QChartView(chart4);

    QPen pen = series->pen();
    QPen pen2 = series2->pen();
    QPen pen3 = series3->pen();
    QPen penk = seriesk->pen();
    QPen pen4 = series4->pen();

    QValueAxis *axisY = new QValueAxis;
    QValueAxis *axisY2 = new QValueAxis;
    QValueAxis *axisY3 = new QValueAxis;
    QValueAxis *axisY4 = new QValueAxis;

    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisX2 = new QValueAxis;
    QValueAxis *axisX3 = new QValueAxis;
    QValueAxis *axisX4 = new QValueAxis;
};
#endif // MAINWINDOW_H
