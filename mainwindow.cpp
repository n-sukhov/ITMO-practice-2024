#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cmath>
#include <fstream>
#include <ctime>
#include <iostream>

void signal_generation(const char* filename, const int samples, const float period_sec, short max_frequency_clear, short min_frequency_clear);
void clear_signal_generation(const char* f_filename, const char* sign_filename, int samples, float period_sec);
void dft(const char* path_to_file, const char* freq_fname, float T);
void csv_count_cols_strs(const char* filename, int* str, int* col, char delimiter_col, char delimiter_str);
double* csv_read(const char* filename, int str, int col);
QLineSeries* get_func(const char* filename, double* minY, double* maxY, double Xstart, double Xend);
void amp_slice(const char* filename, const char * clear_freqs, float k_clear);
void freq_slice(const char* filename, const char* clear_freqs, double min_freq, double max_freq);

double Ymax = 1, Ymin = 0, Ymax2 = 1, Ymax2_G = 1, Ymin2 = 0, Ymax3 = 1, Ymin3 = 0, Ymax4 = 0, Ymin4 = 0, kmax = 1, two_pi = 6.2832;
double Xmin2_4 = 0, Xmax2_4 = 1, hor_s = 0, hor_e = 1;
int samples = 2, max_freq_gen, min_feq_gen, min_freq_clear, max_freq_clear;
float period = 1, k = 0;

