#include "init_handler.h"

#include <iostream>
#include <sstream>

static std::string DrawMonetizers(const std::vector<TMonetizer>& monetizers) {
    std::stringstream res;
    res
        << "<table class=\"table table-hover\">\n"
            << "<thead><tr>\n"
                << "<th>№</th>\n"
                << "<th>Название</th>\n"
                << "<th>Тип</th>\n"
                << "<th>Доходность</th>\n"
                << "<th>Риск</th>\n"
            << "</tr></thead>\n"
            << "<tbody>\n";
    for (const auto& m : monetizers) {
        res
            << "<tr><td>" << m.GetId() << "</td><td>" << m.GetName()
            << "</td><td>" << m.GetTypeString() << "</td><td>" << m.GetProfit()
            << "</td><td>" << m.GetRisk() << "</td></tr>";
    }
    res
        << "</tbody>\n"
        << "</table>\n"
        << "<br></br><br></br>\n";
    return res.str();
}

std::string TInitHandler::HandleRequest(const TRequest& req) const noexcept {
    auto state = req.GetState();
    std::stringstream response_body, response;
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
                    << "<form class=\"form-horizontal\">\n"
                        << "<div class=\"form-group\">\n"
                            << "<label for=\"investName\" class=\"col-sm-3 control-label\">Введите название вложения:</label>\n"
                            << "<div class=\"col-sm-5\">\n"
                                << "<input type=\"text\" class=\"form-control\" name=\"investname\" placeholder=\"Акции Яндекса\">\n"
                            << "</div>\n"
                        << "</div>\n"
                        << "<div class=\"form-group\">\n"
                            << "<label for=\"investType\" class=\"col-sm-3 control-label\">Выберите тип вложения:</label>\n"
                            << "<div class=\"col-sm-5\">\n"
                                << "<select class=\"form-control\" name=\"investtype\">\n"
                                    << "<option>Акции</option>"
                                    << "<option>Депозит</option>"
                                    << "<option>Валюта</option>"
                                    << "<option>Облигации</option>"
                                    << "<option>Драгоценные металлы</option>"
                                << "</select>\n"
                            << "</div>\n"
                        << "</div>\n"
                        << "<div class=\"form-group\">\n"
                            << "<label for=\"investProfit\" class=\"col-sm-3 control-label\">Доходность за месяц в процентах:</label>\n"
                            << "<div class=\"col-sm-5\">\n"
                                << "<input type=\"text\" class=\"form-control\" name=\"investprofit\" value=\"auto\">\n"
                            << "</div>\n"
                        << "</div>\n"
                        << "<div class=\"form-group\">\n"
                            << "<label for=\"investRisk\" class=\"col-sm-3 control-label\">Уровень риска в процентах:</label>\n"
                            << "<div class=\"col-sm-5\">\n"
                                << "<input type=\"text\" class=\"form-control\" name=\"investrisk\" value=\"auto\">\n"
                            << "</div>\n"
                        << "</div>\n"
                        << "<div class=\"form-group\">\n"
                            << "<div class=\"col-md-offset-3 col-md-6\">\n"
                                << "<button type=\"button\" href=\"#\" class=\"btn btn-primary\" onclick=\"return show(this.form)\">Добавить</button>\n"
                            << "</div>\n"
                        << "</div>\n"
                    << "</form>\n"
                << "</div>\n"

                << "<div class=\"row\">\n"
                    << "<div class=\"col-md-offset-1 col-md-10\" align=\"center\">\n"
                        << DrawMonetizers(state->GetMonetizers())
                    << "</div>\n"
                << "</div>\n"

                << "<div class=\"row\">\n"
                    << "<form class=\"form-horizontal\">\n"
                        << "<div class=\"form-group\">\n"
                            << "<label for=\"moneyStart\" class=\"col-sm-3 control-label\">Начальный капитал:</label>\n"
                            << "<div class=\"col-sm-5\">\n"
                                << "<input type=\"text\" class=\"form-control\" name=\"moneystart\" placeholder=\"100000\">\n"
                            << "</div>\n"
                        << "</div>\n"
                        << "<div class=\"form-group\">\n"
                            << "<div class=\"col-md-offset-3 col-md-6\">\n"
                                << "<button type=\"button\" href=\"#\" class=\"btn btn-success\" onclick=\"return game(this.form)\">Начать игру!</button>\n"
                            << "</div>\n"
                        << "</div>\n"
                    << "</form>\n"
                << "</div>\n"
            << "</div>\n"
        << "</body>\n"
        << "<script type=\"text/javascript\" src=\"//yastatic.net/jquery/2.2.3/jquery.min.js\"></script>\n"
        << "<script type=\"text/javascript\" src=\"//yastatic.net/bootstrap/3.3.6/js/bootstrap.min.js\"></script>\n"
        << "<script>\n"
            << "function show(form) {\n"
                << "var elems = form.elements;\n"
                << "var query = \"http://bs-dev08.search.yandex.net:9090/add\";\n"
                << "var investName = \"?invest-name=\" + elems.investname.value;"
                << "var investType = \"&invest-type=\" + elems.investtype.value;"
                << "var investProfit = \"&invest-profit=\" + elems.investprofit.value;"
                << "var investRisk = \"&invest-risk=\" + elems.investrisk.value;"
                << "window.location.href = query + investName + investType + investProfit + investRisk;\n"
            << "}\n"
        << "</script>\n"
        << "<script>\n"
            << "function game(form) {\n"
                << "var elems = form.elements;\n"
                << "var query = \"http://bs-dev08.search.yandex.net:9090/game?money=\";\n"
                << "window.location.href = query + elems.moneystart.value;\n"
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

    return response.str();
}

