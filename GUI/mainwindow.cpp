#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "matrixeditor.h"
#include "myexprtk.h"
#include <QAction>
#include <iostream>
#include <map>
#include <string>
std::map<std::string, Eigen::MatrixXd> MatrixMap;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // resize columns to fill table
  QHeaderView *Header = ui->MatrixTable->horizontalHeader();
  Header->setSectionResizeMode(QHeaderView::Stretch);

  // set icon
  const QString IconPath = "./icon/";
  ui->actionChinese->setIcon(
      QIcon(IconPath + "Flag_of_the_People's_Republic_of_China.svg.png"));
  ui->actionEnglish->setIcon(
      QIcon(IconPath + "Flag_of_the_United_Kingdom_(1-2).svg.png"));

  // connect translator actions
  connect(ui->actionChinese, SIGNAL(triggered()), this,
          SLOT(SetLanguageChinese()));
  connect(ui->actionEnglish, SIGNAL(triggered()), this,
          SLOT(SetLanguageEnglish()));

  // connect help and about message
  connect(ui->actionAbout_2, SIGNAL(triggered()), this, SLOT(ShowAboutMsg()));
  connect(ui->actionShowHelpMsg, SIGNAL(triggered()), this,
          SLOT(ShowHelpMsg()));

  // init
  this->setWindowTitle(tr("Matrix Calculator"));
}

MainWindow::~MainWindow() { delete ui; }

// https://doc.qt.io/qt-6/i18n-source-translation.html#mark-translatable-data-text-strings
// Qt Widgets: Override changeEvent
void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    //    ui->retranslateUi(this);
    this->setWindowTitle(tr("Matrix Calculator"));
    ui->NewButton->setText(tr("New Matrix"));
    ui->DeleteMButton->setText(tr("Delete Matrix"));
    ui->InverseButton->setText(tr("Inverse"));
    ui->DeterminantButton->setText(tr("Determinant"));
    ui->MatrixTable->horizontalHeaderItem(0)->setText(tr("Name"));
    ui->MatrixTable->horizontalHeaderItem(1)->setText(tr("Dimension"));
    ui->menuLanguage->setTitle(tr("Language"));
    ui->menuHelp->setTitle(tr("Help"));
    ui->actionShowHelpMsg->setText(tr("Get help"));
    ui->actionAbout_2->setText(tr("About"));
  } else
    QWidget::changeEvent(event);
}

void MainWindow::ShowAboutMsg(void) {
  QMessageBox *AboutMsg = new QMessageBox();
  AboutMsg->setWindowTitle(tr("About"));
  AboutMsg->setText(
      tr("### Matrix Calculator 1.0-rc1\n\n"
         "Based on Qt 6\n\n") +
#if __linux__
      tr("Built on Linux Gentoo\n\n")
#elif _WIN32
      tr("Built on Windows\n\n")
#endif
      + tr("Copyright 2023 (C) **Jason Li** <lzecheng98@gmail.com>\n\n"
           "**License: GPL-3.0** \n\n"
           "This is free software: you can redistribute it and/or modify it "
           "under the terms of the GNU General Public License as published by "
           "the Free Software Foundation, either version 3 of the License, or "
           "(at your option) any later version.\n\n"
           "This software is distributed in the hope that it will be useful, "
           "but WITHOUT ANY WARRANTY; without even the implied warranty of "
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU "
           "General Public License for more details.\n\n"));
  AboutMsg->setTextFormat(Qt::TextFormat::MarkdownText);
  AboutMsg->show();
}

