#include <sstream>
#include <string>

#include "crow_all.h"
#include "RestApi.hpp"

using namespace std;

void RestApi::start_rest_api(WriterManager& writer_manager, uint16_t port)
{

    #ifdef DEBUG_OUTPUT
        cout << "[rest_interface::start_rest_api] Starting rest interface on port ";
        cout << port << endl;

        cout << "[rest_interface::start_rest_api] Accepting start parameters:" << endl;
        for (const auto& item : rest_start_parameters) {
            cout << "\t " << item.key() << endl;
        }
    #endif

    crow::SimpleApp app;

    CROW_ROUTE(app, "/stop")([&](){
        writer_manager.stop();

        crow::json::wvalue result;
        result["state"] = "ok";
        result["status"] = writer_manager.get_status();

        return result;
    });

    CROW_ROUTE (app, "/status") ([&](){
        crow::json::wvalue result;
        result["state"] = "ok";
        result["status"] = writer_manager.get_status();

        return result;
    });

    CROW_ROUTE (app, "/statistics") ([&](){
        crow::json::wvalue result;

        for (const auto& item : writer_manager.get_statistics()) {
            result[item.first] = item.second;
        }

        result["state"] = "ok";
        result["status"] = writer_manager.get_status();

        return result;
    });

    CROW_ROUTE (app, "/start").methods("POST"_method) ([&](const crow::request& req)
    {
        auto request_parameters = crow::json::load(req.body);

        int n_frames = request_parameters["n_frames"].i();
        string output_file = request_parameters["output_file"].s();
        int user_id = request_parameters["user_id"].i();

        writer_manager.start(output_file, n_frames, user_id);

        crow::json::wvalue result;
        result["message"] = "Writer started.";
        result["status"] = writer_manager.get_status();
        return result;
    });

    app.loglevel(crow::LogLevel::ERROR);
    app.port(port).run();
}
