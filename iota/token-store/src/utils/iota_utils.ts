import * as Iota from '@iota/core';

export const confirmTransaction = async (iota: Iota.API, transaction: string) => {

    return new Promise((resolve, reject) => {
        console.log(`Started confirming transaction with hash: ${transaction}`);

        let tails = [transaction];

        let seconds = 0;
    
        const anonymousMainFunction = autoConfirm.bind(null, tails);
    
        // Run the autoConfirm() function every 10 seconds to check for confirmations
        let interval = setInterval(anonymousMainFunction, 10000);
    
        // Set a timer to measure how long it takes for the bundle to be confirmed
        let timer = setInterval(() => seconds++, 1000);

        // Create a function that checks if a bundle is attached to a valid subtangle
        // and not older than the last 6 milestone transactions
        // If it is, promote it, if not, try to reattach it.
        function autoPromoteReattach (tail: string) {
            iota.isPromotable(tail)
                .then(promote => promote
                ? iota.promoteTransaction(tail, 3, 14)
                    .then(()=> {
                        console.log(`Promoted transaction hash: ${tail}`);
                    })
                : iota.replayBundle(tail, 3, 14)
                    .then(([reattachedTail]) => {
                        const newTailHash = reattachedTail.hash

                        console.log(`Reattached transaction hash: ${tail}`);

                        // Keep track of all reattached tail transaction hashes to check for confirmation
                        tails.push(newTailHash);
                    })
                )
                .catch(err => {
                    console.log(err);
                    reject(err);
                });
        }

        // Create the autoConfirm function
        // that checks if one of the bundles have been confirmed
        // If not, start to promote and reattach the tail transactions until at least one of them is confirmed
        function autoConfirm(tails: string[]){
            console.log(tails);
            iota.getLatestInclusion(tails)
                .then(states => {
                    // Check that none of the transactions have been confirmed
                    if (states.indexOf(true) === -1) {
                        // Get latest tail hash
                        const tail = tails[tails.length - 1] 
                        autoPromoteReattach(tail);
                    } else {
                        console.log(JSON.stringify(states,null, 1));
                        clearInterval(interval);
                        clearInterval(timer);
                        var minutes = (seconds / 60).toFixed(2);
                        var confirmedTail = tails[states.indexOf(true)];
                        console.log(`Confirmed transaction hash in ${minutes} minutes: ${confirmedTail}`);
                        resolve();
                    }
                }).catch(err => {
                    console.log(err);
                    reject(err);
                }
            );
        }
    });
}

export default confirmTransaction;
