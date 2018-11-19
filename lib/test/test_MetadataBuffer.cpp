
#include "../src/MetadataBuffer.hpp"
using namespace std;

TEST(MetadataBuffer, basic_operations)
{

    int n_frames = 10;
    int n_modules = 3;

    auto header_values = shared_ptr<unordered_map<string, HeaderDataType>>(new unordered_map<string, HeaderDataType> {
        {"frame", HeaderDataType("uint64")},
        {"module_number", HeaderDataType("uint64", n_modules)}
    });

    MetadataBuffer metadata_buffer(10, header_values);

    uint64_t base_frame_data = 12345678;
    uint64_t base_module_number = 0;

    for (int i = 0; i<n_frames; i++) {
	uint64_t frame_index = base_frame_data + i;
	metadata_buffer.add_metadata_to_buffer("frame", i, (char*)&frame_index);

	uint64_t module_number = base_module_number + i;
	uint64_t module_number_data[n_modules];

	for (int module_index=0; module_index<n_modules; module_index++) {
	    module_number_data[module_index] = module_number;
	}
	
	metadata_buffer.add_metadata_to_buffer("module_number", i, (char*)&module_number_data);
    }

    uint64_t* frame_data = (uint64_t*) metadata_buffer.get_metadata_values("frame").get();
    uint64_t* modulo_number_data = (uint64_t*) metadata_buffer.get_metadata_values("modulo_number").get();

    for (int i = 0; i<n_frames; i++) {
	EXPECT_EQ(frame_data[i], base_frame_data+i);
    }

}

