/*
 *  This file is part of the Frost distribution
 *  (https://github.com/xainag/frost)
 *
 *  Copyright (c) 2019 XAIN AG.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

package com.de.xain.emdac.utils;

import com.de.xain.emdac.api.model.policy.Policy;
import com.de.xain.emdac.api.model.policy.PolicyObject;

import java.io.InputStream;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * Utility class for providing different conversions
 */
public class ConversionUtils {

    private static final String HASH_FUNCTION = "sha-256";
    private static final String TAG = ConversionUtils.class.getSimpleName();

    /**
     * Calculated the hash of the given object
     *
     * @param policy policy object for which the hash is calculated
     * @return the hash in {@link byte[]} representation
     */
    private static byte[] getHash(PolicyObject policy) {
        MessageDigest digest = null;
        try {
            digest = MessageDigest.getInstance(HASH_FUNCTION);
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        digest.reset();
        return digest.digest(policy.toString().getBytes());
    }

    /**
     * Converts a byte array into a {@link String}
     *
     * @param data byte array to be converted
     * @return the conversion represented as a {@link String}
     */
    private static String bin2hex(byte[] data) {
        return String.format("%0" + (data.length * 2) + "X", new BigInteger(1, data));
    }

    /**
     * Creates a Policy object based on the JSON template and modifies it with the correct information
     *
     * @param is            input stream used to create JSON
     * @param publicKey     public key of the user
     * @param commandValue  value of the command
     * @param startTime     start time in millis
     * @param endTime       end time in millis
     * @param isTimeUpdated indicates if time was updated
     * @return the updated policy object
     */
    public static Policy updatePolicy(InputStream is, String publicKey, String commandValue, long startTime, long endTime, boolean isTimeUpdated) {
        // TODO: 21.2.2019. refactor this
//        ObjectMapper mapper = new ObjectMapper();
//        mapper.setPropertyNamingStrategy(PropertyNamingStrategy.SNAKE_CASE);
//        Policy policy;
//        try {
//            policy = mapper.readValue(is, Policy.class);
//            Timber.d("Policy created");
//            // replace with user's public key
//            policy.getPolicyObject().getPolicyGoc().getAttributeList().get(0).getAttributeList().get(0).setValue(publicKey);
//            // replace with selected action
//            List<AttributeList_> attributeList = policy.getPolicyObject().getPolicyGoc().getAttributeList().get(2).getAttributeList();
//            for (AttributeList_ listItem : attributeList) {
//                listItem.setValue(commandValue);
//            }
//            if (isTimeUpdated) {
//                long startTimeInSeconds = startTime / 1000;
//                long endTimeInSeconds = endTime / 1000;
//                // set start time
//                policy.getPolicyObject().getPolicyGoc().getAttributeList().get(3).getAttributeList().get(0).setValue(String.valueOf(startTimeInSeconds));
//                // set end time
//                policy.getPolicyObject().getPolicyGoc().getAttributeList().get(4).getAttributeList().get(0).setValue(String.valueOf(endTimeInSeconds));
//            }
//            policy.setPolicyId(bin2hex(getHash(policy.getPolicyObject())));
//            Timber.d("Policy updated");
//            return policy;
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
        return null;
    }

    /**
     * Converts a hex {@link String} representation to a {@link byte[]}
     *
     * @param s hex {@link String} to be converted
     * @return a {@link byte[]} representation of the hex {@link String}
     */
    public static byte[] hexStringToByteArray(String s) {
        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }
}
