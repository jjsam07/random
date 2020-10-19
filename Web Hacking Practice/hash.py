#!/usr/bin/env python
import hashlib

m = hashlib.md5()
m.update("a")
print m.hexdigest()