/* Written by : Eric Tan
 *
 * File containing utility functions and constants for implamentation of SGEMV.
 */

#pragma once

#include <vector>


/* CONSTANTS
 */
constexpr int OPEN_FILE_ERROR = -1;
constexpr int CMD_INPUT_ERROR = -2;
constexpr int DATA_MSG   = 1;
constexpr int PROMPT_MSG = 2;

/* FUNCTIONS
 */
int block_low(int id, int p, int n);
int block_high(int id, int p, int n);
int block_size(int id, int p, int n);
void make_mixed_xfer_array(int p, int n, std::vector<int> &cnt, std::vector<int> &disp);
void make_uniforn_xfer_array(int id, int p, int n, std::vector<int> &cnt, std::vector<int> &disp);

