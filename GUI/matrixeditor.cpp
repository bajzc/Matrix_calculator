#include "matrixeditor.h"
#include "myexprtk.h"
#include "ui_matrixeditor.h"
#include <iostream>
#include <limits>
// The largest distance from (0,0), which should be the dimension of matrix.
static int MaxEditRow;
static int MaxEditCol;
const int BufferCell = 6;
extern std::map<std::string, Eigen::MatrixXd> MatrixMap;
bool ExprtkCompErrorFlag = 0;

MatrixEditor::MatrixEditor(QWidget *parent)
    : QWidget(parent), ui(new Ui::MatrixEditor) {
  ui->setupUi(this);
  MaxEditRow = 0;
  MaxEditCol = 0;
  for (int i = 0; i < BufferCell; i++) {
    ui->EditTable->insertRow(i);
    ui->EditTable->insertColumn(i);
  }
}

MatrixEditor::MatrixEditor(QWidget *parent, Eigen::MatrixXd *Matrix,
                           std::string MatrixName)
    : QWidget(parent), ui(new Ui::MatrixEditor) {
  ui->setupUi(this);
  MaxEditRow = Matrix->rows() - 1;
  MaxEditCol = Matrix->cols() - 1;
  if (MaxEditRow == 0) // otherwise no row will be created
    ui->EditTable->setRowCount(1);
  else
    ui->EditTable->setRowCount(MaxEditRow);
  if (MaxEditCol == 0)
    ui->EditTable->setColumnCount(1);
  else
    ui->EditTable->setColumnCount(MaxEditCol);
  for (int i = 0; i <= MaxEditRow; i++) {
    for (int j = 0; j <= MaxEditCol; j++) {
      QTableWidgetItem *Number = new QTableWidgetItem;
      Number->setText(QString::fromStdString(std::to_string((*Matrix)(i, j))));
      ui->EditTable->setItem(i, j, Number);
    }
  }
  ui->NameLineEdit->setText(QString::fromStdString(MatrixName));
  UpdateTable();
}

void MatrixEditor::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    //    ui->retranslateUi(this);
    this->setWindowTitle(tr("Variable Editor"));
  } else
    QWidget::changeEvent(event);
}

MatrixEditor::~MatrixEditor() { delete ui; }

// Make sure there is at least 8 more entries for both row and column
void MatrixEditor::UpdateTable(void) {
  int CurTableRow = ui->EditTable->rowCount();
  int CurTableCol = ui->EditTable->columnCount();
  while (CurTableRow - MaxEditRow != BufferCell) {
    if (CurTableRow - MaxEditRow < BufferCell)
      // to insert, it should be 1 after
      ui->EditTable->insertRow(CurTableRow);
    else
      // to remove, it should be exact that one
      ui->EditTable->removeRow(CurTableRow - 1);
    CurTableRow = ui->EditTable->rowCount();
  }
  while (CurTableCol - MaxEditCol != BufferCell) {
    if (CurTableCol - MaxEditCol < BufferCell)
      ui->EditTable->insertColumn(CurTableCol);
    else
      ui->EditTable->removeColumn(CurTableCol - 1);
    CurTableCol = ui->EditTable->columnCount();
  }
}

Eigen::MatrixXd MatrixEditor::ReadfromTable(void) {
  std::cout << "ReadfromTable: Max row:" << MaxEditRow
            << " Max col:" << MaxEditCol << std::endl;
  Eigen::MatrixXd NewMatrix(MaxEditRow + 1, MaxEditCol + 1);
  for (int i = 0; i <= MaxEditRow; i++) {
    for (int j = 0; j <= MaxEditCol; j++) {
      const char *NumChar =
          ui->EditTable->item(i, j)->text().toStdString().c_str();
      char *end = NULL;
      double num;
      const std::string NumString =
          ui->EditTable->item(i, j)->text().toStdString();
      num = Myexprtk::myexprtk(
          NumString, tr("In (").toStdString() + std::to_string(j + 1) + "," +
                         std::to_string(i + 1) +
                         tr(") variable compliation error").toStdString());
      NewMatrix(i, j) = num;
    }
  }
  std::cout << NewMatrix << std::endl;
  return NewMatrix;
}

void MatrixEditor::on_EditTable_cellChanged(int row, int column) {
  if (ui->EditTable->item(row, column)->text().isEmpty()) {
    if (row == MaxEditRow || column == MaxEditCol) {
      // shrink table
      int tmp_row = MaxEditRow;
      int tmp_col = MaxEditCol;
      MaxEditRow = 0;
      MaxEditCol = 0;
      // loop the table and get the non-empty entry
      for (int i = 0; i <= tmp_row; i++) {
        for (int j = 0; j <= tmp_col; j++) {
          if (ui->EditTable->item(i, j)) {
            if (!ui->EditTable->item(i, j)->text().isEmpty()) {
              if (i > MaxEditRow)
                MaxEditRow = i;
              if (j > MaxEditCol)
                MaxEditCol = j;
            }
          }
        }
      }
    }
  } else {
    if (row > MaxEditRow)
      MaxEditRow = row;
    if (column > MaxEditCol)
      MaxEditCol = column;
  }
  UpdateTable();
}

void MatrixEditor::on_SaveButton_clicked() {
  if (ui->NameLineEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Name is undefined"),
                          QMessageBox::Ok);
    return;
  }
  if ((!ui->EditTable->item(0, 0) ||
       ui->EditTable->item(0, 0)->text().isEmpty()) && // may cause memory leak?
      MaxEditRow == 0 &&
      MaxEditCol == 0) {
    QMessageBox::critical(this, tr("Error"), tr("Matrix is empty"),
                          QMessageBox::Ok);
    return;
  }
  // Check if completed and is a square matrix (N by M)
  for (int i = 0; i <= MaxEditRow; i++) {
    for (int j = 0; j <= MaxEditCol; j++) {
      if (!ui->EditTable->item(i, j)) { // not even modified
        QMessageBox::critical(
            this, tr("Error"),
            QString::fromStdString("(" + std::to_string(j + 1) + "," +
                                   std::to_string(i + 1) +
                                   tr(") is empty").toStdString()),
            QMessageBox::Ok);
        return;
      }
      if (ui->EditTable->item(i, j)
              ->text()
              .isEmpty()) { // has been modified but it is empty now
        QMessageBox::critical(
            this, "Error",
            QString::fromStdString("(" + std::to_string(j + 1) + "," +
                                   std::to_string(i + 1) +
                                   tr(") is empty").toStdString()),
            QMessageBox::Ok);
        return;
      }
    }
  }
  std::string MatrixName = ui->NameLineEdit->text().toStdString();
  Eigen::MatrixXd NewMatrix(ReadfromTable()); // call read function here
  if (ExprtkCompErrorFlag) {                  // compilation error
    ExprtkCompErrorFlag = 0;
    return;
  }
  if (MatrixMap.find(MatrixName) != MatrixMap.end()) {
    QMessageBox::StandardButton ToReplace;
    ToReplace = QMessageBox::question(
        this, "Warning",
        tr("Matrix \"") + QString::fromStdString(MatrixName) +
            tr("\" already exists.\nAre you sure to replace?"),
        QMessageBox::No | QMessageBox::Yes);
    if (ToReplace == QMessageBox::No) {
      return;
    }
  }
  MatrixMap.insert_or_assign(MatrixName, NewMatrix);
  this->close();
  emit EditDone();
}

void MatrixEditor::on_CancelButton_clicked() {
  this->close();
  emit EditDone();
}
