/*
* This file is part of the Frost distribution
* (https://github.com/xainag/frost)
*
* Copyright (c) 2019 XAIN AG.
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

pragma solidity ^0.5.0;

contract PolicyStore {

  uint public lastUpdate;

  struct Policy {
    string owner;
    string device;
    string policyDefinition;
    uint256 lastUpdate;
  }

  mapping (string => Policy) policies;

  //device => Policy ID array
  mapping (string => string[]) devicePolicyIDs;

  Policy EMPTY_POLICY = (
    Policy({
        owner: "",
        device: "",
        policyDefinition: "",
        lastUpdate: 0
      }));

  modifier policyExists(string memory _ID) {
    require(policies[_ID].lastUpdate > 0, "Policy Does Not Exist");
    _;
  }

  modifier policyNotExists(string memory _ID) {
    require(policies[_ID].lastUpdate == 0, "Policy Does Exist");
    _;
  }

  event PolicyCreation(string);
  event PolicyUpdate(string);
  event PolicyDelete(string);

  constructor() public {
    lastUpdate = now;
  }

  function getPolicyByID(string memory _ID)
    public view
    returns (string memory, string memory, string memory) {

    return (
      policies[_ID].owner,
      policies[_ID].device,
      policies[_ID].policyDefinition
      );
  }

  function getPolicyCountForDevice(string memory _device)
    public view returns (uint256) {

    return devicePolicyIDs[_device].length;
  }

  function getPolicyIDForDevice(
    string memory _device,
    uint256 _index)
    public view returns (string memory) {

    return devicePolicyIDs[_device][_index];
  }

  function createPolicy(
    string memory _ID,
    string memory _owner,
    string memory _device,
    string memory _definition)
    public policyNotExists(_ID) {

    policies[_ID] = Policy({
      owner : _owner,
      device: _device,
      policyDefinition: _definition,
      lastUpdate : now});
    devicePolicyIDs[_device].push(_ID);
    lastUpdate = now;
    emit PolicyCreation(_ID);
  }

  function updatePolicy(
    string memory _ID,
    string memory _owner,
    string memory _device,
    string memory _definition)
    public policyExists(_ID) {

    policies[_ID] = Policy({
      owner : _owner,
      device: _device,
      policyDefinition: _definition,
      lastUpdate : now});
    lastUpdate = now;
    emit PolicyUpdate(_ID);
  }

  function deleteAllPoliciesForDevice(
    string memory _device)
    public
  {
    uint arrayLen = devicePolicyIDs[_device].length;
    for (uint i = 0; i < arrayLen; i++) {
      string memory ID = devicePolicyIDs[_device][i];
      delete policies[ID];
      emit PolicyDelete(ID);
    }
    if (arrayLen > 0) {
      delete devicePolicyIDs[_device];
    }
  }

  function deletePolicy(
    string memory _device,
    string memory _ID)
    public policyExists(_ID)
    returns (bool)
  {
    uint arrayLen = devicePolicyIDs[_device].length;
    uint delIndex = arrayLen;
    for (uint ii = 0; ii < arrayLen; ii++){
        if (keccak256(bytes(devicePolicyIDs[_device][ii])) == keccak256(bytes(_ID))) {
           delIndex = ii;
           devicePolicyIDs[_device][delIndex] = devicePolicyIDs[_device][arrayLen-1];
           devicePolicyIDs[_device].length--;
           policies[_ID] = EMPTY_POLICY;
           lastUpdate = now;
           emit PolicyDelete(_ID);
           return true;
        }
    }
    return false;
  }
}
