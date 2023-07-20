#ifndef MATRIXEDITOR_H
#define MATRIXEDITOR_H

#include <Eigen/Dense>
#include <QInputDialog>
#include <QMessageBox>
#include <QWidget>
namespace Ui {
class MatrixEditor;
}

class MatrixEditor : public QWidget {
  Q_OBJECT

public:
  explicit MatrixEditor(QWidget *parent = nullptr);
  MatrixEditor(QWidget *parent, Eigen::MatrixXd *matrix,
               std::string MatrixName);
  ~MatrixEditor();
  void UpdateTable(void);

private slots:
  void on_EditTable_cellChanged(int row, int column);
  void on_SaveButton_clicked();

  void on_CancelButton_clicked();

private:
  Ui::MatrixEditor *ui;
  Eigen::MatrixXd ReadfromTable(void);
  void changeEvent(QEvent *event);
signals:
  void EditDone(void);
  void ExprtkCompError(void);
};

#endif // MATRIXEDITOR_H
