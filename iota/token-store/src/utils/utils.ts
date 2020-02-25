import BigNumber from 'bignumber.js';

/**
 * Make default response for every REST response
 * 
 * @param {boolean} error   true if error happened, false otherwise
 * @param {string}  message Response message
 * @param {object}  data    (Optional) data object
 */
export const defaultResponse = (error: boolean, message: string, data?: object) => {
    return {
        error: error,
        message: message,
        data: data
    };
};

/**
 * Convert provided amount to not use decimal points (if any)
 * 
 * @param amount Amount to convert
 * @param decimals Number of decimals
 */
export const toTokenWithoutDecimals = (amount: number, decimals: number): string => {
    return new BigNumber(String(amount))
        .multipliedBy(new BigNumber(10).exponentiatedBy(decimals))
        .toString();
}

/**
 * Convert provided amount to use decimal points (if any)
 * 
 * @param amount Amount to convert
 * @param decimals Number of decimals
 */
export const toTokenWithDecimals = (amount: number, decimals: number): string => {
    return new BigNumber(String(amount))
        .dividedBy(new BigNumber(10).exponentiatedBy(decimals))
        .toString();
}

export const mandatoryEnvMissing = (item: string): string => {
    return `${item} must be provided as environment variable. Exiting...`;
}

export const parseJson = (json: string) => {
    if (typeof json !== 'string')
        return;
    
    let start = -1;
    let end = -1;
    let balance = 0;
    [...json].forEach((element, index) => {
        if (element === '{') {
            if (start == -1) {
                start = index;
            }
            balance += 1;
        }
        else if (element === '}') {
            end = index;
            balance -= 1; 
        }
    });

    if (balance != 0 || start == -1 || end == -1)
        return;
    
    try {
        return JSON.parse(json.substring(start, end + 1));
    } catch(err) {
        console.log(err);
        return;
    }
}
