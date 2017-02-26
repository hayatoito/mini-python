#ifndef MINIPY_BASE_H__
#define MINIPY_BASE_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                                    \
  DISALLOW_COPY_AND_ASSIGN(TypeName)

#define FOREACH(it,c) for(auto it=(c).begin();it!=(c).end();++it)

#define LOG(level) logging::GetLogger##level(__FILE__,  __LINE__)

namespace logging {

enum LogLevel {
  ERROR = 0,
  INFO = 1,
  VERBOSE = 2,
};

// Declaration
extern int kLogLevel;

template <LogLevel level>
class Logger {
 public:
  Logger(const char* file, int line) {
    if (level <= kLogLevel) {
      std::cout << "[" << file << ":" << line << "] ";
    }
  }
  ~Logger() {
    if (level <= kLogLevel) {
      std::cout << std::endl;
    }
  }

  template <class T>
  Logger& operator<<(const T& o) {
    if (level <= kLogLevel) {
      std::cout << o;
    }
    return *this;
  }
  DISALLOW_COPY_AND_ASSIGN(Logger);
};

inline Logger<ERROR> GetLoggerERROR(const char* file, int line) {
  return Logger<ERROR>(file, line);
}

inline Logger<INFO> GetLoggerINFO(const char* file, int line) {
  return Logger<INFO>(file, line);
}

inline Logger<VERBOSE> GetLoggerVEROBSE(const char* file, int line) {
  return Logger<VERBOSE>(file, line);
}

}  // namespace logging


template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
  const int n = v.size();
  os << "[";
  for (int i = 0; i < n; ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << v[i];
  }
  os << "]";
  return os;
}

template <class K, class V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
  os << "{";
  for (typename std::map<K, V>::const_iterator it = m.begin();
       it != m.end(); ++it) {
    if (it != m.begin()) {
      os << ", ";
    }
    os << it->first << ":" << it->second;
  }
  os << "}";
  return os;
}



#endif  // MINIPY_BASE_H__
