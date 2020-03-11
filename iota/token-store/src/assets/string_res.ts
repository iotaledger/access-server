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

export class StringRes {
  public static readonly MISSING_ADDRESS = 'Missing address.';
  public static readonly MISSING_AMOUNT = 'Missing amount.';
  public static readonly AMOUNT_MUST_BE_NUMBER = 'Amount must be number.';
  public static readonly MISSING_SENDER = 'Missing sender.';
  public static readonly MISSING_RECEIVER = 'Missing receiver.';
  public static readonly CREATE_ACCOUNT_SUCCESS
    = 'Account successfully created.';
  public static readonly CREATE_ACCOUNT_FAILURE
    = 'Unable to create account.';
  public static readonly FUND_SUCCESS = 'Successfully funded.';
  public static readonly FUND_FAILURE = 'Unable to fund.'
  public static readonly SEND_SUCCESS = 'Successfully sent.'
  public static readonly SEND_FAILURE = 'Unable to send.'
  public static readonly BALANCE_REQUEST_SUCCESS
    = 'Balance request succeeded.';
  public static readonly BALANCE_REQUEST_FAILURE
    = 'Balance request failed.';
}
