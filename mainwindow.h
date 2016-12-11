#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <vector>
#include <QVBoxLayout>
#include <string>
#include <QRegExp>
#include <QStringList>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createWeightsWidget();
    void createGradesWidget();
    template <typename T1, typename T2>
    QWidget* getStackedWidgets(T1 top, T2 bottom);
    template <typename T>
    void hideElements(T weights);
    void getWeightInput();
    void calculateFinal();
    void getGradeInput();
    void parseGradeInput(QString input);
    void getResults();

public slots:
    void getNumberOfWeights();
    void getWeights();
    void showResults();
    void reset();
    void saveResults();

private:
    Ui::MainWindow *ui;
    int numOfWeights = 0;
    std::vector<QWidget*> weights;
    std::vector<QWidget*> grades;
    std::vector<QString> weightNames;
    std::vector<double> weightPercentages;
    std::vector<QStringList> gradePercentages;
    std::vector<double> scores;
    QWidget* gradesNeeded;
};

#endif // MAINWINDOW_H
