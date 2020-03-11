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

import * as Iota from '@iota/core';
import logger from './logger';

/**
 * Confirms transaction on tangle.
 * 
 * @param iota Reference to Iota API object.
 * @param tx Transaction to be made.
 */
export const confirmTransaction = async (
  iota: Iota.API,
  tx: string
): Promise<void> => {
  return new Promise((resolve, reject) => {
    logger.info(`Started confirming transaction with hash: ${tx}`);

    let tails = [tx];

    let seconds = 0;

    const anonymousMainFunction = autoConfirm.bind(null, tails);

    // Run the autoConfirm() function every 10 seconds to check for
    // confirmations
    let interval = setInterval(anonymousMainFunction, 10000);

    // Set a timer to measure how long it takes for the bundle to be confirmed
    let timer = setInterval(() => seconds++, 1000);

    /**
     * Create a function that checks if a bundle is attached to a valid
     * subtangle and not older than the last 6 milestone transactions.
     * 
     * If it is, promote it, if not, try to reattach it.
     * @param transaction Transaction.
     */
    function autoPromoteReattach(transaction: string) {
      iota.isPromotable(transaction)
        .then(promote => promote
          ? iota.promoteTransaction(transaction, 3, 14)
            .then(() => {
              logger.info(`Promoted transaction hash: ${transaction}`);
            })
          : iota.replayBundle(transaction, 3, 14)
            .then(([reattachedTail]) => {
              const newTailHash = reattachedTail.hash

              logger.info(`Reattached transaction hash: ${transaction}`);

              // Keep track of all reattached tail transaction hashes to check
              // for confirmation
              tails.push(newTailHash);
            })
        )
        .catch(err => {
          reject(err);
        });
    }

    /**
     * Create the autoConfirm function
     * that checks if one of the bundles have been confirmed
     * If not, start to promote and reattach the tail transactions until at
     * least one of them is confirmed.
     * 
     * @param transactions Transactions.
     */
    function autoConfirm(transactions: string[]) {
      iota.getLatestInclusion(transactions)
        .then(states => {
          // Check that none of the transactions have been confirmed
          if (states.indexOf(true) === -1) {
            // Get latest tail hash
            const tail = transactions[transactions.length - 1]
            autoPromoteReattach(tail);
          } else {
            clearInterval(interval);
            clearInterval(timer);
            var minutes = (seconds / 60).toFixed(2);
            var confirmedTail = transactions[states.indexOf(true)];

            logger.info(
              `Confirmed transaction hash in ${minutes} `
              + `minutes: ${confirmedTail}`
            );

            resolve();
          }
        }).catch(err => {
          reject(err);
        });
    }
  });
}