const char* noised_signal_fname = "noised_signal.csv";
const char* frequencies_fname = "frequencies.csv";
const char* clear_frequencies_fname = "clear_frequencies.csv";
const char* clear_signal_fname = "clear_signal.csv";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Вывод полей для графиков
    chart->setTitle("График исходного сигнала");
    chart->addSeries(series);
    chart->setGeometry(0, 0, ui->chart_widget->width(), ui->chart_widget->height());
    chart->legend()->setVisible(false);
    axisY->setTitleText("A");
    axisY->setTickCount(5);
    axisY->setMinorTickCount(4);
    axisY->setMin(Ymin);
    axisY->setMax(Ymax);
    axisX->setTitleText("t");
    axisX->setTickCount(5);
    axisX->setMinorTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->setGeometry(0, 0, ui->chart_widget->width(), ui->chart_widget->height());
    chartview->setParent(ui->chart_widget);

    chart2->setTitle("Спектр частот исходного сигнала");
    chart2->addSeries(series2);
    chart2->addSeries(seriesk);
    chart2->setGeometry(0, 0, ui->chart_widget2->width(), ui->chart_widget2->height());
    chart2->legend()->setVisible(false);
    axisY2->setTitleText("A");
    axisY2->setTickCount(5);
    axisY2->setMinorTickCount(4);
    axisY2->setMin(Ymin2);
    axisY2->setMax(Ymax2);
    axisX2->setTitleText("ω");
    axisX2->setTickCount(5);
    axisX2->setMinorTickCount(5);
    chart2->addAxis(axisX2, Qt::AlignBottom);
    chart2->addAxis(axisY2, Qt::AlignLeft);
    series2->attachAxis(axisX2);
    series2->attachAxis(axisY2);
    seriesk->attachAxis(axisX2);
    seriesk->attachAxis(axisY2);
    seriesk->append(0, k * Ymax2);
    seriesk->append(period, k * Ymax2);
    penk.setWidth(1);
    penk.setBrush(QBrush("magenta"));
    seriesk->setPen(penk);
    chartview2->setRenderHint(QPainter::Antialiasing);
    chartview2->setGeometry(0, 0, ui->chart_widget2->width(), ui->chart_widget2->height());
    chartview2->setParent(ui->chart_widget2);

    chart3->setTitle("График фильтрованного сигнала");
    chart3->addSeries(series3);
    chart3->setGeometry(0, 0, ui->chart_widget3->width(), ui->chart_widget3->height());
    chart3->legend()->setVisible(false);
    axisY3->setTitleText("A");
    axisY3->setTickCount(5);
    axisY3->setMinorTickCount(4);
    axisY3->setMin(Ymin3);
    axisY3->setMax(Ymax3);
    axisX3->setTitleText("t");
    axisX3->setTickCount(5);
    axisX3->setMinorTickCount(5);
    chart3->addAxis(axisX3, Qt::AlignBottom);
    chart3->addAxis(axisY3, Qt::AlignLeft);
    series3->attachAxis(axisX3);
    series3->attachAxis(axisY3);
    chartview3->setRenderHint(QPainter::Antialiasing);
    chartview3->setGeometry(0, 0, ui->chart_widget3->width(), ui->chart_widget3->height());
    chartview3->setParent(ui->chart_widget3);

    chart4->setTitle("Спектр частот фильтрованного сигнала");
    chart4->addSeries(series4);
    chart4->setGeometry(0, 0, ui->chart_widget4->width(), ui->chart_widget4->height());
    chart4->legend()->setVisible(false);
    axisY4->setTitleText("A");
    axisY4->setTickCount(5);
    axisY4->setMinorTickCount(4);
    axisY4->setMin(Ymin4);
    axisY4->setMax(Ymax4);
    axisX4->setTitleText("ω");
    axisX4->setTickCount(5);
    axisX4->setMinorTickCount(5);
    chart4->addAxis(axisX4, Qt::AlignBottom);
    chart4->addAxis(axisY4, Qt::AlignLeft);
    series4->attachAxis(axisX4);
    series4->attachAxis(axisY4);
    chartview4->setRenderHint(QPainter::Antialiasing);
    chartview4->setGeometry(0, 0, ui->chart_widget4->width(), ui->chart_widget4->height());
    chartview4->setParent(ui->chart_widget4);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void signal_generation(const char* filename, const int samples, const float period_sec, short max_frequency_clear, short min_frequency_clear)
{
    /* Функция принимает на вход количество точек по оси x, период измерения сигнала в секундах, максимальную и минимальную частоту для генерации случайного сигнала.
    Создаёт csv файл с сигналом. */
    srand(time(NULL));
    int min_rand = 10;
    int max_rand = 200;
    short amp_min_clear = 400;
    short amp_max_clear = 1000;
    short amp_max_noise = 300;
    short max_frequncy_noise = max_frequency_clear * 100;
    int n = rand() % (max_rand - min_rand + 1) + min_rand;
    double* frequencies = new double[n] {};
    double* amplitudes = new double[n] {};
    int n_clear = n * (rand() % 16 + 5) / 100;
    // Генерация частот, амплитуд для чистого сигнала
    for (int i = 0; i < n_clear; ++i)
    {
        *(frequencies + i) = rand() % (max_frequency_clear - min_frequency_clear + 1) + min_frequency_clear + (rand() % 10) / 10;
        *(amplitudes + i) = rand() % (amp_max_clear - amp_min_clear + 1) + amp_min_clear + (rand() % 10) / 10;
    }
    // Генерация шумовых частот, амплитуд
    for (int i = n_clear; i < n; ++i)
    {
        *(frequencies + i) = rand() % max_frequncy_noise + (rand() % 10) / 10;
        *(amplitudes + i) = rand() % amp_max_noise + 1 + (rand() % 10) / 10;
    }
    double sec_per_x = period_sec / samples;
    std::ofstream File;
    File.open(filename);
    File.clear();
    for (int i = 0; i < samples; ++i)
    {
        double y = 0.0;
        File << sec_per_x * i << ',';
        for (int j = 0; j < n; ++j)
            y += *(amplitudes + j) * cos(*(frequencies + j) * sec_per_x * i * two_pi);
        File << y << '\n';
    }
    delete[] frequencies;
    delete[] amplitudes;
    File.close();
}

void clear_signal_generation(const char* f_filename, const char* sign_filename, int samples, float period_sec)
{
    /* Функция создаёт csv-файл с очищенным сигналом (принимает на вход csv-файл со списком частот и амплитуд) и возвращает имя файла*/
    int str, col;
    csv_count_cols_strs(f_filename, &str, &col, ',', '\n');
    int size = str * col;
    double sec_per_x = period_sec / samples;
    double *freqs = new double[size]{};
    freqs = csv_read(f_filename, str, col);
    std::ofstream File;
    File.open(sign_filename);
    File.clear();
    for (int i = 0; i < samples; ++i)
    {
        double clear_signal = 0.0;
        File << sec_per_x * i << ',';
        for (int x = 0, y = 1; y < size; x += 2, y += 2)
            clear_signal += *(freqs + y) * cos(*(freqs + x) * sec_per_x * i * two_pi);
        File << clear_signal << '\n';
    }
    File.close();
    delete[] freqs;
}

