/**
 * Default response for every REST call
 */
export interface DefaultResponse {
    error: boolean,
    message: string,
    data?: object
}

/**
 * Make default response for every REST call
 * 
 * @param error true if error happened, false otherwise
 * @param message Response message
 * @param data data object
 */
export const defaultResponse = (error: boolean, message: string, data?: object): DefaultResponse => {
    return {
        error: error,
        message: message,
        data: data
    };
};

/**
 * Extracts JSON from string and converts it to object.
 * Function searches inside string for balanced open and close brackets ("{", "}", "[", "]") to
 * extract valid JSON, because string may contain prefix or suffix characters which are not part of
 * the valid JSON.
 * 
 * @param json JSON string
 */
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
    } catch (err) {
        return;
    }
}
