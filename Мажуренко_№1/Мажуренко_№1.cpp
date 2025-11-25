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


class FuelData {
private:
    std::string fuel_type_;
    std::string date_;
    float price_;

public:
    FuelData() = default;

    explicit FuelData(const std::string& fuel_type, const std::string& date,
        float price)
        : fuel_type_(fuel_type), date_(date), price_(price) {
    }

    FuelData(const FuelData& other) = default;
    FuelData& operator=(const FuelData& other) = default;

    FuelData(FuelData&& other) noexcept
        : fuel_type_(std::move(other.fuel_type_)),
        date_(std::move(other.date_)),
        price_(other.price_) {
    }

    FuelData& operator=(FuelData&& other) noexcept {
        if (this != &other) {
            fuel_type_ = std::move(other.fuel_type_);
            date_ = std::move(other.date_);
            price_ = other.price_;
        }
        return *this;
    }
    const std::string& GetFuelType() const { return fuel_type_; }
    float GetPrice() const { return price_; }
    const std::string& GetDate() const { return date_; }

    friend std::ostream& operator<<(std::ostream& os, const FuelData& obj);
};

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

bool CompareByPrice(const FuelData& a, const FuelData& b) {
    return a.GetPrice() < b.GetPrice();
}

std::vector<FuelData> FilterByFuelType(const std::vector<FuelData>& data,
    const std::string& fuel_type) {
    std::vector<FuelData> result;
    for (const auto& item : data) {
        if (item.GetFuelType() == fuel_type) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<FuelData> ReadFuelDataFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file " << filename << std::endl;
        return {};
    }

    std::vector<FuelData> data;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        FuelData new_obj = ParseFuelData(line);
        data.emplace_back(std::move(new_obj));
    }
    file.close();
    return data;
}

void PrintFilteredData(const std::vector<FuelData>& filtered_data,
    const std::string& target_fuel) {
    if (filtered_data.empty()) {
        std::cout << "No records found for fuel type " << target_fuel << std::endl;
    }
    else {
        for (const auto& obj : filtered_data) {
            std::cout << obj << std::endl;
        }
    }
}

int main() {
    std::vector<FuelData> data = ReadFuelDataFromFile("input.txt");
    if (data.empty()) {
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