void dft(const char* filename, const char* freq_fname, float T)
{
    /* Принимает csv-файл с сигналом и файл для записи спектра частот, выполняет дискретное преобразование Фурье
    (только для вещественной части спектра)*/
    int N, col;
    csv_count_cols_strs(filename, &N, &col, ',', '\n');
    int Ns = N /T;
    if (T > 1)
        N = Ns;
    double* noised_signal= new double[N * col]{};
    noised_signal = csv_read(filename, N, col);
    std::ofstream File;
    File.open(freq_fname);
    File.clear();
    for (int k = 0; k < N / 2; ++k)
    {
        double a = 0.0;
        File << k << ',';
        for (int n = 0; n < N; ++n)
        {
            a += *(noised_signal + 1 + n * 2) * cos(two_pi * n * k / Ns);
        }
        File << std::abs(a) / N * 2 << '\n';
    }
    File.close();
    delete[] noised_signal;
}

void amp_slice(const char* filename, const char* clear_freqs, float k_clear)
{
    // Принимает csv-файл с зашумлёнными частотами и коэффициентом очистки, записывает в другой csv-файл список очищенных частот и амплитуд
    // k_clear должен быть от 0 до 1, все частоты, амплитуды которых меньше [k_clear * макс. значение амплитуды в списке], убираются
    int str, col;
    csv_count_cols_strs(filename, &str, &col, ',', '\n');
    int size = str * col;
    double* noised_frequencies = new double[size]{};
    noised_frequencies = csv_read(filename, str, col);
    double max_amp = 0;
    for (int y = 1; y < size; y += 2)
        if (*(noised_frequencies + y) > max_amp)
            max_amp = *(noised_frequencies + y);
    max_amp *= k_clear;
    std::ofstream File;
    File.open(clear_freqs);
    File.clear();
    for (int x = 0, y = 1; y < size; x += 2, y += 2)
    {
        File << *(noised_frequencies + x) << ',';
        if (*(noised_frequencies + y) < max_amp)
            File << 0.0 << '\n';
        else
            File << *(noised_frequencies + y) << '\n';
    }
    File.close();
    delete[] noised_frequencies;
}

void freq_slice(const char* filename, const char* clear_freqs, double min_freq, double max_freq)
{
    // Принимает csv-файл с зашумлёнными частотами и записывает в другой csv-файл список частот и их амплитуд от min_freq до max_freq
    int str, col;
    csv_count_cols_strs(filename, &str, &col, ',', '\n');
    int size = str * col;
    double* noised_frequencies = new double[size]{};
    noised_frequencies = csv_read(filename, str, col);
    std::ofstream File;
    File.open(clear_freqs);
    File.clear();
    for (int x = 0, y = 1; y < size; x += 2, y += 2)
    {
        if (*(noised_frequencies + x) < min_freq)
            File << *(noised_frequencies + x) << ',' << 0.0 << '\n';
        else if (*(noised_frequencies + x) <= max_freq)
            File << *(noised_frequencies + x) << ',' << *(noised_frequencies + y) << '\n';
        else
            File << *(noised_frequencies + x) << ',' << 0.0 << '\n';
    }
    File.close();
    delete[] noised_frequencies;
}

void csv_count_cols_strs(const char* filename, int* str, int* col, char delimiter_col, char delimiter_str)
{
    // По адресам указателей str и col записываются количество строк и столбцов в прочитанном csv файле.
    *str = 0, * col = 0;
    char a;
    std::ifstream File;
    File.open(filename);
    if (!File.is_open())
    {
        std::cout << "Can't open the file " << filename;
        exit(EXIT_FAILURE);
    }
    while ((a = File.get()) != delimiter_str && a != -1)
        if (a == delimiter_col)
            ++*col;
    if (a == delimiter_str)
        ++*col, ++*str;
    else
        std::cout << "Incorrect format of file" << filename;
    while ((a = File.get()) != -1)
        if (a == delimiter_str)
            ++*str;

    File.close();
    return;
}

