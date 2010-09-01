#!/usr/bin/python

import gobject
from gi.repository import Libosinfo as osinfo;

loader = osinfo.Loader()
loader.process_path("../data/libosinfo-dummy-data.xml")

db = loader.get_db()

devs = db.get_device_list()

print devs

for dev in devs.get_elements():
    print ("Device " + dev.get_id())

names = db.unique_values_for_property_in_dev("name")

for name in names:
    print ("Name " + name)

osnames = db.unique_values_for_property_in_os("short-id")

for name in osnames:
    print ("OS short id " + name)
