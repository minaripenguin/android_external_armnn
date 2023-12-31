//
// Copyright © 2020 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "ClContextDeserializer.hpp"
#include "ClContextSchema_generated.h"

#include <armnn/Exceptions.hpp>
#include <armnn/utility/NumericCast.hpp>
#include <armnn/Logging.hpp>

#include <flatbuffers/flexbuffers.h>

#include <fmt/format.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>


#if defined(__linux__)
#define SERIALIZER_USE_MMAP 1
#if SERIALIZER_USE_MMAP
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#endif

namespace armnn
{

void ClContextDeserializer::Deserialize(arm_compute::CLCompileContext& clCompileContext,
                                        cl::Context& context,
                                        cl::Device& device,
                                        const std::string& filePath)
{
    std::vector<std::uint8_t> binaryContent;
#if !SERIALIZER_USE_MMAP
    std::ifstream inputFileStream(filePath, std::ios::binary);
    while (inputFileStream)
    {
        char input;
        inputFileStream.get(input);
        if (inputFileStream)
        {
            binaryContent.push_back(static_cast<std::uint8_t>(input));
        }
    }
    inputFileStream.close();
#else
    struct stat statbuf;
    int fp = open(filePath.c_str(),O_RDONLY);
    if (!fp)
    {
        ARMNN_LOG(error) << (std::string("Cannot open file ") + filePath);
        return;
    }
    fstat(fp,&statbuf);
    const unsigned long dataSize = static_cast<unsigned long>(statbuf.st_size);
    binaryContent.resize(static_cast<long unsigned int>(dataSize));
    void* ptrmem = mmap(NULL, dataSize,PROT_READ,MAP_PRIVATE,fp,0);
    if(ptrmem!=MAP_FAILED)
    {
         memcpy (binaryContent.data(), ptrmem, dataSize);
    }
    close(fp);
    if(ptrmem == MAP_FAILED)
    {
        ARMNN_LOG(error) << (std::string("Cannot map file ") + filePath);
        return;
    }
#endif

    DeserializeFromBinary(clCompileContext, context, device, binaryContent);
}

void ClContextDeserializer::DeserializeFromBinary(arm_compute::CLCompileContext& clCompileContext,
                                                  cl::Context& context,
                                                  cl::Device& device,
                                                  const std::vector<uint8_t>& binaryContent)
{
    if (binaryContent.data() == nullptr)
    {
        throw InvalidArgumentException(fmt::format("Invalid (null) binary content {}",
                                                   CHECK_LOCATION().AsString()));
    }

    size_t binaryContentSize = binaryContent.size();
    flatbuffers::Verifier verifier(binaryContent.data(), binaryContentSize);
    if (verifier.VerifyBuffer<ClContext>() == false)
    {
        throw ParseException(fmt::format("Buffer doesn't conform to the expected Armnn "
                                         "flatbuffers format. size:{0} {1}",
                                         binaryContentSize,
                                         CHECK_LOCATION().AsString()));
    }
    auto clContext = GetClContext(binaryContent.data());

    for (Program const* program : *clContext->programs())
    {
        const char* volatile programName = program->name()->c_str();
        auto programBinary = program->binary();
        std::vector<uint8_t> binary(programBinary->begin(), programBinary->begin() + programBinary->size());

        cl::Program::Binaries   binaries{ binary };
        std::vector<cl::Device> devices {device};
        cl::Program             theProgram(context, devices, binaries);
        theProgram.build();
        clCompileContext.add_built_program(programName, theProgram);
    }
}

} // namespace armnn
