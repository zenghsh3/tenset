/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \file auto_scheduler_subgraph_sequencing.h
 * \brief add index for subgraph
 */

#include "auto_scheduler_subgraph_sequencing.h"

#include <tvm/relay/attrs/transform.h>
#include <tvm/relay/expr_functor.h>
#include <tvm/relay/op_attr_types.h>
#include <tvm/relay/transform.h>

#include <functional>



namespace tvm {
namespace relay {

int AutoSchedulerSubGraphSequence::index = 0;

Expr AutoSchedulerSubGraphSequence::VisitExpr_(const CallNode* n) {
  if (auto* func = n->op.as<FunctionNode>()) {
    AutoSchedulerSubGraphSequence::index += 1;
    auto new_func = Function(func->params, func->body, func->ret_type, func->type_params, func->attrs, func->span, AutoSchedulerSubGraphSequence::index);
    auto new_n = Call(new_func, n->args, n->attrs, n->type_args, n->span);
    return  ExprMutator::VisitExpr_(new_n.get());
  } 
  return ExprMutator::VisitExpr_(n);
}

Expr AutoSchedulerSubGraphSequencing(const Expr& expr) {
  auto new_expr = AutoSchedulerSubGraphSequence().Mutate(expr);
  AutoSchedulerSubGraphSequence::index = 0;
  return new_expr;
}

namespace transform {

Pass AutoSchedulerSubGraphSequencing() {
  runtime::TypedPackedFunc<Function(Function, IRModule, PassContext)> pass_func =
      [=](Function f, IRModule m, PassContext pc) {
        return Downcast<Function>(relay::AutoSchedulerSubGraphSequencing(f));
      };
  return CreateFunctionPass(pass_func, 3, "AutoSchedulerSubGraphSequencing", {"InferType"});
}

TVM_REGISTER_GLOBAL("relay._transform.AutoSchedulerSubGraphSequencing")
    .set_body_typed(AutoSchedulerSubGraphSequencing);

}  // namespace transform

}  // namespace relay
}  // namespace tvm