double* csv_read(const char* filename, int str, int col)
{
    double* matrix = new double[str * col];
    std::ifstream File;
    File.open(filename);
    for (int i = 0; i < str; ++i)
        for (int j = 0; j < col; ++j)
        {
            (File >> *(matrix + i * col + j)).get();
        }
    File.close();
    return matrix;
}

QLineSeries* get_func(const char* filename, double* minY, double* maxY, double Xstart, double Xend)
{
    /* Читает данные для построения графика из csv-файла, возвращает серию точек для графика,
    записывает в minY и maxY минимальное и максимальное значение функции соответственно
    Xstart и Xend задают интервал рассматриваемых значение x из всех найденных в файле, от 0 до 1,
    где 0 - начало файла, 1 - конец */
    QLineSeries* func = new QLineSeries();
    int str, col;
    csv_count_cols_strs(filename, &str, &col, ',', '\n');
    int size = str * col;
    double *signal = new double[size]{};
    signal = csv_read(filename, str, col);
    if ((Xstart < 0.0) || (Xstart >= 1.0))
        Xstart = 0.0;
    if ((Xend > 1.0) || (Xend <= 0.0))
        Xend = 1.0;
    int min = size * Xstart;
    int max = size * Xend;
    if (min % 2 != 0)
        --min;
    for (int i = min, j = min + 1; j < max; i += 2, j += 2)
    {
        if (*maxY < signal[j])
            *maxY = signal[j];
        else if (*minY > signal[j])
            *minY = signal[j];
        func->append(signal[i], signal[j]);
    }
    delete [] signal;
    return func;
}

void MainWindow::on_pushButton_clicked()
{
    if (ui->lineEdit->isModified() && ui->lineEdit_2->isModified() && ui->lineEdit_3->isModified() && ui->lineEdit_4->isModified())
    {
        period = ui->lineEdit->text().toFloat();
        samples = ui->lineEdit_2->text().toInt();
        min_feq_gen = ui->lineEdit_3->text().toInt();
        max_freq_gen = ui->lineEdit_4->text().toInt();

        Ymin = 0, Ymax = 0, Ymin2 = 0, Ymax2 = 0, Ymin3 = 0, Ymax3 = 0, Ymin4 = 0, Ymax4 = 1, hor_s = 0, hor_e = 1;
        chart2->removeSeries(series2);

        signal_generation(noised_signal_fname, samples, period, max_freq_gen, min_feq_gen);
        series = get_func(noised_signal_fname, &Ymin, &Ymax, 0, 1);
        dft(noised_signal_fname, frequencies_fname, period);
        series2 = get_func(frequencies_fname, &Ymin2, &Ymax2, hor_s, hor_e);
        amp_slice(frequencies_fname, clear_frequencies_fname, k);
        clear_signal_generation(clear_frequencies_fname, clear_signal_fname, samples, period);
        series3 = get_func(clear_signal_fname, &Ymin3, &Ymax3, 0, 1);
        series4 = get_func(clear_frequencies_fname, &Ymin4, &Ymax4, hor_s, hor_e);
        Ymax2_G = Ymax2;
        kmax = 1;
        k = double(ui->verticalSlider->value()) / 100;
    }
    else
        return;

    // Отрисовка графиков
    pen.setWidth(1);
    pen.setBrush(QBrush("red"));
    series->setPen(pen);
    chart->removeAllSeries();
    chart->addSeries(series);
    axisY->setMin(Ymin);
    axisY->setMax(Ymax);
    axisX->setMax(period);

    Xmin2_4 = 0;
    Xmax2_4 = samples / 2;
    if (period > 1)
        Xmax2_4 /= period;
    pen2.setWidth(1);
    pen2.setBrush(QBrush("blue"));
    series2->setPen(pen2);
    chart2->addSeries(series2);
    axisY2->setMax(Ymax2);
    axisX2->setMin(Xmin2_4);
    axisX2->setMax(Xmax2_4);

    pen3.setWidth(1);
    pen3.setBrush(QBrush("red"));
    series3->setPen(pen3);
    chart3->removeAllSeries();
    chart3->addSeries(series3);
    axisY3->setMin(Ymin3);
    axisY3->setMax(Ymax3);
    axisX3->setMax(period);

    pen4.setWidth(1);
    pen4.setBrush(QBrush("blue"));
    series4->setPen(pen4);
    chart4->removeAllSeries();
    chart4->addSeries(series4);
    axisY4->setMax(Ymax4);
    axisX4->setMin(Xmin2_4);
    axisX4->setMax(Xmax2_4);

    chart2->removeSeries(seriesk);
    seriesk->clear();
    chart2->addSeries(seriesk);
    seriesk->append(0, k);
    seriesk->append(samples / 2 / period, k);
    penk.setWidth(1);
    penk.setBrush(QBrush("magenta"));
    seriesk->setPen(penk);

    ui->lineEdit_5->setText("");
    ui->lineEdit_6->setText("");
}