void MainWindow::ShowHelpMsg(void) {
  QMessageBox *HelpMsg = new QMessageBox();
  HelpMsg->setWindowTitle(tr("Help"));
  HelpMsg->setText(
      tr("### Functions\n\n"
         "We support expression "
         "[(examples)](http://www.partow.net/programming/exprtk/) input with "
         "variables to be defined, such as ``sqrt(1 - (x^2)) + pi``, and you "
         "will be asked to input the value\n\n"
         "**Result will be stored in \"ans\"**\n\n"
         "Result for determinant will be stored in \"ans\" as well, so pay "
         "attention if you do so\n\n"
         "The function **A x k**, where k is a constant, if you want to do **A "
         "/ k**, use **k** as \"1/k\"\n\n"
         "Enjoy Computing!\n\n"
         "### Credits\n\n"
         "[Eigen](http://eigen.tuxfamily.org/) for matrix calculation\n\n"
         "[Exprtk](http://www.partow.net/programming/exprtk/) for parsing "
         "expression"));
  HelpMsg->setTextFormat(Qt::TextFormat::MarkdownText);
  HelpMsg->show();
}

void MainWindow::SetLanguageChinese(void) {
  if (ui->actionEnglish->isChecked())
    ui->actionEnglish->setChecked(0);
  ui->actionChinese->setChecked(1);
  if (m_Translator.load("./Translate_zh_CN.qm"))
    qApp->installTranslator(&m_Translator);
}

void MainWindow::SetLanguageEnglish(void) {
  if (ui->actionChinese->isChecked())
    ui->actionChinese->setChecked(0);
  ui->actionEnglish->setChecked(1);
  if (m_Translator.load("./Translate_en.qm"))
    qApp->installTranslator(&m_Translator);
}

void MainWindow::UpdateTable(void) {
  QFont ItemFont("Cantarel", 12, QFont::Bold);
  ui->MatrixTable->setRowCount(0);
  int count = 0;
  std::map<std::string, Eigen::MatrixXd>::iterator MapIter;
  MapIter = MatrixMap.begin();
  while (MapIter != MatrixMap.end()) {
    QTableWidgetItem *IterName = new QTableWidgetItem;
    QTableWidgetItem *IterSize = new QTableWidgetItem;
    IterName->setText(QString::fromStdString(MapIter->first));
    IterSize->setText(
        QString::fromStdString(std::to_string(MapIter->second.rows())) + "x" +
        QString::fromStdString(std::to_string(MapIter->second.cols())));
    IterName->setFont(ItemFont);
    IterSize->setFont(ItemFont);
    IterName->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    IterSize->setFlags(Qt::ItemIsUserCheckable ^ Qt::ItemIsSelectable);
    ui->MatrixTable->insertRow(count);
    ui->MatrixTable->setItem(count, 0, IterName);
    ui->MatrixTable->setItem(count, 1, IterSize);
    count++;
    MapIter++;
  }
}

void MainWindow::on_NewButton_clicked() {
  MatrixEditor *editor = new MatrixEditor();
  connect(editor, SIGNAL(EditDone()), this, SLOT(UpdateTable()));
  editor->show();
}

void MainWindow::on_MatrixTable_cellDoubleClicked(
    int row, int column) // open MatrixEditor
{
  std::string MatrixName =
      ui->MatrixTable->item(row, column)->text().toStdString();
  Eigen::MatrixXd Matrix(MatrixMap.find(MatrixName)->second.matrix());
  MatrixEditor *editor = new MatrixEditor(nullptr, &Matrix, MatrixName);
  connect(editor, SIGNAL(EditDone()), this, SLOT(UpdateTable()));
  editor->show();
}

QStringList MainWindow::GetAllNames(void) {
  QStringList MatrixNames;
  std::map<std::string, Eigen::MatrixXd>::iterator MapIter;
  MapIter = MatrixMap.begin();
  while (MapIter != MatrixMap.end()) {
    MatrixNames.append(QString::fromStdString(MapIter->first));
    MapIter++;
  }
  return MatrixNames;
}

void MainWindow::on_DeleteMButton_clicked() {
  QStringList MatrixNames(GetAllNames());
  if (MatrixNames.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No matrix found"),
                          QMessageBox::Yes);
    return;
  }
  bool ok;
  QString item = QInputDialog::getItem(this, tr("Delete Matrix"), tr("Choose:"),
                                       MatrixNames, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    std::cout << item.toStdString() << std::endl;
    MatrixMap.erase(item.toStdString());
    UpdateTable();
  }
}

