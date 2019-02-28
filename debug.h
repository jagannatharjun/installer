#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <QString>

//#define SHOW(X)                                                                \
//  qDebug() << __func__ << ":" << __LINE__ << ": " << #X << ": " << (X);

#define SHOW(X)                                                                \
  qDebug() << QString("%4:%1:%2: %3 = \0")                                     \
                  .arg(__func__)                                               \
                  .arg(__LINE__)                                               \
                  .arg(#X)                                                     \
                  .arg(__FILE__)                                               \
                  .toStdString()                                               \
                  .c_str()                                                     \
           << X;

static QDebug &operator<<(QDebug &d, const std::string &s) {
  return d << s.c_str();
}

#endif // DEBUG_H
