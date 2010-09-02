#!/usr/bin/python

import gobject
from gi.repository import Libosinfo as osinfo;

loader = osinfo.Loader()
loader.process_path("../data/libosinfo-dummy-data.xml")

db = loader.get_db()

devs = db.get_device_list()

print devs

print "All device IDs"
for dev in devs.get_elements():
    print ("  Device " + dev.get_id())

names = db.unique_values_for_property_in_dev("name")

print "All device names"
for name in names:
    print ("  Name " + name)

osnames = db.unique_values_for_property_in_os("short-id")

print "All OS short IDs"
for name in osnames:
    print ("  OS short id " + name)

osid = "http://fedoraproject.org/fedora-11"
hvid = "http://qemu.org/qemu-kvm-0.11.0"
drvclass = "network"
#drvclass = "audio"

os = db.get_os(osid)
hv = db.get_platform(hvid)

fltr = osinfo.Filter()
fltr.add_constraint("class", drvclass)
drvlink = os.get_preferred_device_link(hv, fltr)
drv = drvlink.get_target()
print ("For OS '" + os.get_param_value("name") + "' " +
       "with HV '" + hv.get_param_value("name") + "' " +
       "for class '" + drvclass + "' " +
       "use device '" + drv.get_param_value("name") + "' " +
       "with HV driver '" + drvlink.get_param_value("driver") + "'")
