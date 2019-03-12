#ifndef DEBUG_H
#define DEBUG_H

#include <QString>
#include <gupta/format_io.hpp>

static std::string to_string(const QString &s) { return s.toStdString(); }

template <typename PtrT> std::string to_string(const PtrT *s) {
  return typeid(PtrT).name() + std::string("(") +
         std::to_string(reinterpret_cast<std::uintptr_t>(s)) + ")";
}

template <typename Obj,
          typename = typename std::is_convertible<Obj, bool>::type>
std::string to_string(const Obj &o) {
  return (bool)o ? "true" : "false";
}

static inline std::string to_string(bool b) {
  return b ? "true" : "false";
}

#endif // DEBUG_H
