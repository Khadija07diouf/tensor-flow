# Copyright 2023 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Case functions for Control Flow Operations."""

import collections
import functools
from tensorflow.python.eager import context
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor
from tensorflow.python.ops import array_ops_stack
from tensorflow.python.ops import cond
from tensorflow.python.ops import control_flow_assert
from tensorflow.python.ops import math_ops
from tensorflow.python.platform import tf_logging as logging
from tensorflow.python.util import dispatch
from tensorflow.python.util.tf_export import tf_export


@tf_export("case", v1=[])
@dispatch.add_dispatch_support
def case_v2(pred_fn_pairs,
            default=None,
            exclusive=False,
            strict=False,
            name="case"):
  """Create a case operation.

  See also `tf.switch_case`.

  The `pred_fn_pairs` parameter is a list of pairs of size N.
  Each pair contains a boolean scalar tensor and a python callable that
  creates the tensors to be returned if the boolean evaluates to True.
  `default` is a callable generating a list of tensors. All the callables
  in `pred_fn_pairs` as well as `default` (if provided) should return the same
  number and types of tensors.

  If `exclusive==True`, all predicates are evaluated, and an exception is
  thrown if more than one of the predicates evaluates to `True`.
  If `exclusive==False`, execution stops at the first predicate which
  evaluates to True, and the tensors generated by the corresponding function
  are returned immediately. If none of the predicates evaluate to True, this
  operation returns the tensors generated by `default`.

  `tf.case` supports nested structures as implemented in
  `tf.nest`. All of the callables must return the same (possibly nested) value
  structure of lists, tuples, and/or named tuples. Singleton lists and tuples
  form the only exceptions to this: when returned by a callable, they are
  implicitly unpacked to single values. This behavior is disabled by passing
  `strict=True`.

  @compatibility(v2)
  `pred_fn_pairs` could be a dictionary in v1. However, tf.Tensor and
  tf.Variable are no longer hashable in v2, so cannot be used as a key for a
  dictionary.  Please use a list or a tuple instead.
  @end_compatibility


  **Example 1:**

  Pseudocode:

  ```
  if (x < y) return 17;
  else return 23;
  ```

  Expressions:

  ```python
  f1 = lambda: tf.constant(17)
  f2 = lambda: tf.constant(23)
  r = tf.case([(tf.less(x, y), f1)], default=f2)
  ```

  **Example 2:**

  Pseudocode:

  ```
  if (x < y && x > z) raise OpError("Only one predicate may evaluate to True");
  if (x < y) return 17;
  else if (x > z) return 23;
  else return -1;
  ```

  Expressions:

  ```python
  def f1(): return tf.constant(17)
  def f2(): return tf.constant(23)
  def f3(): return tf.constant(-1)
  r = tf.case([(tf.less(x, y), f1), (tf.greater(x, z), f2)],
           default=f3, exclusive=True)
  ```

  Args:
    pred_fn_pairs: List of pairs of a boolean scalar tensor and a callable which
      returns a list of tensors.
    default: Optional callable that returns a list of tensors.
    exclusive: True iff at most one predicate is allowed to evaluate to `True`.
    strict: A boolean that enables/disables 'strict' mode; see above.
    name: A name for this operation (optional).

  Returns:
    The tensors returned by the first pair whose predicate evaluated to True, or
    those returned by `default` if none does.

  Raises:
    TypeError: If `pred_fn_pairs` is not a list/tuple.
    TypeError: If `pred_fn_pairs` is a list but does not contain 2-tuples.
    TypeError: If `fns[i]` is not callable for any i, or `default` is not
               callable.
  """
  return _case_helper(
      cond.cond,
      pred_fn_pairs,
      default,
      exclusive,
      name,
      allow_python_preds=False,
      strict=strict)


