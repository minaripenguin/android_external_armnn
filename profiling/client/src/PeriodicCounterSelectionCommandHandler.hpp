//
// Copyright © 2019 Arm Ltd and Contributors. All rights reserved.
// SPDX-License-Identifier: MIT
//

#pragma once

#include "ProfilingStateMachine.hpp"
#include "SendCounterPacket.hpp"
#include "IPeriodicCounterCapture.hpp"

#include <client/include/CounterIdMap.hpp>
#include <client/include/Holder.hpp>
#include <client/include/ICounterValues.hpp>

#include <client/include/backends/IBackendProfilingContext.hpp>

#include <common/include/CommandHandlerFunctor.hpp>
#include <common/include/Logging.hpp>
#include <common/include/Packet.hpp>

#include <set>

namespace arm
{

namespace pipe
{


class PeriodicCounterSelectionCommandHandler : public arm::pipe::CommandHandlerFunctor
{

public:
    PeriodicCounterSelectionCommandHandler(uint32_t familyId,
                                           uint32_t packetId,
                                           uint32_t version,
                                           const std::unordered_map<std::string,
                                                   std::shared_ptr<IBackendProfilingContext>>&
                                                   backendProfilingContexts,
                                           const ICounterMappings& counterIdMap,
                                           Holder& captureDataHolder,
                                           const uint16_t maxArmnnCounterId,
                                           IPeriodicCounterCapture& periodicCounterCapture,
                                           const IReadCounterValues& readCounterValue,
                                           ISendCounterPacket& sendCounterPacket,
                                           const ProfilingStateMachine& profilingStateMachine)
        : CommandHandlerFunctor(familyId, packetId, version)
        , m_BackendProfilingContexts(backendProfilingContexts)
        , m_CounterIdMap(counterIdMap)
        , m_CaptureDataHolder(captureDataHolder)
        , m_MaxArmCounterId(maxArmnnCounterId)
        , m_PeriodicCounterCapture(periodicCounterCapture)
        , m_PrevCapturePeriod(0)
        , m_ReadCounterValues(readCounterValue)
        , m_SendCounterPacket(sendCounterPacket)
        , m_StateMachine(profilingStateMachine)

    {

    }

    void operator()(const arm::pipe::Packet& packet) override;

private:

    std::unordered_map<std::string, std::vector<uint16_t>> m_BackendCounterMap;
    const std::unordered_map<std::string,
          std::shared_ptr<IBackendProfilingContext>>& m_BackendProfilingContexts;
    const ICounterMappings& m_CounterIdMap;
    Holder& m_CaptureDataHolder;
    const uint16_t m_MaxArmCounterId;
    IPeriodicCounterCapture& m_PeriodicCounterCapture;
    uint32_t m_PrevCapturePeriod;
    std::set<uint16_t> m_PrevBackendCounterIds;
    const IReadCounterValues& m_ReadCounterValues;
    ISendCounterPacket& m_SendCounterPacket;
    const ProfilingStateMachine& m_StateMachine;

    void ActivateBackendCounters(const std::string backendId,
                                 const uint32_t capturePeriod,
                                 const std::vector<uint16_t> counterIds)
    {
        arm::pipe::Optional<std::string> errorMsg =
                m_BackendProfilingContexts.at(backendId)->ActivateCounters(capturePeriod, counterIds);

        if(errorMsg.has_value())
        {
            ARM_PIPE_LOG(warning) << "An error has occurred when activating counters of " << backendId << ": "
                               << errorMsg.value();
        }
    }
    void ParseData(const arm::pipe::Packet& packet, CaptureData& captureData);
    std::set<std::string> ProcessBackendCounterIds(const uint32_t capturePeriod,
                                                        const std::set<uint16_t> newCounterIds,
                                                        const std::set<uint16_t> unusedCounterIds);

};

} // namespace pipe

} // namespace arm
