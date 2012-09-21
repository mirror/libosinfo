#!/usr/bin/python

from gi.repository import Libosinfo as osinfo;

loader = osinfo.Loader()
loader.process_path("./")

db = loader.get_db()

devs = db.get_device_list()

print "All device IDs"
for dev in devs.get_elements():
    print ("  Device " + dev.get_id())

names = db.unique_values_for_property_in_device("name")

print "All device names"
for name in names:
    print ("  Name " + name)

osnames = db.unique_values_for_property_in_os("short-id")
osnames.sort()

print "All OS short IDs"
for name in osnames:
    print ("  OS short id " + name)

hvnames = db.unique_values_for_property_in_platform("short-id")
hvnames.sort()

print "All HV short IDs"
for name in hvnames:
    print ("  HV short id " + name)

osid = "http://fedoraproject.org/fedora-11"
hvid = "http://qemu.org/qemu-kvm-0.11.0"
drvclass = "net"
#drvclass = "audio"

os = db.get_os(osid)
hv = db.get_platform(hvid)

dep = db.find_deployment(os, hv)

fltr = osinfo.Filter()
fltr.add_constraint("class", drvclass)
link = dep.get_preferred_device_link(osinfo.DeviceLinkFilter(target_filter = fltr))
dev = link.get_target()
animals = os.get_param_value_list("x-animal")
zoo = str.join(",", animals)
print ("For OS '" + os.get_name() + "' " +
       "fruit '" + os.get_param_value("x-fruit") + "' " +
       "zoo '" + zoo + "' " +
       "with HV '" + hv.get_name() + "' " +
       "for class '" + drvclass + "' " +
       "use device '" + dev.get_name() + "' " +
       "with HV driver '" + link.get_driver() + "'")
