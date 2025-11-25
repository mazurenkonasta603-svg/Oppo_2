#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <iomanip> 

class FuelData;

FuelData ParseFuelData(const std::string& input_string);

static std::string ExtractValue(const std::string& input_string,
    const std::regex& pattern) {
    std::smatch match;
    if (std::regex_search(input_string, match, pattern)) {
        if (match.size() > 1 && match[1].length() > 0) {
            return match[1].str();
        }
        return match[0].str();
    }
    return "";
}


class fuel_Data {
private:
    std::string fuel_type_;
    std::string date_;
    float price_;

public:
    fuel_Data() = default;

    void parse_string(const std::string& _string) {
        std::regex re_date(R"((\d{4}[-./]\d{2}[-./]\d{2}|\d{2}[-./]\d{2}[-./]\d{4}))");
        std::regex re_price(R"(\d+([.,]\d+)?)");
        std::regex re_word(R"(["](.*?)["])");

        std::vector<std::string> tokens;
        std::stringstream ss(_string);
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        std::smatch match;
        for (const auto& part : tokens) {
            if (std::regex_match(part, re_date)) {
                date_ = part;
            }
            else if (std::regex_match(part, re_price)) {
                std::string p = part;
                std::replace(p.begin(), p.end(), ',', '.');
                price_ = std::stof(p);
            }
            else if (std::regex_match(part, match, re_word)) {
                fuel_type_ = match[1].str();
            }
        }
    }

    const std::string& get_fuel_type() const { return fuel_type_; };
    float get_price() const { return price_; };

    friend std::ostream& operator<<(std::ostream& os, const fuel_Data& obj);
};

std::ostream& operator<<(std::ostream& os, const fuel_Data& obj) {
    os << "Fuel type: " << obj.fuel_type_ << " Date: " << obj.date_ << " Price: " << std::fixed << std::setprecision(2) << obj.price_;
    return os;
}

bool by_price(const fuel_Data& a, const fuel_Data& b) {
    return a.get_price() < b.get_price();
}

std::vector<fuel_Data> f_fuel_type(const std::vector<fuel_Data>& data, const std::string& fuel_type) {
    std::vector<fuel_Data> result;
    for (const auto& item : data) {
        if (item.get_fuel_type() == fuel_type) {
            result.push_back(item);
        }
    }
    return result;
}

int main() {
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error opening file input.txt" << std::endl;
        return 1;
    }

    std::vector<fuel_Data> data;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        fuel_Data new_obj;
        new_obj.parse_string(line);
        data.push_back(new_obj);
    }
    file.close();

    std::sort(data.begin(), data.end(), by_price);

    std::string target_fuel;

    std::cout << "Enter fuel type to filter (e.g., DT, AI-95): ";

    std::getline(std::cin, target_fuel);

    auto filtered_data = f_fuel_type(data, target_fuel);

    if (filtered_data.empty()) {
        std::cout << "No records found for fuel type: " << target_fuel << std::endl;
    }
    else {
        for (const auto& obj : filtered_data) {
            std::cout << obj << std::endl;
        }
    }

    return 0;
}