#include <sstream>
#include <string>

#include "crow_all.h"
#include "RestApi.hpp"

using namespace std;

void RestApi::start_rest_api(WriterManager& writer_manager, uint16_t port)
{
    #ifdef DEBUG_OUTPUT
        cout << "[rest_interface::start_rest_api] Starting rest interface on port " << port << endl;
    #endif

    crow::SimpleApp app;

    CROW_ROUTE(app, "/kill")([&](){
        writer_manager.kill();

        crow::json::wvalue result;

        result["status"] = "killed";
        result["success"] = "True";
        result["value"] = "Ok";
        
        app.stop();

        return result;
    });

    CROW_ROUTE(app, "/stop")([&](){
        writer_manager.stop();

        crow::json::wvalue result;

        result["status"] = writer_manager.get_status();
        result["success"] = "True";
        result["value"] = "Ok";

        return result;
    });

    CROW_ROUTE (app, "/status") ([&](){
        crow::json::wvalue result;

        result["status"] = writer_manager.get_status();
        result["success"] = "True";
        result["value"] = "Ok";

        return result;
    });

    CROW_ROUTE (app, "/statistics") ([&](){
        crow::json::wvalue result;

        for (const auto& item : writer_manager.get_statistics()) {
            result[item.first] = item.second;
        }
        result["start_time"] = writer_manager.get_time_start();
        result["end_time"] = writer_manager.get_time_end();
        result["status"] = writer_manager.get_status();

        auto frame_time_difference = std::chrono::system_clock::now() - writer_manager.get_time_start_point();
        auto time_diff_s = std::chrono::duration<float, milli>(frame_time_difference).count();
        // received_rate = total number of received frames / elapsed time
        auto receiving_rate = float(writer_manager.get_n_received_frames()) / time_diff_s;
        // writing_rate = total number of written frames / elapsed time
        auto writing_rate = float(writer_manager.get_n_written_frames()) / time_diff_s;
        result["receiving_rate"] = receiving_rate;
        result["writing_rate"] = writing_rate;

        result["success"] = "True";
        return result;
    });

    CROW_ROUTE (app, "/parameters").methods("GET"_method, "POST"_method) ([&](const crow::request& req){
        crow::json::wvalue result;
        auto parameters_type = writer_manager.get_parameters_type();

        if (req.method == "GET"_method) {
            for (const auto& item : writer_manager.get_parameters()) {
                auto parameter_name = item.first;
                auto parameter_value = item.second;

                try {
                    auto parameter_type = parameters_type.at(parameter_name);

                    if (parameter_type == NX_FLOAT || parameter_type == NX_NUMBER) {
                        result[parameter_name] = boost::any_cast<double>(parameter_value);

                    } else if (parameter_type == NX_CHAR || parameter_type == NXnote || parameter_type == NX_DATE_TIME) {
                        result[parameter_name] = boost::any_cast<string>(parameter_value);

                    } else if (parameter_type == NX_INT) {
                        result[parameter_name] = boost::any_cast<int>(parameter_value);
                    }
                } catch (const boost::bad_any_cast& exception) {
                    stringstream error_message;
                    using namespace date;
                    error_message << "[" << std::chrono::system_clock::now() << "]";
                    error_message << "[RestApi::parameters(get)] Cannot cast parameter " << parameter_name << " into specified type." << endl;

                    throw runtime_error(error_message.str());
                } catch (const out_of_range& exception){
                    stringstream error_message;
                    using namespace date;
                    error_message << "[" << std::chrono::system_clock::now() << "]";
                    error_message << "[RestApi::parameters(get)] No type mapping for parameter " << parameter_name << " in file format."<< endl;
                    
                    throw runtime_error(error_message.str());
                }
            }

            return result;
        } else {
            auto request_parameters = crow::json::load(req.body);
            std::unordered_map<std::string, boost::any> new_parameters;

            for (const auto& item : request_parameters) {
                string parameter_name = item.key();
                
                try{
                    auto parameter_type = parameters_type.at(parameter_name);

                    if (parameter_type == NX_FLOAT || parameter_type == NX_NUMBER) {
                        new_parameters[parameter_name] = double(item.d());
                    } else if (parameter_type == NX_INT) {
                        new_parameters[parameter_name] = int(item.i());
                    } else if (parameter_type == NX_CHAR) {
                        new_parameters[parameter_name] = string(item.s());
                    } else if (parameter_type == NX_DATE_TIME) {
                        new_parameters[parameter_name] = string(item.s());
                    } else {
                        stringstream error_message;
                        using namespace date;
                        error_message << "[" << std::chrono::system_clock::now() << "]";
                        error_message << "[RestApi::parameters(post)] No NX type mapping for parameter " << parameter_name << endl;

                        throw runtime_error(error_message.str());
                    }
                } catch (const out_of_range& exception){
                    stringstream error_message;
                    using namespace date;
                    error_message << "[" << std::chrono::system_clock::now() << "]";
                    error_message << "[RestApi::parameters(post)] No type mapping for received parameter " << parameter_name << " in file format."<< endl;
                    
                    throw runtime_error(error_message.str());
                } catch (const boost::bad_any_cast& exception) {
                    stringstream error_message;
                    using namespace date;
                    error_message << "[" << std::chrono::system_clock::now() << "]";
                    error_message << "[RestApi::parameters(post)] Cannot cast parameter " << parameter_name << " into specified type." << endl;

                    throw runtime_error(error_message.str());
                }
            }
            
            writer_manager.set_parameters(new_parameters);

            result["message"] = "Parameters set.";
            return result;
        }
    });

    app.loglevel(crow::LogLevel::ERROR);
    app.port(port).run();
}