#include <stdexcept>

class TimeoutException : public std::runtime_error {
 public:
  TimeoutException(const std::string& msg) : std::runtime_error(msg) {}
  ~TimeoutException() {}
};