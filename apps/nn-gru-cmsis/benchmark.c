/*
 * Copyright (c) 2018-2020, University of Southampton.
 * Copyright (C) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   - Neither the name of Arm LIMITED nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ----------------------------------------------------------------------------
 * Example adapted from CMSIS_5/CMSIS/NN/Examples/<...>/arm_nnexamples_gru.cpp
 *
 *
 * Documentation available at
 * https://arm-software.github.io/CMSIS_5/NN/html/group__GRUExample.html
 * ----------------------------------------------------------------------------
 */

#include "arm_math.h"
#include "arm_nnexamples_gru_test_data.h"
#include "arm_nnfunctions.h"
#include "lib/iclib/ic.h"
#include "lib/support/support.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define DIM_HISTORY 32
#define DIM_INPUT 32
#define DIM_VEC 64

static q7_t update_gate_weights[DIM_VEC * DIM_HISTORY] = UPDATE_GATE_WEIGHT_X4;
static q7_t reset_gate_weights[DIM_VEC * DIM_HISTORY] = RESET_GATE_WEIGHT_X4;
static q7_t hidden_state_weights[DIM_VEC * DIM_HISTORY] =
    HIDDEN_STATE_WEIGHT_X4;
static q7_t update_gate_bias[DIM_HISTORY] = UPDATE_GATE_BIAS;
static q7_t reset_gate_bias[DIM_HISTORY] = RESET_GATE_BIAS;
static q7_t hidden_state_bias[DIM_HISTORY] = HIDDEN_STATE_BIAS;

static q15_t test_input1[DIM_INPUT] = INPUT_DATA1;
static q15_t test_history[DIM_HISTORY] = HISTORY_DATA;

q15_t scratch_buffer[DIM_HISTORY * 4 + DIM_INPUT];

void gru_example(q15_t *scratch_input, uint16_t input_size,
                 uint16_t history_size, q7_t *weights_update,
                 q7_t *weights_reset, q7_t *weights_hidden_state,
                 q7_t *bias_update, q7_t *bias_reset, q7_t *bias_hidden_state) {
  q15_t *reset = scratch_input;
  q15_t *input = scratch_input + history_size;
  q15_t *history = scratch_input + history_size + input_size;
  q15_t *update = scratch_input + 2 * history_size + input_size;
  q15_t *hidden_state = scratch_input + 3 * history_size + input_size;

  // reset gate calculation
  // the range of the output can be adjusted with bias_shift and output_shift
  arm_fully_connected_mat_q7_vec_q15_opt(
      input, weights_reset, input_size + history_size, history_size, 0, 15,
      bias_reset, reset, NULL);
  // sigmoid function, the size of the integer bit-width should be consistent
  // with out_shift
  arm_nn_activations_direct_q15(reset, history_size, 0, ARM_SIGMOID);
  arm_mult_q15(history, reset, reset, history_size);

  // update gate calculation
  // the range of the output can be adjusted with bias_shift and output_shift
  arm_fully_connected_mat_q7_vec_q15_opt(
      input, weights_update, input_size + history_size, history_size, 0, 15,
      bias_update, update, NULL);
  // sigmoid function, the size of the integer bit-width should be consistent
  // with out_shift
  arm_nn_activations_direct_q15(update, history_size, 0, ARM_SIGMOID);

  // hidden state calculation
  arm_fully_connected_mat_q7_vec_q15_opt(
      reset, weights_hidden_state, input_size + history_size, history_size, 0,
      15, bias_hidden_state, hidden_state, NULL);

  // tanh function, the size of the integer bit-width should be consistent with
  // out_shift
  arm_nn_activations_direct_q15(hidden_state, history_size, 0, ARM_TANH);
  arm_mult_q15(update, hidden_state, hidden_state, history_size);

  // we calculate z - 1 here
  // so final addition becomes substraction
  arm_offset_q15(update, 0x8000, update, history_size);
  // multiply history
  arm_mult_q15(history, update, update, history_size);
  // calculate history_out
  arm_sub_q15(hidden_state, update, history, history_size);

  return;
}

void benchmark_init() {
  // copy over the input data
  const int input_size = DIM_INPUT;
  const int history_size = DIM_HISTORY;

  memset(scratch_buffer, 0, sizeof(scratch_buffer));
  arm_copy_q15(test_input1, scratch_buffer + history_size, input_size);
  arm_copy_q15(test_history, scratch_buffer + history_size + input_size,
               history_size);
}

void benchmark_run() {
  const int input_size = DIM_INPUT;
  const int history_size = DIM_HISTORY;

  gru_example(scratch_buffer, input_size, history_size, update_gate_weights,
              reset_gate_weights, hidden_state_weights, update_gate_bias,
              reset_gate_bias, hidden_state_bias);
}

bool benchmark_verify() {
  const uint32_t golden = 965;
  int result = 0;
  for (int i = 0; i < sizeof(scratch_buffer) / sizeof(q15_t); ++i) {
    result += scratch_buffer[i];
  }

  return result != golden;
}

void main() {
  while (1) {
    benchmark_init();
    benchmark_run();
    benchmark_verify();
  }
}
