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
    public static readonly UNSUPPORTED_COMMAND: string = 'unsupported command';
    public static readonly MISSING_BODY: string = 'missing body';
    public static readonly MISSING_COMMAND: string = 'missing command';
    public static readonly MISSING_POLICY_ID_INSIDE_POLICY: string = 'missing policy_id inside policy';
    public static readonly MISSING_POLICY_ID: string = 'missing policyId';
    public static readonly MISSING_POLICY_STORE_ID: string = 'missing policyStoreId';
    public static readonly MISSING_POLICY: string = 'missing policy';
    public static readonly MISSING_OWNER: string = 'missing owner';
    public static readonly MISSING_DEVICE_ID: string = 'missing deviceId';
    public static readonly POLICY_ALREADY_EXIST: string = 'policy already exist';
    public static readonly POLICY_ADDED_SUCCESSFULLY: string = 'Policy added successfully';
    public static readonly POLICY_STORE_EMPTY: string = 'Policy store empty';
    public static readonly POLICY_NOT_FOUND: string = 'Policy not found';
    public static readonly POLICY_STORE_NOT_CHANGED: string = 'Policy store not changed';
    public static readonly DELETING_ALL_POLICIES: string = 'Deleting all policies';
    public static readonly UNABLE_TO_CONNECT_TO_IRI_NODE: string = 'Unable to connection to IRI node';
    public static readonly UNABLE_TO_ADD_POLICY: string = 'Unable to add policy';
    public static readonly UNABLE_TO_GET_POLICY: string = 'Unable to get policy';
    public static readonly UNABLE_TO_DELETE_POLICIES: string = 'Unable to delete policies';
    public static readonly UNABLE_TO_GET_POLICIES: string = 'Unable to get policies';
    public static readonly OK: string = 'ok';
}
