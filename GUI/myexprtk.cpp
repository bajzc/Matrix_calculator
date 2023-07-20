#include "myexprtk.h"
double Myexprtk::myexprtk(std::string NumString, std::string ErrorMsg) {
  std::vector<std::string> VariableList;
  exprtk::symbol_table<double> SymbolTable;
  // Do not support variable
  if (!exprtk::collect_variables(NumString,
                                 VariableList)) { // failed
    QMessageBox::critical(NULL, QObject::tr("Error"),
                          QString::fromStdString(ErrorMsg), QMessageBox::Ok);
    return NAN;
  } else { // ask for variable
    for (const auto &var : VariableList) {
      if (var == "pi" || var == "epsilon" || var == "inf")
        continue;
      bool ok;
      do {
        double d = QInputDialog::getDouble(
            NULL, QObject::tr("Input variable"),
            QObject::tr("Variable: ") + QString::fromStdString(var), 0,
            std::numeric_limits<double>::min(),
            std::numeric_limits<double>::max(), 10, &ok, Qt::WindowFlags(), 1);
        if (ok) {
          SymbolTable.add_variable(var, d);
          break;
        } else {
          QMessageBox::warning(
              NULL, QObject::tr("Variable"),
              QObject::tr("Please enter the value for variable"),
              QMessageBox::Ok);
        }
      } while (1);
    }
  }
  exprtk::expression<double> expression;
  exprtk::parser<double> parser;
  SymbolTable.add_constants();
  expression.register_symbol_table(SymbolTable);
  if (!parser.compile(NumString, expression)) {
    QMessageBox::critical(NULL, QObject::tr("Error"),
                          QString::fromStdString(ErrorMsg), QMessageBox::Ok);
  }
  return expression.value();
}
