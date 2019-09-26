/*
 * This file is part of the DAC distribution (https://github.com/xainag/frost)
 * Copyright (c) 2019 XAIN AG.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Defautl response for every REST call
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
        console.log(err);
        return;
    }
}
