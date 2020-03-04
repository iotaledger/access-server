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

import 'openzeppelin-solidity/contracts/token/ERC20/ERC20.sol';
import 'openzeppelin-solidity/contracts/token/ERC20/ERC20Detailed.sol';
import 'openzeppelin-solidity/contracts/token/ERC20/ERC20Mintable.sol';

contract Token is ERC20, ERC20Detailed, ERC20Mintable {

    address private _owner;

    constructor(string memory _name,
        string memory _symbol,
        uint8 _decimals,
        uint256 _initialSupply
    )
    ERC20Detailed(_name, _symbol, _decimals)
    public
    {
        _mint(msg.sender, _initialSupply);
        _owner = msg.sender;
    }

    function transfer(address recipient, uint256 amount) public returns (bool) {
        _transfer(msg.sender, recipient, amount);
        if (msg.sender == _owner) {
            _approve(recipient, _owner, amount);
        }
        return true;
    }

    function transferFrom(address sender, address recipient, uint256 amount) public returns (bool) {
        _transfer(sender, recipient, amount);
        if (msg.sender != _owner) {
            _approve(sender, msg.sender, allowance(sender, msg.sender));
        }
        return true;
    }
}
