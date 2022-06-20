#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    std::string line;
    std::ifstream file;
    file.open("..\\PredictionTrie\\words.txt");
    if (!file.is_open()) {
        throw "file is not opened";
    }
    while (getline(file, line)) {
        std::stringstream lineStream(line);
        std::string tmpString;
        while (getline(lineStream, tmpString, ' ')) {
                words->insert(tmpString);
        }
    }
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_lineEdit_textEdited(const QString &arg1)
{
    QStringList complete;
    auto vectorMatch = words->findBestMatches(arg1.toStdString(), 5);
    for (auto word : vectorMatch) {
        complete << QString::fromStdString(word);
    }
    QCompleter* completer = new QCompleter(complete);
    ui->lineEdit->setCompleter(completer);
}

