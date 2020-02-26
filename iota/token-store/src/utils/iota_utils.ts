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
