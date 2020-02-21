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

/****************************************************************************
 * \project Decentralized Access Control
 * \file pep.h
 * \brief
 * Implementation of Policy Enforcement Point
 *
 * @Author Dejan Nedic
 *
 * \notes
 *
 * \history
 * 25.09.2018. Initial version.
 ****************************************************************************/

#include "parson.h"

/**
 * @fn      int pep_request_access(JSON_Value *request)
 *
 * @brief   Function that computes decision for PEP
 *
 * @param   request         request JSON containing request uri and request object
 *
 * @return  DENY (0), GRANT (1)
 */
int pep_request_access(char *request);
