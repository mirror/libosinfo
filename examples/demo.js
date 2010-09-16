#!/usr/bin/seed

const osinfo = imports.gi.Libosinfo;

var loader = new osinfo.Loader()
loader.process_path("../data/libosinfo-dummy-data.xml")

var db = loader.get_db()

var devs = db.get_device_list()

print ("All device IDs")
for (idx in devs.get_elements()) {
    var dev = devs.get_nth(idx)
    print ("  Device " + dev.get_id())
}

var names = db.unique_values_for_property_in_device("name")

print ("All device names")
for (idx in names) {
    var name = names[idx]
    print ("  Name " + name)
}

var osnames = db.unique_values_for_property_in_os("short-id")

print ("All OS short ids")
for (idx in osnames) {
    var name = osnames[idx]

    print ("  OS short id " + name)
}

var osid = "http://fedoraproject.org/fedora-11"
var hvid = "http://qemu.org/qemu-kvm-0.11.0"
var drvclass = "network"
//var drvclass = "audio"

var os = db.get_os(osid)
var hv = db.get_platform(hvid)

var dep = db.find_deployment(os, hv)

var filter = new osinfo.Filter()
filter.add_constraint("class", drvclass)
var link = dep.get_preferred_device_link(new osinfo.DeviceLinkFilter({target_filter: filter}))
var dev = link.get_target()
var drv = link.get_param_value("driver")
print ("For OS '" + os.get_name() + "' " +
       "fruit '" + os.get_param_value("x-fruit") + "' " +
       "zoo '" + os.get_param_values("x-animal") + "' " +
       "with HV '" + hv.get_name() + "' " +
       "for class '" + drvclass + "' " +
       "use device '" + dev.get_name() + "' " +
       "with HV driver '" + link.get_driver() + "'")

