/*
 * This file is part of the IOTA Access distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Stiftung
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/***************************************************************************
 * Project:     IOTA Access
 *
 * Module:      Policy validation tool
 *
 * Info:        This module is created in order to validate or
 *              discard policy, also it has functionality to
 *              perform policy optimisation.
 *
 * File:        parser.c
 *
 * Designed-by: Strahinja Golic
 *
 * History:     25.03.2020. - Initial version
 *****************************************************************************/

/***************************************************************************
 * INCLUDES
 ****************************************************************************/
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************
 * FUNCTION IMPLEMENTATIONS
 ****************************************************************************/

/*
 *  Function: Parser_next_key_sibling_idx
 *  Description: Get index of next key sibling token
 *  Parameters: _tokens - tokens array
 *              cur_idx - current sibling index
 *              max_idx - maximum index
 *  Returns: ID of the next sibling token
 */
int parser_next_key_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx) {
  int next_idx = -1;

  // only key tokens have size 1
  if (_tokens[cur_idx].size == 1) {
    for (int i = cur_idx + 1; i < max_idx; i++) {
      if (_tokens[i].start >= _tokens[cur_idx + 1].end) {
        next_idx = i;
        break;
      }
    }
  }
  return next_idx;
}

/*
 *  Function: Parser_next_object_sibling_idx
 *  Description: Get index of next object sibling token
 *  Parameters: _tokens - tokens array
 *              cur_idx - current sibling index
 *              max_idx - maximum index
 *  Returns: ID of the next sibling token
 */
int parser_next_object_sibling_idx(jsmntok_t* _tokens, int cur_idx, int max_idx) {
  int next_idx = -1;

  if (_tokens[cur_idx].type == JSMN_OBJECT) {
    for (int i = cur_idx + 1; i < max_idx; i++) {
      if (_tokens[i].start >= _tokens[cur_idx].end) {
        next_idx = i;
        break;
      }
    }
  }
  return next_idx;
}

/*
 *  Function: Parser_end_of_current_idx
 *  Description: Get last index in current object
 *  Parameters: _tokens - tokens array
 *              cur_idx - current sibling index
 *              max_idx - maximum index
 *  Returns: End position of the token
 */
int parser_end_of_current_idx(jsmntok_t* _tokens, int cur_idx, int max_idx) {
  int end_of_current = -1;

  for (int i = cur_idx + 1; i < max_idx; i++) {
    if (_tokens[i].end <= _tokens[cur_idx + 1].end) {
      end_of_current = i;
    }
    if (_tokens[i].start >= _tokens[cur_idx + 1].end) {
      break;
    }
  }
  return end_of_current;
}

/*
 *  Function: Parser_object_parent_idx
 *  Description: Get index of the object's parent
 *  Parameters: _tokens - tokens array
 *              cur_idx - current object index
 *  Returns: Position of the parent token
 */
int parser_object_parent_idx(jsmntok_t* _tokens, int cur_idx) {
  int parent = -1;

  if (_tokens[cur_idx].type == JSMN_OBJECT) {
    for (int i = cur_idx - 1; i >= 0; i--) {
      if (_tokens[i].type == JSMN_ARRAY) {
        // if array is found before object, it must be it's parent
        parent = i;
        break;
      } else if (_tokens[i].type == JSMN_OBJECT) {
        // object can be another object's parent, just check if they are not siblings
        int siblings = parser_next_object_sibling_idx(_tokens, i, cur_idx + 1);
        while (siblings >= 0) {
          if (siblings == cur_idx) {
            // objects are siblings, continue search
            break;
          } else {
            siblings = parser_next_object_sibling_idx(_tokens, siblings, cur_idx + 1);
          }
        }

        if (siblings == -1)  // did not match cur_idx
        {
          parent = i;
          break;
        }
      }
    }
  }

  return parent;
}

/*
 *  Function: Parser_get_op
 *  Description: Get operation from string
 *  Parameters: str - string
 *              len - string length
 *  Returns: operation
 */
parser_operations_e parser_get_op(char* str, int len) {
  if (str == NULL || len == 0) {
    return OP_UNKNOWN;
  }

  if (strncmp(str, "eq", len) == 0) {
    return OP_EQ;
  } else if (strncmp(str, "leq", len) == 0) {
    return OP_LEQ;
  } else if (strncmp(str, "geq", len) == 0) {
    return OP_GEQ;
  } else if (strncmp(str, "lte", len) == 0) {
    return OP_LTE;
  } else if (strncmp(str, "gte", len) == 0) {
    return OP_GTE;
  } else if (strncmp(str, "and", len) == 0) {
    return OP_AND;
  } else if (strncmp(str, "or", len) == 0) {
    return OP_OR;
  } else {
    return OP_UNKNOWN;
  }
}
