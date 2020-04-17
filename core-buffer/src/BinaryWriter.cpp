#include "BinaryWriter.hpp"

BinaryWriter::BinaryWriter(
        const std::string& device_name,
        const std::string& root_folder) :
            device_name_(device_name),
            root_folder_(root_folder)
{

}

void BinaryWriter::write(uint64_t pulse_id, const JFFileFormat& buffer)
{

}

void BinaryWriter::close()
{

}