//
// Copyright © 2017 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "ElementwiseBaseLayer.hpp"

namespace armnn
{
/// This layer represents an addition operation.
class AdditionLayer : public ElementwiseBaseLayer
{
public:
    /// Makes a workload for the Addition type.
    /// @param [in] graph The graph where this layer can be found.
    /// @param [in] factory The workload factory which will create the workload.
    /// @return A pointer to the created workload, or nullptr if not created.
    virtual std::unique_ptr<IWorkload> CreateWorkload(const IWorkloadFactory& factory) const override;

    /// Creates a dynamically-allocated copy of this layer.
    /// @param [in] graph The graph into which this layer is being cloned.
    AdditionLayer* Clone(Graph& graph) const override;

    void ExecuteStrategy(IStrategy& strategy) const override;

protected:
    /// Constructor to create an AdditionLayer.
    /// @param [in] name Optional name for the layer.
    AdditionLayer(const char* name);

    /// Default destructor
    ~AdditionLayer() = default;
};

} // namespace
