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
 * \file tvm/relay/function.h
 * \brief Relay Function.
 */
#ifndef TVM_RELAY_FUNCTION_H_
#define TVM_RELAY_FUNCTION_H_

#include <tvm/ir/function.h>
#include <tvm/relay/expr.h>

#include <string>

namespace tvm {
namespace relay {

/*!
 * \brief Relay Function container
 * \sa Function
 */
class FunctionNode : public BaseFuncNode {
 public:
  /*! \brief The index of the subgraph on Module */
  int index;

  /*! \brief Function parameters */
  tvm::Array<Var> params;
  /*!
   * \brief
   * The expression which represents the computation of the function,
   * the expression may reference the parameters, and the type of it
   * or sub-expressions may reference the type variables.
   */
  Expr body;
  /*! \brief User annotated return type of the function. */
  Type ret_type;
  /*!
   * \brief Type parameters of the function.
   *  Enables the function to vary its type based on these.
   *  This corresponds to template paramaters in c++'s terminology.
   *
   * \note This can be usually empty for non-polymorphic functions.
   */
  tvm::Array<TypeVar> type_params;

  void VisitAttrs(tvm::AttrVisitor* v) {
    v->Visit("params", &params);
    v->Visit("body", &body);
    v->Visit("ret_type", &ret_type);
    v->Visit("type_params", &type_params);
    v->Visit("attrs", &attrs);
    v->Visit("span", &span);
    v->Visit("index", &index);
    v->Visit("_checked_type_", &checked_type_);
  }

  bool SEqualReduce(const FunctionNode* other, SEqualReducer equal) const {
    // Important to make def equal first.
    equal->MarkGraphNode();
    return equal.DefEqual(params, other->params) &&
           equal.DefEqual(type_params, other->type_params) && equal(ret_type, other->ret_type) &&
           equal(attrs, other->attrs) && equal(body, other->body);
  }

  void SHashReduce(SHashReducer hash_reduce) const {
    hash_reduce->MarkGraphNode();
    hash_reduce.DefHash(params);
    hash_reduce.DefHash(type_params);
    hash_reduce(ret_type);
    hash_reduce(attrs);
    hash_reduce(body);
  }

  /*!
   * \brief Return the derived function annotation of this expression.
   *
   * \return The function type annotation.
   * \note The function type annotation can contain IncompleteType.
   */
  TVM_DLL FuncType func_type_annotation() const;

  static constexpr const char* _type_key = "relay.Function";
  TVM_DECLARE_FINAL_OBJECT_INFO(FunctionNode, BaseFuncNode);
};

/*!
 * \brief Managed reference to FunctionNode.
 * \sa FunctionNode
 */
class Function : public BaseFunc {
 public:
  /*!
   * \brief Constructor
   * \param params The parameters of the function.
   * \param body The body of the function.
   * \param ret_type The return type of the function.
   * \param ty_params The type parameters.
   * \param attrs Additional function attributes.
   * \param span The span of the function.
   */
  TVM_DLL Function(tvm::Array<Var> params, Expr body, Type ret_type, tvm::Array<TypeVar> ty_params,
                   tvm::DictAttrs attrs = NullValue<DictAttrs>(), Span span = Span(), int index = -1);

  TVM_DEFINE_OBJECT_REF_METHODS(Function, BaseFunc, FunctionNode);
  TVM_DEFINE_OBJECT_REF_COW_METHOD(FunctionNode);
};

/*!
 * \brief namespace of the attributes that can be attached to a relay::Function.
 */
namespace attr {
/*! \brief Mark the function as a primitive function. */
constexpr const char* kPrimitive = "Primitive";
/*!
 * \brief Indicate the compiler that should be used for builing this function.
 * When this is unset or set to "default", the default compilation pipeline will be used.
 */
constexpr const char* kCompiler = "Compiler";
/*! \brief Indicate if the function is a closure. */
constexpr const char* kClosure = "Closure";
/*! \brief Store a Var to parameter/Constant mapping on a Function. */
constexpr const char* kParams = "__params__";
/*! \brief Store the unique external symbol for external compilers. */
constexpr const char* kExternalSymbol = "ExternalSymbol";
/*! \brief Mark if the function should be avoided being optimized. */
constexpr const char* kSkipOptimization = "SkipOptimization";
/*! \brief Treat the function as a composite operator. */
constexpr const char* kComposite = "Composite";
/*! \brief Mark the function to be inlined. */
constexpr const char* kInline = "Inline";
/*! \brief Indicate the function was created by the Pattern Partitioning Pass. */
constexpr const char* kPartitionedFromPattern = "PartitionedFromPattern";
}  // namespace attr

}  // namespace relay
}  // namespace tvm
#endif  // TVM_RELAY_FUNCTION_H_
