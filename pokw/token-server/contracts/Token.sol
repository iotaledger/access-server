//
//  This file is part of the DAC distribution (https://github.com/xainag/frost)
//  Copyright (c) 2019 XAIN AG.
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 3.
//
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program. If not, see <http://www.gnu.org/licenses/>.
//

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
