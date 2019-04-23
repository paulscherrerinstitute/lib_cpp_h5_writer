#include <sstream>
#include <string>

#include "crow_all.h"
#include "RestApi.hpp"

using namespace std;

const unordered_map<string, DATA_TYPE> RestApi::rest_start_parameters = 
{
    {"n_frames", NX_INT},
    {"user_id", NX_INT},
    {"output_file", NX_CHAR}
};


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

    CROW_ROUTE (app, "/start").methods("POST"_method) ([&](const crow::request& req){
        auto parameters_type = writer_manager.get_parameters_type();

        auto request_parameters = crow::json::load(req.body);
        std::unordered_map<std::string, boost::any> parameters;

        for (const auto& item : request_parameters) {
            string parameter_name = item.key();
            
            try{
                auto parameter_type = parameters_type.at(parameter_name);

                if (parameter_type == NX_FLOAT || parameter_type == NX_NUMBER) {
                    parameters[parameter_name] = double(item.d());
                } else if (parameter_type == NX_INT) {
                    parameters[parameter_name] = int(item.i());
                } else if (parameter_type == NX_CHAR) {
                    parameters[parameter_name] = string(item.s());
                } else if (parameter_type == NX_DATE_TIME) {
                    parameters[parameter_name] = string(item.s());
                } else {
                    stringstream error_message;
                    using namespace date;
                    error_message << "[" << std::chrono::system_clock::now() << "]";
                    error_message << "[RestApi::start(post)] No NX type mapping ";
                    error_message << "for parameter " << parameter_name << endl;

                    throw runtime_error(error_message.str());
                }
                
            } catch (const out_of_range& exception){
                stringstream error_message;
                using namespace date;
                error_message << "[" << std::chrono::system_clock::now() << "]";
                error_message << "[RestApi::start(post)] No type mapping ";
                error_message << "for received parameter " << parameter_name << endl;
                
                throw runtime_error(error_message.str());

            } catch (const boost::bad_any_cast& exception) {
                stringstream error_message;
                using namespace date;
                error_message << "[" << std::chrono::system_clock::now() << "]";
                error_message << "[RestApi::start(post)] Cannot cast parameter ";
                error_message << parameter_name << " into specified type." << endl;

                throw runtime_error(error_message.str());

            }
        }
        
        writer_manager.start(parameters);

        crow::json::wvalue result;
        result["message"] = "Writer started.";
        result["status"] = writer_manager.get_status();
        return result;
    });

    app.loglevel(crow::LogLevel::ERROR);
    app.port(port).run();
}
