/* Copyright 2024 The OpenXLA Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/service/memory_space_assignment/simulator.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "xla/hlo/ir/hlo_instruction.h"
#include "xla/hlo/ir/hlo_opcode.h"
#include "xla/hlo/utils/hlo_live_range.h"
#include "xla/service/hlo_value.h"
#include "xla/service/memory_space_assignment/allocation.h"
#include "xla/shape_util.h"
#include "xla/util.h"

namespace xla {
namespace memory_space_assignment {

float RuntimeSimulator::SimulateElapsedTimeWithoutAsyncCopies(
    const HloLiveRange& hlo_live_range, const AllocationSequence& allocations) {
  absl::flat_hash_map<const HloInstruction*, std::vector<ShapeIndex>>
      outputs_in_alternate_memory_map;
  absl::flat_hash_map<const HloInstruction*,
                      std::vector<std::pair<int64_t, ShapeIndex>>>
      operands_in_alternate_memory_map;

  for (auto& allocation : allocations) {
    if (!allocation->is_copy_allocation()) {
      if (allocation->memory_space() == MemorySpace::kAlternate) {
        const HloInstruction* defining_instruction =
            allocation->defining_position().instruction;
        outputs_in_alternate_memory_map[defining_instruction].push_back(
            allocation->defining_position().index);
      }
    }
    for (auto& hlo_use : allocation->uses()) {
      const HloInstruction* use_instruction = hlo_use.instruction;
      operands_in_alternate_memory_map[use_instruction].push_back(
          std::make_pair(hlo_use.operand_number, hlo_use.operand_index));
    }
  }

  const auto& instruction_sequence =
      hlo_live_range.flattened_instruction_sequence().instructions();
  float total_elapsed = 0.0;
  for (const HloInstruction* instruction : instruction_sequence) {
    if (instruction->opcode() == HloOpcode::kWhile) {
      continue;
    }
    std::vector<ShapeIndex> outputs_in_alternate_memory;
    auto output_it = outputs_in_alternate_memory_map.find(instruction);
    if (output_it != outputs_in_alternate_memory_map.end()) {
      outputs_in_alternate_memory = output_it->second;
    }
    std::vector<std::pair<int64_t, ShapeIndex>> operands_in_alternate_memory;
    auto operand_it = operands_in_alternate_memory_map.find(instruction);
    if (operand_it != operands_in_alternate_memory_map.end()) {
      operands_in_alternate_memory = operand_it->second;
    }

    float instruction_elapsed_per_invoke =
        cost_analysis_->GetInstructionElapsedInAlternateMemory(
            *instruction, operands_in_alternate_memory,
            outputs_in_alternate_memory);
    float total_trip_count = cost_analysis_->CalculateNestTripCount(
        instruction, &cost_analysis_cache_);
    // Calculate total elapsed time by summing up the overall elapsed time of
    // each instruction.
    total_elapsed += total_trip_count * instruction_elapsed_per_invoke;
  }
  return total_elapsed;
}

float RuntimeSimulator::SimulateAsyncCopyTransfer(
    float bytes_to_transfer,
    std::queue<const HloInstruction*>& memory_access_queue_to_share_bandwidth,
    absl::flat_hash_map<const HloInstruction*, float>&
        remaining_size_of_buffers,
    float default_memory_bytes_per_second) {
  float remaining_bytes = bytes_to_transfer;
  float elapsed_time = 0.0;
  while (!memory_access_queue_to_share_bandwidth.empty() &&
         remaining_bytes > 0) {
    const HloInstruction* front_async_copy =
        memory_access_queue_to_share_bandwidth.front();
    float smaller_buffer_size = std::min(
        remaining_bytes, remaining_size_of_buffers.at(front_async_copy));
    // The bandwidth is shared, so the request can only use half of the
    // bandwidth.
    elapsed_time +=
        smaller_buffer_size / (0.5 * default_memory_bytes_per_second);
    remaining_bytes -= smaller_buffer_size;
    remaining_size_of_buffers.at(front_async_copy) -= smaller_buffer_size;
    if (remaining_size_of_buffers.at(front_async_copy) <= 0) {
      remaining_size_of_buffers.erase(front_async_copy);
      memory_access_queue_to_share_bandwidth.pop();
    }
  }
  if (remaining_bytes > 0) {
    // The queue that shares the bandwidth is drained, we can now use the full
    // bandwidth.
    elapsed_time += remaining_bytes / default_memory_bytes_per_second;
  }
  return elapsed_time;
};

void RuntimeSimulator::ProcessAsyncCopyInTimeWindow(
    float time_windows, std::queue<const HloInstruction*>& read_queue,
    std::queue<const HloInstruction*>& write_queue,
    absl::flat_hash_map<const HloInstruction*, float>&
        remaining_size_of_buffers,
    float default_memory_bytes_per_second) {
  float elapsed_time = time_windows;
  while (!read_queue.empty() || !write_queue.empty()) {
    if (elapsed_time <= 0) {
      // Run out of time, return
      return;
    }
    if (!read_queue.empty() && !write_queue.empty()) {
      // Both queues are not empty, share the bandwidth between them.
      const HloInstruction* front_read_default_async_copy = read_queue.front();
      const HloInstruction* front_write_default_async_copy =
          write_queue.front();
      float smaller_buffer_size = std::min(
          remaining_size_of_buffers.at(front_read_default_async_copy),
          remaining_size_of_buffers.at(front_write_default_async_copy));
      float required_time =
          smaller_buffer_size / (0.5 * default_memory_bytes_per_second);
      if (required_time > elapsed_time) {
        // The required time is larger than the remaining
        // computation time, use the remaining computation time as
        // the required time to transfer a part of the buffer.
        required_time = elapsed_time;
        smaller_buffer_size =
            required_time * (0.5 * default_memory_bytes_per_second);
      }
      elapsed_time -= required_time;
      remaining_size_of_buffers.at(front_read_default_async_copy) -=
          smaller_buffer_size;
      remaining_size_of_buffers.at(front_write_default_async_copy) -=
          smaller_buffer_size;
      if (remaining_size_of_buffers.at(front_read_default_async_copy) <= 0) {
        remaining_size_of_buffers.erase(front_read_default_async_copy);
        read_queue.pop();
      }
      if (remaining_size_of_buffers.at(front_write_default_async_copy) <= 0) {
        remaining_size_of_buffers.erase(front_write_default_async_copy);
        write_queue.pop();
      }
    } else {
      // One of the queue is not empty, execute the async copy from
      // that queue with full bandwidth.
      std::queue<const HloInstruction*>& queue =
          read_queue.empty() ? write_queue : read_queue;
      const HloInstruction* front_async_copy = queue.front();
      float required_time = remaining_size_of_buffers.at(front_async_copy) /
                            default_memory_bytes_per_second;
      if (required_time > elapsed_time) {
        required_time = elapsed_time;
      }
      elapsed_time -= required_time;
      remaining_size_of_buffers.at(front_async_copy) -=
          required_time * default_memory_bytes_per_second;
      if (remaining_size_of_buffers.at(front_async_copy) <= 0) {
        remaining_size_of_buffers.erase(queue.front());
        queue.pop();
      }
    }
  }
}

}  // namespace memory_space_assignment
}  // namespace xla
