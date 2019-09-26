/*
 *  This file is part of the DAC distribution (https://github.com/xainag/frost)
 *  Copyright (c) 2019 XAIN AG.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

package com.de.xain.emdac.api.model.policy;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class PolicyHelper {

    private static final String HASH_FUNCTION = "sha-256";

    /**
     * Calculated the hash of the given policy
     *
     * @param policy policy object for which the hash is calculated
     * @return the hash in {@link byte[]} representation
     */
    private static byte[] getHash(Policy policy, String hashFunction) {
        MessageDigest digest = null;
        try {
            digest = MessageDigest.getInstance(hashFunction);
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        digest.reset();
        return digest.digest(policy.getPolicyObject().toString().getBytes());
    }
}
