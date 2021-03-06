#
# Copyright (c) 2019-2020, University of Southampton and Contributors.
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#!/usr/bin/env python3

import sys
import numpy as np

if (len(sys.argv) < 2):
  print("Usage: generate-dvdb-table <dvdb>")
  exit(1)

dvdb = float(sys.argv[1])
bytesToSave = 32*(1+np.arange(128))
lsbPerVolt = 1024
vdrop = dvdb*bytesToSave*lsbPerVolt

# Emit C-array
print('// Voltage dop per byte saved/restored to/from FRAM when no energy is supplied.')
print('// Generated by generate-dvdb-table.py')
print('#include <stdint.h>')
print('const uint16_t vdrop[] = {')
for i in range(len(bytesToSave)//8):
  print(','.join(str(int(vdrop[i*8+j])>>2) for j in range(8)) + ',')
print('};')

#print(','.join(str(int(e)) for e in energy))
