#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Eigen/Dense>
#include <QDialog>
#include <QEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QTextFormat>
#include <QTranslator>
#define ADD 1
#define SUB 2
#define MLP 3

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void SetLanguageChinese(void);
    void SetLanguageEnglish(void);
    void ShowAboutMsg(void);
    void ShowHelpMsg(void);
    void on_NewButton_clicked();
    void UpdateTable(void);
    void on_MatrixTable_cellDoubleClicked(int row, int column);
    void on_DeleteMButton_clicked();
    void on_MAddMButton_clicked();
    void on_MSubMButton_clicked();
    void on_MTimeMButton_clicked();
    void on_MScaleButton_clicked();
    void on_InverseButton_clicked();
    void on_DeterminantButton_clicked();

private:
    Ui::MainWindow* ui;
    QString expression;
    QTranslator m_Translator;
    QStringList GetAllNames(void);
    void OpofTwoMatrix(int Opcode, std::string OpString);
    void changeEvent(QEvent* event);
};
#endif // MAINWINDOW_H
