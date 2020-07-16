/*
 * This file is part of the DAC distribution
 * (https://github.com/iotaledger/access)
 *
 * Copyright (c) 2020 IOTA Foundation
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
 * \project IOTA Access
 * \file vehicle_datasharing_dataset.c
 * \brief
 * Definitions of datasets which can be shared via policy action response.
 *
 * @Author Djordje Golubovic
 *
 * \notes
 *
 * \history
 * 04.03.2020. Initial version.
 ****************************************************************************/
#include "vehicle_datasharing_dataset.h"

char vehicledataset01_options[] =
    "DoorDrvrSts"
    "|DoorDrvrReSts";

char vehicledataset02_options[] = "";

char vehicledataset03_options[] = "";

char vehicledataset04_options[] = "";

char vehicledataset05_options[] = "";

char vehicledataset06_options[] = "";

char vehicledataset07_options[] = "";

char vehicledataset08_options[] = "";

char vehicledataset09_options[] = "";

char vehicledataset10_options[] = "";

char vehicledataset11_options[] = "";

char vehicledataset12_options[] = "";

char vehicledataset13_options[] = "";

char vehicledataset14_options[] = "";

char vehicledataset15_options[] = "";

char vehicledataset16_options[] = "";

char* dataset_options[DATASET_ARRAY_SIZE] = {
    vehicledataset01_options, vehicledataset02_options, vehicledataset03_options, vehicledataset04_options,
    vehicledataset05_options, vehicledataset06_options, vehicledataset07_options, vehicledataset08_options,
    vehicledataset09_options, vehicledataset10_options, vehicledataset11_options, vehicledataset12_options,
    vehicledataset13_options, vehicledataset14_options, vehicledataset15_options, vehicledataset16_options};
