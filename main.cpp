#include "club.h"

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "No arguments!" << std::endl;
    }
    if (argc > 2) {
        std::cout << "Too many arguments!" << std::endl;
    }

    size_t n_tables, cost;
    std::string line;
    std::string open_time, close_time;
    std::string file_name{argv[1]};
    if (!CheckCorrectness(file_name)) {
        return 0;
    }
    std::ifstream file;
    file.open(file_name);
    if (!file.is_open()) {
        std::cout << "Unavailable file\n";
        return 0;
    }
    if (std::getline(file, line)) {
        n_tables = std::stoul(line);
    }
    if (std::getline(file, line)) {
        open_time = line.substr(0, 5);
        close_time = line.substr(6, 5);
    }
    if (std::getline(file, line)) {
        cost = std::stoul(line);
    }

    Club club{n_tables, open_time, close_time, cost, std::cout};
    std::cout << open_time << "\n";
    while (std::getline(file, line)) {
        EventInfo info;
        int result = 0;
        result = ParseLine(&info, line);
        if (info.table_number.has_value()) {
            result = club.Event(info.time, info.id, info.client_name, info.table_number.value());
        } else {
            result = club.Event(info.time, info.id, info.client_name);
        }
        if (result == -1) {
            std::cout << line << "\n";
            return 0;
        }
    }
    club.Event(close_time, 11, "");
    std::cout << close_time << "\n";
    club.ShowRevenues();
    file.close();
    return 0;
}