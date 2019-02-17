#ifndef FEL_H
#define FEL_H

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace fel
{
  struct LogEntry
  {
    std::string file;
    std::string message;
    int line = 0;
    int column = 0;

    bool operator==(const LogEntry& rhs) const;
  };
  std::ostream& operator<<(std::ostream& o, const LogEntry& entry);

  struct Log
  {
    std::vector<LogEntry> entries;
  };

  struct Entry
  {
    std::string str;
  };
  
  struct State
  {
    std::vector<Entry> stack;
    const Entry& from_index(int index) const;

    bool is_string(int index) const;
    const std::string& as_string(int index) const;
  };

  struct Fel
  {
    using Callback = std::function<void(int arguments, State* state)>;

    std::map<std::string, Callback> functions;

    void SetFunction(const std::string& name, Callback callback);
    void LoadAndRunString(const std::string& str, const std::string& filename, Log* log);
    void LoadAndRunFile(const std::string& file, Log* log);
  };
}

#endif // FEL_H
