#ifndef MYEXPRTK_H
#define MYEXPRTK_H
#include "exprtk.hpp"
#include <QInputDialog>
#include <QMessageBox>
#include <QTranslator>
class Myexprtk {
public:
  static double myexprtk(std::string NumString, std::string ErrorMsg);
};

#endif // MYEXPRTK_H
