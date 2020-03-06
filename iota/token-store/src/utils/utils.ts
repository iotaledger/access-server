/*
 * This file is part of the Frost distribution
 * (https://github.com/iotaledger/frost)
 * 
 * Copyright (c) 2020 IOTA.
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

import BigNumber from 'bignumber.js';

/**
 * Type of default REST response.
 */
export type DefaultResponse = {
  error: boolean
  message: string,
  data?: object
}

/**
 * Make default response for every REST response.
 * 
 * @param error True if error happened, false otherwise.
 * @param message Response message.
 * @param data Data object.
 * 
 * @returns Default REST response.
 */
export const defaultResponse = (
  error: boolean,
  message: string,
  data?: object
): DefaultResponse => {
  return {
    error: error,
    message: message,
    data: data
  };
};

/**
 * Convert provided amount to not use decimal points (if any).
 * 
 * @param amount Amount to convert.
 * @param decimals Number of decimals.
 * 
 * @returns Amount of tokens without decimal point.
 */
export const toTokenWithoutDecimals = (
  amount: number,
  decimals: number
): string => {
  return new BigNumber(String(amount))
    .multipliedBy(new BigNumber(10).exponentiatedBy(decimals))
    .toString();
}

/**
 * Convert provided amount to use decimal points (if any).
 * 
 * @param amount Amount to convert.
 * @param decimals Number of decimals.
 * 
 * @returns Amount of tokens with decimal point for easier readability.
 */
export const toTokenWithDecimals = (
  amount: number,
  decimals: number
): string => {
  return new BigNumber(String(amount))
    .dividedBy(new BigNumber(10).exponentiatedBy(decimals))
    .toString();
}

/**
 * Creates message explaining which environment variable must be provided.
 * 
 * @param item Name of environment variable that is missing.
 */
export const mandatoryEnvMissing = (item: string): string => {
  return `${item} must be provided as environment variable. Exiting...`;
}