void MainWindow::OpofTwoMatrix(int Opcode, std::string OpString) {
  QStringList MatrixNames(GetAllNames());
  if (MatrixNames.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No matrix found"),
                          QMessageBox::Yes);
    return;
  }
  bool okA;
  QString NameA = QInputDialog::getItem(this, QString::fromStdString(OpString),
                                        tr("Choose matrix A:"), MatrixNames, 0,
                                        false, &okA);
  bool okB;
  QString NameB = QInputDialog::getItem(this, QString::fromStdString(OpString),
                                        tr("Choose matrix B:"), MatrixNames, 0,
                                        false, &okA);
  if (okA && okB && !NameA.isEmpty() && !NameB.isEmpty()) {
    Eigen::MatrixXd MA(MatrixMap.find(NameA.toStdString())->second.matrix());
    Eigen::MatrixXd MB(MatrixMap.find(NameB.toStdString())->second.matrix());
    Eigen::MatrixXd Mans;
    switch (Opcode) {
    case ADD:
      Mans = MA + MB;
      break;
    case SUB:
      Mans = MA - MB;
      break;
    case MLP:
      Mans = MA * MB;
      break;
    }
    MatrixMap.insert_or_assign("ans", Mans);
    UpdateTable();
  }
}

void MainWindow::on_MAddMButton_clicked() { OpofTwoMatrix(ADD, "A + B"); }

void MainWindow::on_MSubMButton_clicked() { OpofTwoMatrix(SUB, "A - B"); }

void MainWindow::on_MTimeMButton_clicked() { OpofTwoMatrix(MLP, "A x B"); }

void MainWindow::on_MScaleButton_clicked() {
  QStringList MatrixNames(GetAllNames());
  if (MatrixNames.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No matrix found"),
                          QMessageBox::Yes);
    return;
  }
  bool okA;
  QString NameA = QInputDialog::getItem(this, "A * k", tr("Choose matrix A:"),
                                        MatrixNames, 0, false, &okA);
  bool okB;
  QString NumQString =
      QInputDialog::getText(this, "A * k", tr("Input k"), QLineEdit::Normal,
                            tr("Accept expression"), &okB);
  // TODO
  double k = Myexprtk::myexprtk(NumQString.toStdString(),
                                tr("k complation error").toStdString());
  if (okA && okB && !NameA.isEmpty()) {
    Eigen::MatrixXd MA(MatrixMap.find(NameA.toStdString())->second.matrix());
    Eigen::MatrixXd Mans(MA * k);
    MatrixMap.insert_or_assign("ans", Mans);
    UpdateTable();
  }
}

void MainWindow::on_InverseButton_clicked() {
  QStringList MatrixNames(GetAllNames());
  if (MatrixNames.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No matrix found"),
                          QMessageBox::Yes);
    return;
  }
  bool ok;
  QString Name = QInputDialog::getItem(
      this, tr("Inverse"), tr("Choose matrix:"), MatrixNames, 0, false, &ok);
  if (ok && !Name.isEmpty()) {
    Eigen::MatrixXd Mans(MatrixMap.find(Name.toStdString())->second.matrix());
    Mans = Mans.inverse();
    MatrixMap.insert_or_assign("ans", Mans);
    UpdateTable();
  }
}

void MainWindow::on_DeterminantButton_clicked() {
  QStringList MatrixNames(GetAllNames());
  if (MatrixNames.isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("No matrix found"),
                          QMessageBox::Yes);
    return;
  }
  bool ok;
  QString Name = QInputDialog::getItem(
      this, tr("Inverse"), tr("Choose matrix:"), MatrixNames, 0, false, &ok);
  if (ok && !Name.isEmpty()) {
    Eigen::MatrixXd MA(MatrixMap.find(Name.toStdString())->second.matrix());
    double det = MA.determinant();
    std::cout << det << std::endl;
    Eigen::MatrixXd Mans(1, 1);
    Mans << det;
    MatrixMap.insert_or_assign("ans", Mans);
    UpdateTable();
  }
}
