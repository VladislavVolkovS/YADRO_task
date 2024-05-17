#include "club.h"

size_t CharToDigit(char c) {
    size_t result = static_cast<size_t>(c - '0');
    return result;
}

size_t TimeToSizet(std::string_view time) {
    size_t hours = CharToDigit(time[0]) * 10 + CharToDigit(time[1]);
    size_t minutes = CharToDigit(time[3]) * 10 + CharToDigit(time[4]);
    return hours * 60 + minutes;
}

bool NumericTimeCheck(size_t time, size_t open_time, size_t close_time) {
    if (open_time < close_time) {
        return time >= open_time && time <= close_time;
    } else {
        return time >= open_time || time <= close_time;
    }
}

void CalculateTable(Table* table, size_t start_time, size_t end_time, size_t cost) {
    if (end_time < start_time) {
        end_time += 24 * 60;
    }
    auto diff_times = end_time - start_time;
    auto hours = diff_times / 60;
    if (diff_times % 60 != 0) {
        ++hours;
    }
    table->revenue += hours * cost;
    table->full_time += diff_times;
    table->is_busy = false;
}

Club::Club(size_t n_tables, std::string_view open_time, std::string_view close_time, size_t cost,
           std::ostream& stream)
    : tables_(n_tables), cost_(cost), stream_(stream) {
    open_time_ = TimeToSizet(open_time);
    close_time_ = TimeToSizet(close_time);
}

int Club::Event(std::string str_time, size_t id, std::string name) {
    auto time = TimeToSizet(str_time);
    last_event_time_ = time;
    int result = 0;
    switch (id) {
    case 1:
        stream_ << str_time << " " << id << " " << name << "\n";
        if (auto it = clients_.find(name); it != clients_.end()) {
            result = Club::Event(str_time, 13, "YouShallNotPass");
            break;
        }
        if (!NumericTimeCheck(time, open_time_, close_time_)) {
            result = Club::Event(str_time, 13, "NotOpenYet");
            break;
        }
        clients_.emplace(name, Info{0, -1});
        queue_.push_back(name);
        break;
    case 3:
        stream_ << str_time << " " << id << " " << name << "\n";
        if (busy_tables_ < tables_.size()) {
            result = Club::Event(str_time, 13, "ICanWaitNoLonger!");
            break;
        }
        if (queue_.size() > tables_.size()) {
            result = Club::Event(str_time, 11, name);
        }
        break;
    case 4:
        {
            stream_ << str_time << " " << id << " " << name << "\n";
            if (auto it = clients_.find(name); it == clients_.end()) {
                result = Club::Event(str_time, 13, "ClientUnknown");
                break;
            }
            auto table_number = clients_[name].table_number;
            CalculateTable(&tables_[table_number - 1], clients_[name].start_time, time, cost_);
            --busy_tables_;
            clients_.erase(name);
            if (queue_.size() == 0) {
                break;
            }
            result = Club::Event(str_time, 12, queue_.front(), table_number);
            break;
        }
    case 11:
        if (time == close_time_) {
            for (auto& [client_name, info] : clients_) {
                stream_ << str_time << " " << id << " " << client_name << "\n"; 
                if (info.table_number != -1) {
                    CalculateTable(&tables_[info.table_number - 1], info.start_time, time, cost_);
                    --busy_tables_;
                }
            }
            break;
        } else {
            stream_ << str_time << " " << id << " " << name << "\n";
            clients_.erase(name);
            auto it = queue_.begin();
            for (; it != queue_.end(); ++it) {
                if ((*it) == name) {
                    break;
                }
            }
            queue_.erase(it);
        }
        break;
    case 13:
        stream_ << str_time << " " << id << " " << name << "\n";
        break;
    default:
        result = -1;
        break;
    }
    return result;
}

int Club::Event(std::string str_time, size_t id, std::string name, size_t table_number) {
    if (table_number > tables_.size()) {
        return -1;
    }
    auto time = TimeToSizet(str_time);
    int result = 0;
    last_event_time_ = time;
    switch (id) {
    case 2:
        stream_ << str_time << " " << id << " " << name << " " << table_number << "\n";
        if (tables_[table_number - 1].is_busy) {
            result = Club::Event(str_time, 13, "PlaceIsBusy");
            break;
        }
        if (auto it = clients_.find(name); it == clients_.end()) {
            result = Club::Event(str_time, 13, "ClientUnknown");
            break;
        }
        // CalculateTable(&tables_[table_number - 1], clients_[name].start_time, time, cost_);
        clients_[name].start_time = time;
        clients_[name].table_number = table_number;
        tables_[table_number - 1].is_busy = true;
        ++busy_tables_;
        queue_.pop_front();
        break;
    case 12:
        stream_ << str_time << " " << id << " " << name << " " << table_number << "\n";
        clients_[queue_.front()].start_time = time;
        clients_[queue_.front()].table_number = table_number;
        tables_[table_number - 1].is_busy = true;
        ++busy_tables_;
        queue_.pop_front();
        break;  
    default:
        result = -1;
        break;
    }
    return result;
}

