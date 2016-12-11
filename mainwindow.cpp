#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief Constructor for main application window
 * @param parent The parent node to which this window will conform to
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    reset();
    connect(ui->saveButton, SIGNAL (clicked()), this, SLOT (saveResults()));
}

void MainWindow::saveResults() {
    qDebug() << "Saving";
}

/**
 * @brief Get number of weights from the user.
 */
void MainWindow::getNumberOfWeights() {
    numOfWeights = ui->weightSpinBox->value();
    ui->instructionLabel->setText("Enter all of your weights below.\nSeperate with a new line.");
    //give options to enter weights and name them
    createWeightsWidget();
    //diconnect signals
    ui->nextButton->disconnect();
    //connect button to next stage
    connect(ui->nextButton, SIGNAL (clicked()), this, SLOT (getWeights()));
}

/**
 * @brief Based on number of weights the user asked for create
 *        the needed inputs.
 */
void MainWindow::createWeightsWidget() {
    qDebug() << "Make Weights.";
    ui->weightSpinBox->hide();
    //create input for the number of weights
    for (int i = 0; i < numOfWeights; i++) {
        std::string title;
        (i + 1) == numOfWeights ? title = "Final" : title = "Weight " + std::to_string(i + 1);
        QLabel* tempLabel = new QLabel(title.c_str());
        tempLabel->setAlignment(Qt::AlignCenter);
        QLineEdit* tempInput = new QLineEdit();
        tempInput->setAlignment(Qt::AlignCenter);
        tempInput->setText("0");
        //add label and line edit to vertical layout
        QWidget* tempLayout = getStackedWidgets(tempLabel, tempInput);
        ui->horizontalLayout->addWidget(tempLayout);
        weights.push_back(tempLayout);
    }
}

/**
 * @brief Get the weights the user input
 */
void MainWindow::getWeights() {
    //hide weight inputs
    hideElements(weights);

    ui->instructionLabel->setText("Enter your scores below.");
    //give column to enter grades under each named weight
    createGradesWidget();
    ui->nextButton->setText("Calculate");
    //diconnect signals
    ui->nextButton->disconnect();
    //connect button to next stage
    connect(ui->nextButton, SIGNAL (clicked()), this, SLOT (showResults()));
}

/**
 * @brief Based on number of weights create inputs for the users grades
 */
void MainWindow::createGradesWidget() {
    qDebug() << "Make grades.";
    getWeightInput();
    //create inputs
    for (int i = 0; i < numOfWeights - 1; i++) {
        std::string title = weightNames[0].toStdString() + ": " + std::to_string(weightPercentages[i]);
        QLabel* tempLabel = new QLabel(title.c_str());
        tempLabel->setAlignment(Qt::AlignCenter);
        QTextEdit* tempInput = new QTextEdit();
        tempInput->setAlignment(Qt::AlignCenter);
        tempInput->setText("0.0");
        //add label and line edit to vertical layout
        QWidget* tempLayout = getStackedWidgets(tempLabel, tempInput);
        ui->horizontalLayout->addWidget(tempLayout);
        grades.push_back(tempLayout);
    }
}

/**
 * @brief Get name and weights from input
 */
void MainWindow::getWeightInput() {
    for (auto item : weights) {
        weightNames.push_back(item->findChildren<QLabel*>()[0]->text());
        weightPercentages.push_back(item->findChildren<QLineEdit*>()[0]->text().toDouble());
    }
}

/**
 * @brief Calculate grade needed for final
 * @return returns grades needed
 */
