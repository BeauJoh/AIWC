// WorkloadCharacterisation.h (Oclgrind)
// Copyright (c) 2017, Beau Johnston,
// The Australian National University. All rights reserved.
//
// This program is provided under a three-clause BSD license. For full
// license terms please see the LICENSE file distributed with this
// source code.

#include "core/Plugin.h"

#include <mutex>
#if LEVELDB_ENABLED
#include <leveldb/db.h>
#include <fstream>
#include <iostream>
#endif

namespace llvm
{
  class Function;
}

namespace oclgrind
{
  class WorkloadCharacterisation : public Plugin
  {
  public:
    WorkloadCharacterisation(const Context *context) : Plugin(context){};
    ~WorkloadCharacterisation();
    virtual void instructionExecuted(const WorkItem *workItem,
                                     const llvm::Instruction *instruction,
                                     const TypedValue& result) override;
    virtual void memoryLoad(const Memory *memory, const WorkItem *workItem,size_t address, size_t size) override;
    virtual void memoryStore(const Memory *memory, const WorkItem *workItem,size_t address, size_t size,  const uint8_t *storeData) override;
    virtual void memoryAtomicLoad(const Memory *memory, const WorkItem *workItem, AtomicOp op, size_t address, size_t size) override;
    virtual void memoryAtomicStore(const Memory *memory, const WorkItem *workItem, AtomicOp op, size_t address, size_t size) override;
    virtual void kernelBegin(const KernelInvocation *kernelInvocation) override;
    virtual void kernelEnd(const KernelInvocation *kernelInvocation) override;
    virtual void workGroupBegin(const WorkGroup *workGroup) override;
    virtual void workGroupComplete(const WorkGroup *workGroup) override;
    virtual void workItemBegin(const WorkItem *workItem) override;
    virtual void workItemComplete(const WorkItem *workItem) override;
    virtual void workItemBarrier(const WorkItem *workItem) override;
    virtual void workItemClearBarrier(const WorkItem *workItem) override;

  private:
#if LEVELDB_ENABLED
    std::string load_db_name;
    std::string store_db_name;
    std::vector<std::pair<std::string,leveldb::DB*>> db_tracefile_handles;
#else
    std::vector<std::pair<size_t,size_t>> m_loadMemoryOps;
    std::vector<std::pair<size_t,size_t>> m_storeMemoryOps;
    std::unordered_map<std::string,size_t> m_computeOps;
    std::unordered_map<unsigned,std::vector<bool>> m_branchOps;
    std::vector<float> m_instructionsToBarrier;
    std::vector<size_t> m_instructionWidth;
    unsigned m_threads_invoked;
    unsigned m_barriers_hit;
#endif

    struct WorkerState
    {
      std::unordered_map<std::string,size_t> *computeOps;
      std::vector<std::pair<size_t,size_t>> *loadMemoryOps;
      std::vector<std::pair<size_t,size_t>> *storeMemoryOps;
      bool previous_instruction_is_branch;
      std::string target1, target2;
      unsigned branch_loc;
      std::unordered_map<unsigned,std::vector<bool>> *branchOps;
      unsigned threads_invoked;
      unsigned barriers_hit;
      unsigned instruction_count;
      std::vector<unsigned> *instructionsBetweenBarriers;
      std::vector<unsigned> *instructionWidth;
    };
    static THREAD_LOCAL WorkerState m_state;

    std::mutex m_mtx;

  };
}