void MainWindow::on_verticalSlider_valueChanged(int value)
{
    ui->label_5->setText(QString::number(value));
    k = double(value) / 100 * kmax;
    if (ui->lineEdit->isModified() && ui->lineEdit_2->isModified() && ui->lineEdit_3->isModified() && ui->lineEdit_4->isModified())
    {
        Ymin3 = 0, Ymax3 = 0, Ymax4 = 0;
        amp_slice(frequencies_fname, clear_frequencies_fname, k/kmax);
        clear_signal_generation(clear_frequencies_fname, clear_signal_fname, samples, period);
        series3 = get_func(clear_signal_fname, &Ymin3, &Ymax3, 0, 1);
        chart4->removeSeries(series4);
        series4->clear();
        series4 = get_func(clear_frequencies_fname, &Ymin4, &Ymax4, hor_s, hor_e);

        pen3.setWidth(1);
        pen3.setBrush(QBrush("red"));
        series3->setPen(pen3);
        chart3->removeAllSeries();
        chart3->addSeries(series3);
        axisY3->setMin(Ymin3);
        axisY3->setMax(Ymax3);
        axisX3->setMax(period);

        pen4.setWidth(1);
        pen4.setBrush(QBrush("blue"));
        series4->setPen(pen4);
        chart4->addSeries(series4);
        axisY4->setMax(Ymax4);
        axisX4->setMin(Xmin2_4);
        axisX4->setMax(Xmax2_4);
    }
    chart2->removeSeries(seriesk);
    seriesk->clear();
    chart2->addSeries(seriesk);
    seriesk->append(0, k);
    seriesk->append(samples / 2 / period, k);
    penk.setWidth(1);
    penk.setBrush(QBrush("magenta"));
    seriesk->setPen(penk);
}


void MainWindow::on_lineEdit_5_textChanged(const QString &arg1)
{
    if (((hor_s = arg1.toDouble() / (samples / 2 / period)) >= 1.0) || (hor_s < 0.0))
        Xmin2_4 = 0;
    else
    {
        Xmin2_4 = samples / 2 * hor_s;
        if (period > 1)
            Xmax2_4 /= period;
    }
    kmax = Ymax2_G;
    chart2->removeSeries(series2);
    Ymax2 = 0;
    series2 = get_func(frequencies_fname, &Ymin2, &Ymax2, hor_s, hor_e);
    pen2.setWidth(1);
    pen2.setBrush(QBrush("blue"));
    series2->setPen(pen2);
    chart2->addSeries(series2);
    axisY2->setMax(Ymax2);
    axisX2->setMin(Xmin2_4);
    axisX2->setMax(Xmax2_4);
    kmax /= Ymax2;
    k *= kmax;

    chart4->removeSeries(series4);
    Ymax4 = 0;
    series4 = get_func(clear_frequencies_fname, &Ymin4, &Ymax4, hor_s, hor_e);
    pen4.setWidth(1);
    pen4.setBrush(QBrush("blue"));
    series4->setPen(pen4);
    chart4->addSeries(series4);
    axisY4->setMax(Ymax2);
    axisX4->setMin(Xmin2_4);
    axisX4->setMax(Xmax2_4);

    chart2->removeSeries(seriesk);
    seriesk->clear();
    chart2->addSeries(seriesk);
    seriesk->append(0, k);
    seriesk->append(Xmax2_4, k);
    penk.setWidth(1);
    penk.setBrush(QBrush("magenta"));
    seriesk->setPen(penk);
}


