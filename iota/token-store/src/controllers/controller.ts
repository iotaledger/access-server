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

/**
 * Controller for managing users and tokens.
 */
export default interface Controller {
  /**
   * Creates new account.
   * 
   * @returns Promise containing ID of newly created account.
   */
  createAccount(): Promise<string>

  /**
   * Gets balance of user with provided ID.
   * 
   * @param userId Id of user.
   * 
   * @returns Promise containing balance.
   */
  getBalance(userId: string): Promise<string>

  /**
   * Sends specified amount of tokens from one account to another.
   * 
   * @param from ID of user from which tokens are sent.
   * @param to ID of user to which tokens are sent.
   * @param amount Amount of tokens to be sent.
   */
  send(from: string, to: string, amount: number): Promise<void>

  /**
   * Funds user from master user's account with specified ID with given
   * amount.
   * 
   * @param receiver ID of user that receives tokens.
   * @param amount Amount of tokens to be transferred.
   */
  fund(receiver: string, amount: number): Promise<void>
}
