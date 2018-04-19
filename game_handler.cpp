#include "game_handler.h"

#include <sstream>
#include <iomanip>
#include <iostream>

static std::string DrawCurrentState(TGameStatePtr state) {
    std::stringstream res;
    res << std::fixed << std::setprecision(3);
    res
        << "<table class=\"table table-hover\">\n"
            << "<thead><tr>\n"
                << "<th>Суммарный капитал</th>\n"
                << "<th>Свободные средства</th>\n"
                << "<th>Доход за месяц</th>\n"
                << "<th>Налог за месяц</th>\n"
            << "</tr></thead>\n"
            << "<tbody>\n"
                << "<tr><td>" << state->GetFullMoney() << "</td><td>" << state->GetMoneyCount()
                << "</td><td>" << state->GetIncome() << "</td><td>" << state->GetIncome() * 0.17 << "</td></tr>"
            << "</tbody>\n"
        << "</table>\n"
        << "<br></br>\n";

    res << "<h2>Информация по монетизаторам</h2>"
        << "<form>"
        << "<table class=\"table\">\n"
            << "<thead><tr>\n"
                << "<th>№</th>\n"
                << "<th>Название</th>\n"
                << "<th>Тип</th>\n"
                << "<th>Изменения за месяц</th>\n"
                << "<th>Средства</th>\n"
                << "<th>Доходность</th>\n"
                << "<th>Риск</th>\n"
            << "</tr></thead>\n"
            << "<tbody>\n";

    const auto drawInput = [](const auto id, const std::string& type, const auto& value) {
        return "<input type=\"text\" class=\"form-control\" name=\"" + type + std::to_string(id) + "\" value=\"" + std::to_string(value) + "\">\n";
    };

    for (const auto& item : state->GetMonetizers()) {
        const auto id = item.GetId();
        res
            << "<tr><td>" << id << "</td><td>"
            << item.GetName() << "</td><td>"
            << item.GetTypeString() << "</td><td>"
            << item.GetDiff() << "</td><td>"
            << drawInput(id, "money", item.GetMoney()) << "</td><td>"
            << drawInput(id, "profit", item.GetProfit()) << "</td><td>"
            << drawInput(id, "risk", item.GetRisk()) << "</td></tr>";
    }
    res
        << "</tbody>\n"
        << "</table>\n"
        << "<div class=\"form-group\">\n"
            << "<button type=\"button\" href=\"#\" class=\"btn btn-primary\" onclick=\"return game(this.form, "
            << state->GetMonetizers().size() << ")\">Посчитаем!</button>\n"
        << "</div>\n"
        << "</form>"
        << "<br></br><br></br>\n";
    return res.str();
}

static bool UpdateStateInfo(const TRequest& req) {
    const auto& args = req.GetQueryArgs();
    auto state = req.GetState();

    struct TTmpInfo {
        std::string Money;
        std::string Profit;
        std::string Risk;
    };

    std::unordered_map<uint32_t, TTmpInfo> monetizerInfo;
    for (const auto& item : state->GetMonetizers()) {
        const auto it = args.find("info" + std::to_string(item.GetId()));
        if (it == args.end()) {
            return false;
        }
        std::string info = it->second;
        std::vector<std::string> parts;
        while (info.find(':') != std::string::npos) {
            parts.push_back(info.substr(0, info.find(':')));
            info.erase(0, info.find(':') + 1);
        }
        parts.push_back(info);
        if (parts.size() != 3) {
            return false;
        }
        monetizerInfo[item.GetId()] = {parts[0], parts[1], parts[2]};
    }

    double moneyDiff = 0.;
    for (const auto& item : monetizerInfo) {
        moneyDiff += std::stod(item.second.Money) - state->GetMonetizers()[item.first].GetMoney();
    }
    if (moneyDiff > state->GetMoneyCount()) {
        return false;
    } else {
        state->DeltaMoney(-moneyDiff);
    }
    double income = 0.;
    for (auto& item : state->GetMonetizers()) {
        const auto& info = monetizerInfo[item.GetId()];
        item.SetMoney(std::stod(info.Money));
        item.SetProfit(info.Profit);
        item.SetRisk(info.Risk);
        state->DeltaMoney(item.GetDiff());
        income += item.GetDiff();
    }
    state->SetIncome(income);
    state->DeltaMoney(-income * 0.17);
    return true;
}

std::string TGameHandler::HandleRequest(const TRequest& req) const noexcept {
    auto state = req.GetState();
    if (!state->GetMonth()) {
        state->InitMoney(std::stod(req.GetQueryArgs().at("money")));
    }
    const bool status = !state->GetMonth() || UpdateStateInfo(req);

    std::stringstream response, response_body;
    response_body
        << "<html>\n"
        << "<head>\n"
        << "<title>HTTP Server Response</title>\n"
        << "<link rel=\"stylesheet\" href=\"//yastatic.net/bootstrap/3.3.6/css/bootstrap.min.css\">\n"
        << "</head>\n"
        << "<body>\n"
            << "</br>\n"
            << "</br>\n"
            << "<div class=\"container top-buffer\">\n"
                << "<div class=\"row\">\n"
                    << "<div align=\"center\">\n"
                        << DrawCurrentState(state)
                    << "</div>\n"
                << "</div>\n"
                << "<div class=\"row\">\n"
                    << "<div align=\"center\">\n"
                        << (!status ? "<h1>Некорректные данные! Откат!</h1>" : "")
                    << "</div>\n"
                << "</div>\n"
            << "</div>\n"
        << "</body>\n"
        << "<script type=\"text/javascript\" src=\"//yastatic.net/jquery/2.2.3/jquery.min.js\"></script>\n"
        << "<script type=\"text/javascript\" src=\"//yastatic.net/bootstrap/3.3.6/js/bootstrap.min.js\"></script>\n"
        << "<script>\n"
            << "function game(form, len) {\n"
                << "var elems = form.elements;\n"
                << "var query = \"http://bs-dev08.search.yandex.net:9090/game?month=0\";\n"
                << "for (var i = 0; i < len; i++) {\n"
                    << "var info = \"&info\" + i + \"=\";"
                    << "var money = elems[\"money\" + i].value;"
                    << "var profit = elems[\"profit\" + i].value;"
                    << "var risk = elems[\"risk\" + i].value;"
                    << "var add = info + money + \":\" + profit + \":\" + risk;"
                    << "query += add;"
                << "}\n"
                << "window.location.href = query;\n"
            << "}\n"
        << "</script>\n"
        << "</html>\n";
    response
        << "HTTP/1.1 200 OK\r\n"
        << "Version: HTTP/1.1\r\n"
        << "Content-Type: text/html; charset=utf-8\r\n"
        << "Content-Length: " << response_body.str().length()
        << "\r\n\r\n"
        << response_body.str();

    state->IncMonth();
    return response.str();
}