@tf_export(v1=["case"])
@dispatch.add_dispatch_support
def case(pred_fn_pairs,
         default=None,
         exclusive=False,
         strict=False,
         name="case"):
  """Create a case operation.

  See also `tf.switch_case`.

  The `pred_fn_pairs` parameter is a dict or list of pairs of size N.
  Each pair contains a boolean scalar tensor and a python callable that
  creates the tensors to be returned if the boolean evaluates to True.
  `default` is a callable generating a list of tensors. All the callables
  in `pred_fn_pairs` as well as `default` (if provided) should return the same
  number and types of tensors.

  If `exclusive==True`, all predicates are evaluated, and an exception is
  thrown if more than one of the predicates evaluates to `True`.
  If `exclusive==False`, execution stops at the first predicate which
  evaluates to True, and the tensors generated by the corresponding function
  are returned immediately. If none of the predicates evaluate to True, this
  operation returns the tensors generated by `default`.

  `tf.case` supports nested structures as implemented in
  `tf.nest`. All of the callables must return the same (possibly nested) value
  structure of lists, tuples, and/or named tuples. Singleton lists and tuples
  form the only exceptions to this: when returned by a callable, they are
  implicitly unpacked to single values. This behavior is disabled by passing
  `strict=True`.

  If an unordered dictionary is used for `pred_fn_pairs`, the order of the
  conditional tests is not guaranteed. However, the order is guaranteed to be
  deterministic, so that variables created in conditional branches are created
  in fixed order across runs.

  @compatibility(eager)
  Unordered dictionaries are not supported in eager mode when `exclusive=False`.
  Use a list of tuples instead.
  @end_compatibility


  **Example 1:**

  Pseudocode:

  ```
  if (x < y) return 17;
  else return 23;
  ```

  Expressions:

  ```python
  f1 = lambda: tf.constant(17)
  f2 = lambda: tf.constant(23)
  r = tf.case([(tf.less(x, y), f1)], default=f2)
  ```

  **Example 2:**

  Pseudocode:

  ```
  if (x < y && x > z) raise OpError("Only one predicate may evaluate to True");
  if (x < y) return 17;
  else if (x > z) return 23;
  else return -1;
  ```

  Expressions:

  ```python
  def f1(): return tf.constant(17)
  def f2(): return tf.constant(23)
  def f3(): return tf.constant(-1)
  r = tf.case({tf.less(x, y): f1, tf.greater(x, z): f2},
           default=f3, exclusive=True)
  ```

  Args:
    pred_fn_pairs: Dict or list of pairs of a boolean scalar tensor and a
      callable which returns a list of tensors.
    default: Optional callable that returns a list of tensors.
    exclusive: True iff at most one predicate is allowed to evaluate to `True`.
    strict: A boolean that enables/disables 'strict' mode; see above.
    name: A name for this operation (optional).

  Returns:
    The tensors returned by the first pair whose predicate evaluated to True, or
    those returned by `default` if none does.

  Raises:
    TypeError: If `pred_fn_pairs` is not a list/dictionary.
    TypeError: If `pred_fn_pairs` is a list but does not contain 2-tuples.
    TypeError: If `fns[i]` is not callable for any i, or `default` is not
               callable.
  """
  return _case_helper(
      cond.cond,
      pred_fn_pairs,
      default,
      exclusive,
      name,
      allow_python_preds=False,
      strict=strict)


def _assert_at_most_n_true(predicates, n, msg):
  """Returns an Assert op that checks that at most n predicates are True.

  Args:
    predicates: list of bool scalar tensors.
    n: maximum number of true predicates allowed.
    msg: Error message.
  """
  preds_c = array_ops_stack.stack(predicates, name="preds_c")
  num_true_conditions = math_ops.reduce_sum(
      math_ops.cast(preds_c, dtypes.int32), name="num_true_conds")
  condition = math_ops.less_equal(num_true_conditions,
                                  constant_op.constant(n, name="n_true_conds"))
  preds_names = ", ".join(getattr(p, "name", "?") for p in predicates)
  error_msg = [
      "%s: more than %d conditions (%s) evaluated as True:" %
      (msg, n, preds_names), preds_c
  ]
  return control_flow_assert.Assert(
      condition, data=error_msg, summarize=len(predicates))


def _case_create_default_action(predicates, actions):
  """Creates default action for a list of actions and their predicates.

  It uses the input actions to select an arbitrary as default and makes sure
  that corresponding predicates have valid values.

  Args:
    predicates: a list of bool scalar tensors
    actions: a list of callable objects which return tensors.

  Returns:
    a callable
  """
  k = len(predicates) - 1  # could pick any
  predicate, action = predicates[k], actions[k]
  other_predicates, other_actions = predicates[:k], actions[:k]

  def default_action():
    others_msg = ("Implementation error: "
                  "selected default action #%d was called, but some of other "
                  "predicates are True: " % k)
    default_msg = ("Input error: "
                   "None of conditions evaluated as True:",
                   array_ops_stack.stack(predicates, name="preds_c"))
    with ops.control_dependencies([
        _assert_at_most_n_true(  # pylint: disable=protected-access
            other_predicates, n=0, msg=others_msg),
        control_flow_assert.Assert(predicate, data=default_msg)
    ]):
      return action()

  return default_action, other_predicates, other_actions


