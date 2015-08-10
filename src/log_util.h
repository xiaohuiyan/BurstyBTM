#ifndef _Log_UTIL_H
#define _Log_UTIL_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cassert>

using namespace std;

/**
 * A simple class for logging
 */
namespace log_util {

  inline void logInfo(string msg) {
	cout << "[Info] " << msg << endl;
  }

  inline void logError(string msg) {
    cout << "[Error] " << msg << endl;
    exit(1);
  }
}

#endif
