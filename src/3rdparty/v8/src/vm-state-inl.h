// Copyright 2010 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef V8_VM_STATE_INL_H_
#define V8_VM_STATE_INL_H_

#include "vm-state.h"

namespace v8 {
namespace internal {

//
// VMState class implementation.  A simple stack of VM states held by the
// logger and partially threaded through the call stack.  States are pushed by
// VMState construction and popped by destruction.
//
#ifdef ENABLE_VMSTATE_TRACKING
inline const char* StateToString(StateTag state) {
  switch (state) {
    case JS:
      return "JS";
    case GC:
      return "GC";
    case COMPILER:
      return "COMPILER";
    case OTHER:
      return "OTHER";
    default:
      UNREACHABLE();
      return NULL;
  }
}

VMState::VMState(Isolate* isolate, StateTag state)
    : isolate_(isolate),
      disabled_(true),
      state_(OTHER),
      external_callback_(NULL) {
  ASSERT(isolate == Isolate::Current());

#ifdef ENABLE_LOGGING_AND_PROFILING
  if (!isolate_->logger()->is_logging() &&
      !CpuProfiler::is_profiling(isolate_)) {
    return;
  }
#endif

  disabled_ = false;
#if !defined(ENABLE_HEAP_PROTECTION)
  // When not protecting the heap, there is no difference between
  // EXTERNAL and OTHER.  As an optimization in that case, we will not
  // perform EXTERNAL->OTHER transitions through the API.  We thus
  // compress the two states into one.
  if (state == EXTERNAL) state = OTHER;
#endif
  state_ = state;
  // Save the previous state.
  previous_ = isolate_->current_vm_state();
  // Install the new state.
  isolate_->set_current_vm_state(this);

#ifdef ENABLE_LOGGING_AND_PROFILING
  if (FLAG_log_state_changes) {
    LOG(UncheckedStringEvent("Entering", StateToString(state_)));
    if (previous_ != NULL) {
      LOG(UncheckedStringEvent("From", StateToString(previous_->state_)));
    }
  }
#endif

#ifdef ENABLE_HEAP_PROTECTION
  if (FLAG_protect_heap) {
    if (state_ == EXTERNAL) {
      // We are leaving V8.
      ASSERT((previous_ != NULL) && (previous_->state_ != EXTERNAL));
      isolate_->heap()->Protect();
    } else if ((previous_ == NULL) || (previous_->state_ == EXTERNAL)) {
      // We are entering V8.
      isolate_->heap()->Unprotect();
    }
  }
#endif
}


VMState::~VMState() {
  ASSERT(isolate_ == Isolate::Current());
  if (disabled_) return;
  // Return to the previous state.
  isolate_->set_current_vm_state(previous_);

#ifdef ENABLE_LOGGING_AND_PROFILING
  if (FLAG_log_state_changes) {
    LOG(UncheckedStringEvent("Leaving", StateToString(state_)));
    if (previous_ != NULL) {
      LOG(UncheckedStringEvent("To", StateToString(previous_->state_)));
    }
  }
#endif  // ENABLE_LOGGING_AND_PROFILING

#ifdef ENABLE_HEAP_PROTECTION
  if (FLAG_protect_heap) {
    if (state_ == EXTERNAL) {
      // We are reentering V8.
      ASSERT((previous_ != NULL) && (previous_->state_ != EXTERNAL));
      isolate_->heap()->Unprotect();
    } else if ((previous_ == NULL) || (previous_->state_ == EXTERNAL)) {
      // We are leaving V8.
      isolate_->heap()->Protect();
    }
  }
#endif  // ENABLE_HEAP_PROTECTION
}
#endif  // ENABLE_VMSTATE_TRACKING

} }  // namespace v8::internal

#endif  // V8_VM_STATE_INL_H_