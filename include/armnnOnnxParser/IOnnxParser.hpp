//
// Copyright © 2017,2022 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//
#pragma once

#include <armnn/INetwork.hpp>
#include <armnn/Tensor.hpp>

#include <memory>
#include <vector>
#include <map>

namespace armnnOnnxParser
{

using BindingPointInfo = armnn::BindingPointInfo;

class OnnxParserImpl;
class IOnnxParser;
using IOnnxParserPtr = std::unique_ptr<IOnnxParser, void(*)(IOnnxParser* parser)>;

class IOnnxParser
{
public:
    static IOnnxParser* CreateRaw();
    static IOnnxParserPtr Create();
    static void Destroy(IOnnxParser* parser);

    /// Create the network from a protobuf binary vector
    armnn::INetworkPtr CreateNetworkFromBinary(const std::vector<uint8_t>& binaryContent);

    /// Create the network from a protobuf binary vector, with inputShapes specified
    armnn::INetworkPtr CreateNetworkFromBinary(const std::vector<uint8_t>& binaryContent,
                                               const std::map<std::string, armnn::TensorShape>& inputShapes);

    /// Create the network from a protobuf binary file on disk
    armnn::INetworkPtr CreateNetworkFromBinaryFile(const char* graphFile);

    /// Create the network from a protobuf text file on disk
    armnn::INetworkPtr CreateNetworkFromTextFile(const char* graphFile);

    /// Create the network directly from protobuf text in a string. Useful for debugging/testing
    armnn::INetworkPtr CreateNetworkFromString(const std::string& protoText);

    /// Create the network from a protobuf binary file on disk, with inputShapes specified
    armnn::INetworkPtr CreateNetworkFromBinaryFile(const char* graphFile,
                                                   const std::map<std::string, armnn::TensorShape>& inputShapes);

    /// Create the network from a protobuf text file on disk, with inputShapes specified
    armnn::INetworkPtr CreateNetworkFromTextFile(const char* graphFile,
                                                 const std::map<std::string, armnn::TensorShape>& inputShapes);

     /// Create the network directly from protobuf text in a string, with inputShapes specified.
     /// Useful for debugging/testing
    armnn::INetworkPtr CreateNetworkFromString(const std::string& protoText,
                                               const std::map<std::string, armnn::TensorShape>& inputShapes);

    /// Retrieve binding info (layer id and tensor info) for the network input identified by the given layer name
    BindingPointInfo GetNetworkInputBindingInfo(const std::string& name) const;

    /// Retrieve binding info (layer id and tensor info) for the network output identified by the given layer name
    BindingPointInfo GetNetworkOutputBindingInfo(const std::string& name) const;

private:
    IOnnxParser();
    ~IOnnxParser();

    std::unique_ptr<OnnxParserImpl> pOnnxParserImpl;
  };

  }
