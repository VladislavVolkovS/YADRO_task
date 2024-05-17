#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "club.h"

void Run(std::string path);

std::string GetLine(std::ifstream& file);

void Check(std::string answer_path);

TEST_CASE("Example", "") {
    std::string path{"../tests/example_test.txt"};
    REQUIRE(CheckCorrectness(path));
    Run(path);
    Check("../answers/example_test.txt");
}

// 21:00 - 07:00
TEST_CASE("FromEveningToMorning", "") {
    std::string path{"../tests/time_test.txt"};
    REQUIRE(CheckCorrectness(path));
    Run(path);
    Check("../answers/time_test.txt");
}

TEST_CASE("ClientUnknown", "") {
    std::string path{"../tests/ClientUnknown_test.txt"};
    REQUIRE(CheckCorrectness(path));
    Run(path);
    Check("../answers/ClientUnknown_test.txt");
}

TEST_CASE("YouShallNotPass", "") {
    std::string path{"../tests/YouShallNotPass_test.txt"};
    REQUIRE(CheckCorrectness(path));
    Run(path);
    Check("../answers/YouShallNotPass_test.txt");
}

TEST_CASE("QueueIsFull", "") {
    std::string path{"../tests/QueueIsFull_test.txt"};
    REQUIRE(CheckCorrectness(path));
    Run(path);
    Check("../answers/QueueIsFull_test.txt");
}

// output in console wrong line
TEST_CASE("WrongInput", "") {
    std::string path{"../tests/wrong_time_test.txt"};
    REQUIRE_FALSE(CheckCorrectness(path));                          // 21:46 3 client1
    path = "../tests/2_numbers_in_the_first_line_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 3 4
    path = "../tests/2_spaces_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 20:48 1  client1
    path = "../tests/absence_of_an_insignificant_zero_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 21:00 23:0
    path = "../tests/absence_of_the_name_of_client_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 20:48 1
    path = "../tests/invalid_character_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 20:48 1 clien)t1
    path = "../tests/negative_cost_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // -10
    path = "../tests/negative_number_of_tables_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // -3
    path = "../tests/no_amount_of_tables_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 21:00 23:00
    path = "../tests/no_end_time_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 21:00
    path = "../tests/no_event_number_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 21:46 client1
    path = "../tests/no_event_start_time_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 1 client1
    path = "../tests/no_line_break_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 20:48 1 client121:41 1 client1
    path = "../tests/no_price_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 20:48 1 client1
    path = "../tests/event_number_is_greater_than_n_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 21:46 8 client1
    path = "../tests/table_number_is_greater_than_n_test.txt";
    REQUIRE_FALSE(CheckCorrectness(path));                          // 09:54 2 client1 8
}


void Check(std::string answer_path) {
    std::string result_path{"result.txt"};
    std::ifstream answer, result;
    answer.open(answer_path);
    result.open(result_path);
    if (!answer.is_open() && !result.is_open()) {
        std::cout << "Unavailable files\n";
        return;
    }
    std::string line;
    while (std::getline(result, line)) {
        auto ans = GetLine(answer);
        CHECK(line == ans);
    }
}

void Run(std::string path) {
    size_t n_tables, cost;
    std::string line;
    std::string open_time, close_time;
    if (!CheckCorrectness(path)) {
        return;
    }
    std::ifstream file;
    std::ofstream out;
    file.open(path);
    out.open("result.txt");
    if (!file.is_open() && !out.is_open()) {
        std::cout << "Unavailable files\n";
        return;
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

    Club club{n_tables, open_time, close_time, cost, out};
    out << open_time << "\n";
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
            out << line << "\n";
            return;
        }
    }
    club.Event(close_time, 11, "");
    out << close_time << "\n";
    club.ShowRevenues();
    file.close();
}

std::string GetLine(std::ifstream& file) {
    std::string result;
    if (std::getline(file, result)) {
        return result;
    }
    return "";
}