void MainWindow::on_lineEdit_6_textChanged(const QString &arg1)
{
    if (((hor_e = arg1.toDouble() / (samples / 2 / period)) > 1.0) || (hor_e <=  0.0))
    {
        Xmax2_4 = samples / 2;
        if (period > 1)
            Xmax2_4 /= period;
    }
    else
    {
        Xmax2_4 = samples / 2 * hor_e;
        if (period > 1)
            Xmax2_4 /= period;
    }
    kmax = Ymax2_G;
    chart2->removeSeries(series2);
    Ymax2 = 0;
    series2 = get_func(frequencies_fname, &Ymin2, &Ymax2, hor_s, hor_e);
    pen2.setWidth(1);
    pen2.setBrush(QBrush("blue"));
    series2->setPen(pen2);
    chart2->addSeries(series2);
    axisY2->setMax(Ymax2);
    axisX2->setMin(Xmin2_4);
    axisX2->setMax(Xmax2_4);
    kmax /= Ymax2;
    k *= kmax;

    chart4->removeSeries(series4);
    Ymax4 = 0;
    series4 = get_func(clear_frequencies_fname, &Ymin4, &Ymax4, hor_s, hor_e);
    pen4.setWidth(1);
    pen4.setBrush(QBrush("blue"));
    series4->setPen(pen4);
    chart4->addSeries(series4);
    axisY4->setMax(Ymax4);
    axisX4->setMin(Xmin2_4);
    axisX4->setMax(Xmax2_4);

    chart2->removeSeries(seriesk);
    seriesk->clear();
    chart2->addSeries(seriesk);
    seriesk->append(0, k);
    seriesk->append(Xmax2_4, k);
    penk.setWidth(1);
    penk.setBrush(QBrush("magenta"));
    seriesk->setPen(penk);
}




void MainWindow::on_lineEdit_7_textChanged(const QString &arg1)
{
    min_freq_clear = arg1.toDouble();
}


void MainWindow::on_lineEdit_8_textChanged(const QString &arg1)
{
    max_freq_clear = arg1.toDouble();
}


void MainWindow::on_pushButton_2_clicked()
{
    if (ui->lineEdit->isModified() && ui->lineEdit_2->isModified() && ui->lineEdit_3->isModified() && ui->lineEdit_4->isModified() && ui->lineEdit_7->isModified() && ui->lineEdit_8->isModified())
    {
        Ymin3 = 0, Ymax3 = 0, Ymax4 = 0;
        freq_slice(frequencies_fname, clear_frequencies_fname, min_freq_clear, max_freq_clear);
        clear_signal_generation(clear_frequencies_fname, clear_signal_fname, samples, period);
        series3 = get_func(clear_signal_fname, &Ymin3, &Ymax3, 0, 1);
        chart4->removeSeries(series4);
        series4->clear();
        series4 = get_func(clear_frequencies_fname, &Ymin4, &Ymax4, hor_s, hor_e);

        pen3.setWidth(1);
        pen3.setBrush(QBrush("red"));
        series3->setPen(pen3);
        chart3->removeAllSeries();
        chart3->addSeries(series3);
        axisY3->setMin(Ymin3);
        axisY3->setMax(Ymax3);
        axisX3->setMax(period);

        pen4.setWidth(1);
        pen4.setBrush(QBrush("blue"));
        series4->setPen(pen4);
        chart4->addSeries(series4);
        axisY4->setMax(Ymax4);
        axisX4->setMin(Xmin2_4);
        axisX4->setMax(Xmax2_4);
    }
    chart2->removeSeries(seriesk);
    seriesk->clear();
    chart2->addSeries(seriesk);
    seriesk->append(0, k);
    seriesk->append(samples / 2 / period, k);
    penk.setWidth(1);
    penk.setBrush(QBrush("magenta"));
    seriesk->setPen(penk);
}

