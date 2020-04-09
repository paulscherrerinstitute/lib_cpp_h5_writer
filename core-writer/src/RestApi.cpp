#include <sstream>
#include <string>

#include "crow_all.h"
#include "RestApi.hpp"

using namespace std;

void RestApi::start_rest_api(ProcessManager& manager, uint16_t port)
{

    #ifdef DEBUG_OUTPUT
        cout << "[RestApi::start_rest_api]";
        cout << " Starting rest interface on port ";
        cout << static_cast<int>(port) << endl;
    #endif

    crow::SimpleApp app;

    CROW_ROUTE (app, "/writing").methods("POST"_method, "DELETE"_method)
            ([&](const crow::request& req)
             {
                 if (req.method == "POST"_method) {
                     auto request_parameters = crow::json::load(req.body);

                     string output_file =
                             request_parameters["output_file"].s();
                     int n_frames =
                             request_parameters["n_frames"].i();
                     int user_id =
                             request_parameters["user_id"].i();

                     manager.start_writing(output_file, n_frames, user_id);

                     crow::json::wvalue result;
                     result["state"] = "ok";
                     result["status"] = manager.get_status();
                     result["message"] = "Writing started.";

                     return result;
                 }

                 if (req.method == "DELETE"_method) {
                     manager.stop_writing();

                     crow::json::wvalue result;
                     result["state"] = "ok";
                     result["status"] = manager.get_status();
                     result["message"] = "Writing stopped.";

                     return result;
                 }

                 throw runtime_error("You should not see this.");

             });

    CROW_ROUTE (app, "/receiving").methods("POST"_method, "DELETE"_method)
            ([&](const crow::request& req)
             {
                 if (req.method == "POST"_method) {

                     auto request_parameters = crow::json::load(req.body);

                     string url =
                             request_parameters["connect_address"].s();
                     int n_threads =
                             request_parameters["n_receiving_threads"].i();

                     manager.start_receiving(url, n_threads);

                     crow::json::wvalue result;
                     result["state"] = "ok";
                     result["status"] = manager.get_status();
                     result["message"] = "Receiving started.";

                     return result;
                 }

                 if (req.method == "DELETE"_method) {
                     manager.stop_receiving();

                     crow::json::wvalue result;
                     result["state"] = "ok";
                     result["status"] = manager.get_status();
                     result["message"] = "Receiving stopped.";

                     return result;
                 }

                 throw runtime_error("You should not see this.");
             });

    CROW_ROUTE (app, "/status")
           ([&](){
                crow::json::wvalue result;
                result["state"] = "ok";
                result["status"] = manager.get_status();

                return result;
        });

    CROW_ROUTE (app, "/statistics")
    ([&](){
        crow::json::wvalue result;

        for (const auto& item : manager.get_statistics()) {
            result[item.first] = item.second;
        }

        result["state"] = "ok";
        result["status"] = manager.get_status();

        return result;
    });

    app.loglevel(crow::LogLevel::ERROR);
    app.port(port).run();
}
