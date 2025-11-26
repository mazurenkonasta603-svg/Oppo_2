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

std::ostream& operator<<(std::ostream& os, const FuelData& obj) {
    os << "Fuel type: " << obj.fuel_type_ << " Date: " << obj.date_
        << " Price: " << std::fixed << std::setprecision(2) << obj.price_;
    return os;
}


FuelData ParseFuelData(const std::string& input_string) {
    std::regex date_pattern(R"((\d{4}[-./]\d{2}[-./]\d{2}|\d{2}[-./]\d{2}[-./]\d{4}))");
    std::regex price_decimal_pattern(R"(\d+[.,]\d+)");
    std::regex price_integer_pattern(R"(\d+)");
    std::regex fuel_type_pattern(R"(["]([^"]*)["])");

    std::string tmp = input_string;
    std::smatch match;

    // Fuel type: ищем в кавычках
    std::string fuel;
    if (std::regex_search(tmp, match, fuel_type_pattern)) {
        fuel = match[1].str(); // внутренняя группа без кавычек
        tmp.erase(match.position(), match.length());
    }

    // Date: ищем дату
    std::string date;
    if (std::regex_search(tmp, match, date_pattern)) {
        date = match[0].str();
        tmp.erase(match.position(), match.length());
    }

    // Price: в оставшейся строке сначала ищем число с десятичной частью,
    // если нет — любое целое число
    std::string price_str;
    if (std::regex_search(tmp, match, price_decimal_pattern)) {
        price_str = match[0].str();
    }
    else if (std::regex_search(tmp, match, price_integer_pattern)) {
        price_str = match[0].str();
    }

    // Нормализация запятой в точку и конвертация в float
    std::replace(price_str.begin(), price_str.end(), ',', '.');
    float price = 0.0f;
    if (!price_str.empty()) {
        try {
            price = std::stof(price_str);
        }
        catch (const std::exception&) {
            price = 0.0f;
        }
    }

    return FuelData(fuel, date, price);
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
    std::sort(data.begin(), data.end(), CompareByPrice);

    std::string target_fuel;
    std::cout << "Enter fuel type to filter (e.g., DT, AI-95): ";
    std::getline(std::cin, target_fuel);

    auto filtered_data = FilterByFuelType(data, target_fuel);
    PrintFilteredData(filtered_data, target_fuel);

    return 0;
}