#pragma once

#include <iostream>
#include <deque>
#include <vector>
#include <map>
#include <string>
#include <optional>
#include <fstream>
#include <string_view>
#include <cmath>

struct Table {
    bool is_busy;
    size_t full_time;
    size_t revenue;
};

struct Info {
    size_t start_time;
    int32_t table_number;
};

struct EventInfo {
    std::string time;
    size_t id;
    std::string client_name;
    std::optional<size_t> table_number;
};

class Club {
public:
    Club(size_t n_tables, std::string_view open_time, std::string_view close_time, size_t cost,
         std::ostream& stream);

    int Event(std::string time, size_t id, std::string name);
    int Event(std::string time, size_t id, std::string name, size_t table_number);

    void ShowRevenues();

    

private:
    // Clients -> key: name, value: (entrance_time, table_number)
    using Clients = std::map<std::string, Info>;
    Clients clients_;

    // waiters queue
    std::deque<std::string> queue_;

    // tables
    std::vector<Table> tables_;
    // number of not empty tables
    size_t busy_tables_ = 0;

    // times
    size_t open_time_, close_time_;
    size_t last_event_time_;

    // cost
    size_t cost_;

    // stream to out
    std::ostream& stream_;
};

size_t TimeToSizet(std::string_view time);
bool CheckTime(std::string_view time);
bool CheckName(std::string_view name);
int ParseLine(EventInfo* info, std::string line);
bool CheckCorrectness(std::string path);