def _case_helper(cond_fn,
                 pred_fn_pairs,
                 default,
                 exclusive,
                 name,
                 allow_python_preds=False,
                 **cond_kwargs):
  """Implementation of case that allows for different cond functions.

  Args:
    cond_fn: method that has signature and semantics of `cond` above.
    pred_fn_pairs: Dict or list of pairs of a boolean scalar tensor, and a
      callable which returns a list of tensors.
    default: Optional callable that returns a list of tensors.
    exclusive: True iff at most one predicate is allowed to evaluate to `True`.
    name: A name for this operation (optional).
    allow_python_preds: if true, pred_fn_pairs may contain Python bools in
      addition to boolean Tensors
    **cond_kwargs: keyword arguments that will be passed to `cond_fn`.

  Returns:
    The tensors returned by the first pair whose predicate evaluated to True, or
    those returned by `default` if none does.

  Raises:
    TypeError: If `pred_fn_pairs` is not a list/dictionary.
    TypeError: If `pred_fn_pairs` is a list but does not contain 2-tuples.
    TypeError: If `fns[i]` is not callable for any i, or `default` is not
               callable.
  """
  predicates, actions = _case_verify_and_canonicalize_args(
      pred_fn_pairs, exclusive, name, allow_python_preds)
  with ops.name_scope(name, "case", [predicates]):
    if default is None:
      default, predicates, actions = _case_create_default_action(
          predicates, actions)
    fn = default
    # To eval conditions in direct order we create nested conditions in reverse:
    #   cond_fn(c[0], true_fn=.., false_fn=cond_fn(c[1], ...))
    for predicate, action in reversed(list(zip(predicates, actions))):
      fn = functools.partial(
          cond_fn, predicate, true_fn=action, false_fn=fn, **cond_kwargs)
    if exclusive:
      with ops.control_dependencies([
          _assert_at_most_n_true(  # pylint: disable=protected-access
              predicates, n=1, msg="Input error: exclusive=True")
      ]):
        return fn()
    else:
      return fn()


def _case_verify_and_canonicalize_args(pred_fn_pairs, exclusive, name,
                                       allow_python_preds):
  """Verifies input arguments for the case function.

  Args:
    pred_fn_pairs: Dict or list of pairs of a boolean scalar tensor, and a
      callable which returns a list of tensors.
    exclusive: True iff at most one predicate is allowed to evaluate to `True`.
    name: A name for the case operation.
    allow_python_preds: if true, pred_fn_pairs may contain Python bools in
      addition to boolean Tensors

  Raises:
    TypeError: If `pred_fn_pairs` is not a list/dictionary.
    TypeError: If `pred_fn_pairs` is a list but does not contain 2-tuples.
    TypeError: If `fns[i]` is not callable for any i, or `default` is not
               callable.

  Returns:
    a tuple <list of scalar bool tensors, list of callables>.
  """
  if not isinstance(pred_fn_pairs, (list, tuple, dict)):
    raise TypeError("'pred_fn_pairs' must be a list, tuple, or dict. "
                    f"Received: {type(pred_fn_pairs)}")

  if isinstance(pred_fn_pairs, collections.OrderedDict):
    pred_fn_pairs = pred_fn_pairs.items()
  elif isinstance(pred_fn_pairs, dict):
    if context.executing_eagerly():
      # No name to sort on in eager mode. Use dictionary traversal order,
      # which is nondeterministic in versions of Python < 3.6
      if not exclusive:
        raise ValueError("Unordered dictionaries are not supported for the "
                         "'pred_fn_pairs' argument when `exclusive=False` and "
                         "eager mode is enabled.")
      pred_fn_pairs = list(pred_fn_pairs.items())
    else:
      pred_fn_pairs = sorted(
          pred_fn_pairs.items(), key=lambda item: item[0].name)
      if not exclusive:
        logging.warn(
            "%s: An unordered dictionary of predicate/fn pairs was "
            "provided, but exclusive=False. The order of conditional "
            "tests is deterministic but not guaranteed.", name)
  for pred_fn_pair in pred_fn_pairs:
    if not isinstance(pred_fn_pair, tuple) or len(pred_fn_pair) != 2:
      raise TypeError("Each entry in 'pred_fn_pairs' must be a 2-tuple. "
                      f"Received {pred_fn_pair}.")
    pred, fn = pred_fn_pair

    if isinstance(pred, tensor.Tensor):
      if pred.dtype != dtypes.bool:
        raise TypeError("pred must be Tensor of type bool: %s" % pred.name)
    elif not allow_python_preds:
      raise TypeError("pred must be a Tensor, got: %s" % pred)
    elif not isinstance(pred, bool):
      raise TypeError("pred must be a Tensor or bool, got: %s" % pred)

    if not callable(fn):
      raise TypeError("fn for pred %s must be callable." % pred.name)

  predicates, actions = zip(*pred_fn_pairs)
  return predicates, actions
