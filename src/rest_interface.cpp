#include <sstream>
#include <string>

#include "rest_interface.hpp"
#include "crow_all.h"

void start_rest_api(WriterManager& writer_manager, uint16_t port)
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/kill")([&](){
        writer_manager.stop();

        crow::json::wvalue result;

        result["status"] = "Writer killed.";
        
        app.stop();

        return result;
    });

    CROW_ROUTE(app, "/stop")([&](){
        writer_manager.stop();

        crow::json::wvalue result;

        result["status"] = writer_manager.get_status();

        return result;
    });

    CROW_ROUTE (app, "/status") ([&](){
        crow::json::wvalue result;

        result["status"] = writer_manager.get_status();

        return result;
    });

    CROW_ROUTE (app, "/statistics") ([&](){
        crow::json::wvalue result;

        for (const auto& item : writer_manager.get_statistics()) {
            result[item.first] = item.second;
        }

        return result;
    });

    CROW_ROUTE (app, "/parameters").methods("GET"_method, "POST"_method) ([&](const crow::request& req){
        crow::json::wvalue result;

        if (req.method == "GET"_method) {

            for (const auto& item : writer_manager.get_paramters()) {
                result[item.first] = item.second;
            }

            return result;
        } else {
            auto request_parameters = crow::json::load(req.body);

            // TODO: Fix this.
            
            // const std::map<std::string, std::string> parameters_to_set;

            // for (const auto& item : request_parameters) {
            //     parameters_to_set[item.first] = item.second;
            // }
            
            // writer_manager.set_parameters(parameters_to_set);

            result["message"] = "Parameters set.";
            return result;
        }
    });

    app.loglevel(crow::LogLevel::INFO);
    app.port(port).run();
}