void MainWindow::calculateFinal() {
    double currentGrade = 0.0;
    for (int i = 0; i < numOfWeights - 1; i++) {
        double sum = 0.0;
        for (auto grade : gradePercentages[i]) {
            if (grade.contains("/")) {
                auto operands = grade.split(QRegExp("(\\/)"));
                sum += (operands[0].toDouble() / operands[1].toDouble()) * 100;
            } else if (grade == "" || !QRegExp("(\\d*)").exactMatch(grade)) {
                gradePercentages[i].removeOne(grade);
            }
            else {
                sum += grade.toDouble();
            }
        }
        currentGrade += (sum / gradePercentages[i].size()) * (weightPercentages[i] / 100);
    }
    //add current grade to scores
    scores.push_back(currentGrade);
    //add score needed on final for each grade level
    for (int i = 90; i > 40; i -= 10) {
        scores.push_back(((i - currentGrade) / weightPercentages[numOfWeights - 1]) * 100);
    }
}

/**
 * @brief Break up users grades into actual distinct strings of grades
 * @param input The string of grade the user input
 */
void MainWindow::parseGradeInput(QString input) {
    QRegExp rx("(\\ |\\,|\\t|\\n)"); //RegEx for ' ' or ',' or '.' or ':' or '\t'
    QStringList items = input.split(rx);
    gradePercentages.push_back(items);
}

/**
 * @brief Get users grades input
 */
void MainWindow::getGradeInput() {
    for (auto item : grades) {
        QString temp = item->findChildren<QTextEdit*>()[0]->toPlainText();
        parseGradeInput(temp);
    }
}

/**
 * @brief Show calculated results to the GUI
 */
void MainWindow::showResults() {
    getResults();
    hideElements(grades);
    ui->instructionLabel->setText("Current Grade: " + QString::number(scores[0]) + "\n\nGrades Needed");
    QString gradeString[] = {"A","B","C","D","F"};
    gradesNeeded = new QWidget();
    QVBoxLayout* tempLayout = new QVBoxLayout();
    for (unsigned int i = 1; i < scores.size(); i++) {
        QLabel* tempLabel = new QLabel();
        tempLabel->setText(gradeString[i - 1] + ": " + QString::number(scores[i]));
        tempLabel->setAlignment(Qt::AlignCenter);
        tempLayout->addWidget(tempLabel);
    }
    gradesNeeded->setLayout(tempLayout);
    ui->horizontalLayout->addWidget(gradesNeeded);
    ui->nextButton->setText("Finish");
    ui->saveButton->show();
    //diconnect signals
    ui->nextButton->disconnect();
    //connect button to next stage
    connect(ui->nextButton, SIGNAL (clicked()), this, SLOT (reset()));
}

/**
 * @brief Get results based off of all user input
 */
void MainWindow::getResults() {
    getGradeInput();
    //calculate grade needed
    calculateFinal();
}

/**
 * @brief Reset application to base state
 */
void MainWindow::reset() {   
    this->setWindowTitle("Weighted Finals Calculator");
    ui->setupUi(this);
    //diconnect signals
    ui->nextButton->disconnect();

    ui->saveButton->hide();
    connect(ui->nextButton, SIGNAL (clicked()), this, SLOT (getNumberOfWeights()));
    for (auto item : weights) {
        ui->horizontalLayout->removeWidget(item);
        delete item;
    }
    for (auto item : grades) {
        ui->horizontalLayout->removeWidget(item);
        delete item;
    }
    scores.clear();
    weights.clear();
    grades.clear();
    numOfWeights = 0;
    gradePercentages.clear();
    weightNames.clear();
    weightPercentages.clear();
    ui->horizontalLayout->removeWidget(gradesNeeded);
    delete gradesNeeded;
    gradesNeeded = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 *  @brief Method to stack two widgets vertically
 *  @param T1 Top widget
 *  @param T2 bottom widget
 */
template <typename T1, typename T2>
QWidget* MainWindow::getStackedWidgets(T1 top, T2 bottom) {
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(top);
    layout->addWidget(bottom);
    QWidget* shell = new QWidget();
    shell->setLayout(layout);
    return shell;
}

/**
 * @brief Method to hide an array of elements
 */
template <typename T>
void MainWindow::hideElements(T elements) {
    for (auto item : elements)
        item->hide();
}

