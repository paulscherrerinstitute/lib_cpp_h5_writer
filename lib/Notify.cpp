
void PSIWriter::notify_first_pulse_id(uint64_t pulse_id) 
{
    string request_address(bsread_rest_address);

    async(launch::async, [pulse_id, &request_address]{
        try {

            cout << "Sending first received pulse_id " << pulse_id <<;
            cout << " to bsread_rest_address " << request_address << endl;

            stringstream request;
            request << "curl -X PUT " << request_address;
            request << "/start_pulse_id/" << pulse_id;

            string request_call(request.str());

            #ifdef DEBUG_OUTPUT
                using namespace date;
                cout << "[" << chrono::system_clock::now() << "]";
                cout << "[ProcessManager::notify_first_pulse_id] Sending request";
                cout << "(" << request_call << ")." << endl;
            #endif

            system(request_call.c_str());
        } catch (...){}
        
    });
}


void PSIWriter::notify_last_pulse_id(uint64_t pulse_id) 
{

    try {
        cout << "Sending last received pulse_id " << pulse_id;
        cout << " to bsread address " << bsread_rest_address << endl;

        stringstream request;
        request << "curl -X PUT " << bsread_rest_address;
        request << "/stop_pulse_id/" << pulse_id;

        cout << "Request: " << request.str() << endl;

        string request_call(request.str());

        #ifdef DEBUG_OUTPUT
            using namespace date;
            cout << "[" << chrono::system_clock::now() << "]";
            cout << "[ProcessManager::notify_last_pulse_id] Sending request";
            cout << "(" << request_call << ")." << endl;
        #endif

        system(request_call.c_str());
    } catch (...){}
}

