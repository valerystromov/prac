#pragma once

#include <chrono>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

enum class EMonetizerType : uint8_t {
    Share = 0,
    Deposit = 1,
    Currency = 2,
    Obligation = 3,
    Metal = 4
};

static const std::unordered_map<std::string, EMonetizerType> MonetizerTypeString = {
    {"Акции", EMonetizerType::Share},
    {"Депозит", EMonetizerType::Deposit},
    {"Валюта", EMonetizerType::Currency},
    {"Облигации", EMonetizerType::Obligation},
    {"Драгоценные металлы", EMonetizerType::Metal}
};

class TMonetizer {
public:
    explicit TMonetizer(uint32_t id, std::mt19937& generator, const std::string& name,
                        const std::string& type, const std::string& profit, const std::string& risk)
        : Id(id)
        , Generator(generator)
        , Name(name)
        , Type(MonetizerTypeString.at(type))
        , Profit(ComputeValue(profit, true))
        , Risk(ComputeValue(risk, true))
    {
    }

    uint32_t GetId() const noexcept {
        return Id;
    }

    const std::string& GetName() const noexcept {
        return Name;
    }

    EMonetizerType GetType() const noexcept {
        return Type;
    }

    std::string GetTypeString() const noexcept {
        for (const auto& kv : MonetizerTypeString) {
            if (kv.second == GetType()) {
                return kv.first;
            }
        }
        return "";
    }

    double GetProfit() const noexcept {
        return Profit;
    }

    double GetRisk() const noexcept {
        return Risk;
    }

    double GetMoney() const noexcept {
        return Money;
    }

    void UpdateMoney(double value) {
        Money = value;
    }

    double GetDiff() const noexcept {
        return Money * Profit * (1 - Risk);
    }

    void SetMoney(double value) noexcept {
        Money = value;
    }

    void SetProfit(const std::string& value) {
        Profit = ComputeValue(value, false);
    }

    void SetRisk(const std::string& value) {
        Risk = ComputeValue(value, false);
    }

protected:
    double ComputeValue(const std::string& def, bool divide) {
        if (def == "auto") {
            std::uniform_int_distribution<uint32_t> distribution(0, 100);
            return distribution(Generator) / 100.;
        }
        return std::stod(def) / (divide ? 100. : 1.);
    }

private:
    uint32_t Id;
    std::mt19937& Generator;
    std::string Name;
    EMonetizerType Type;
    double Profit = 0;
    double Risk = 0;
    double Money = 0.0;
};

class TGameState {
public:
    TGameState()
        : Generator(std::chrono::system_clock::now().time_since_epoch().count())
    {
    }

    template <typename... Args>
    void AddMonetizer(Args&&... args) noexcept {
        Monetizers.emplace_back(Monetizers.size(), Generator, std::forward<Args>(args)...);
    }

    std::vector<TMonetizer>& GetMonetizers() noexcept {
        return Monetizers;
    }

    double GetMoneyCount() const noexcept {
        return Money;
    }

    double GetFullMoney() const noexcept {
        return std::accumulate(Monetizers.begin(), Monetizers.end(), Money, [](double s, const auto& item) { return s + item.GetMoney(); });
    }

    void InitMoney(double value) noexcept {
        Money = value;
    }

    size_t GetMonth() const noexcept {
        return Month;
    }

    void IncMonth() noexcept {
        ++Month;
    }

    void DeltaMoney(double value) {
        Money += value;
    }

    void SetIncome(double value) {
        Income = value;
    }

    double GetIncome() const {
        return Income;
    }

private:
    std::mt19937 Generator;
    double Money = 0.;
    size_t Month = 0;
    double Income = 0.;
    std::vector<TMonetizer> Monetizers;
};

using TGameStatePtr = std::shared_ptr<TGameState>;