std::string ConvertTime(size_t time) {
    auto hours = std::to_string(time / 60);
    if (hours.size() == 1) {
        hours = "0" + hours;
    }
    auto minutes = std::to_string(time % 60);
    if (minutes.size() == 1) {
        minutes = "0" + minutes;
    }
    return hours + ":" + minutes;
}

void Club::ShowRevenues() {
    for (size_t i = 0; i < tables_.size(); ++i) {
        stream_ << i + 1 << " " << tables_[i].revenue << " " << ConvertTime(tables_[i].full_time);
        stream_ << "\n";
    }
}

bool CheckTime(std::string_view time) {
    if (time.size() != 5) {
        return false;
    }
    if (!(time[0] >= '0' && time[0] <= '2')) {
        return false;
    }
    if (time[0] == '2' && !(time[1] >= '0' && time[1] <= '3')) {
        return false;
    }
    if (!(time[1] >= '0' && time[1] <= '9')) {
        return false;
    }
    if (time[2] != ':') {
        return false;
    }
    if (!(time[3] >= '0' && time[3] <= '5' && time[4] >= '0' && time[4] <= '9')) {
        return false;
    }
    return true;
}

bool CheckName(std::string_view name) {
    for (const auto& symbol : name) {
        if (!((symbol >= 'a' && symbol <= 'z') || (symbol >= '0' && symbol <= '9') || symbol == '-'
            || symbol == '_')) {
            return false;
        }
    }
    return true;
}

bool CheckNumber(std::string_view string) {
    for (const auto& symbol : string) {
        if (!(symbol >= '0' && symbol <= '9')) {
            return false;
        }
    }
    return true;
}

bool CheckSpaces(std::string_view line) {
    char prev_symb;
    for (const auto& symbol : line) {
        if (prev_symb == ' ' && symbol == ' ') {
            return false;
        }
        prev_symb = symbol;
    }
    return true;
}

int ParseLine(EventInfo* info, std::string line) {
    int result = 0;
    size_t field = 0;
    size_t left = 0;
    size_t right = 0;
    while (right <= line.size()) {
        if (line[right] == ' ' || right == line.size()) {
            switch (field) {
            case 0:
                info->time = line.substr(left, right - left);
                if (!CheckTime(info->time)) {
                    result = -1;
                    break;
                }
                ++field;
                break;
            case 1:
                {
                    std::string id = line.substr(left, right - left);
                    if (!CheckNumber(id)) {
                        result = -1;
                        break;
                    }
                    info->id = std::stoul(id);
                    if (!(info->id >= 1 && info->id <= 4)) {
                        result = -1;
                    }
                    ++field;
                    break;
                }
            case 2:
                info->client_name = line.substr(left, right - left);
                if (!CheckName(info->client_name)) {
                    result = -1;
                    break;
                }
                ++field;
                break;
            case 3:
                info->table_number = std::stoul(line.substr(left, right - left));
                ++field;
                break;
            default:
                result = -1;
                break;
            }
            if (result == -1) {
                return result;
            }
            ++right;
            left = right;
        } else {
            ++right;
        }
    }
    if (field <= 2) {
        result = -1;
    }
    return result;
}

bool CheckCorrectness(std::string path) {
    size_t n_tables, cost;
    std::string line;
    size_t open_time, close_time;
    std::ifstream file;
    file.open(path);
    if (!file.is_open()) {
        std::cout << "Unavailable file\n";
        return false;
    }
    if (std::getline(file, line)) {
        if (!CheckNumber(line) || !CheckSpaces(line)) {
            std::cout << line << "\n";
            return false;
        }
        n_tables = std::stoul(line);
    }
    if (std::getline(file, line)) {
        if (line.size() != 11 || !CheckSpaces(line)) {
            std::cout << line << "\n";
            return false;
        }
        auto open_time_str = line.substr(0, 5);
        auto close_time_str = line.substr(6, 5);
        if (!CheckTime(open_time_str) || !(CheckTime(close_time_str))) {
            std::cout << line << "\n";
            return false;
        }
        open_time = TimeToSizet(line.substr(0, 5));
        close_time = TimeToSizet(line.substr(6, 5));
    }
    if (std::getline(file, line)) {
        if (!CheckNumber(line) || !CheckSpaces(line)) {
            std::cout << line << "\n";
            return false;
        }
        cost = std::stoul(line);
    }
    size_t current_time = 0;
    while (std::getline(file, line)) {
        if (!CheckSpaces(line)) {
            std::cout << line << "\n";
            return false;
        }
        EventInfo info;
        int result = 0;
        if (ParseLine(&info, line) != 0) {
            std::cout << line << "\n";
            return false;
        }
        size_t time = TimeToSizet(info.time);
        if (close_time > open_time) {
            if (!(time >= current_time)) {
                std::cout << line << "\n";
                return false;
            }
        } else {
            if (!(time >= current_time || time < close_time)) {
                std::cout << line << "\n";
                return false;
            }
        }
        if (info.table_number.has_value()) {
            if (info.table_number.value() > n_tables) {
                std::cout << line << "\n";
                return false;
            }
        }
        if (result == -1) {
            std::cout << line << "\n";
            return 0;
        }
        current_time = time;
    }
    file.close();
    return true;
}