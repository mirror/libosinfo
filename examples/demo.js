#!/usr/bin/seed

const osinfo = imports.gi.Libosinfo;

var loader = new osinfo.Loader()
loader.process_path("../data/libosinfo-dummy-data.xml")

var db = loader.get_db()

var devs = db.get_device_list()

print (devs)

for (idx in devs.get_elements()) {
    var dev = devs.get_nth(idx)
    print ("Device " + dev.get_id())
}

var names = db.unique_values_for_property_in_dev("name")

for (idx in names) {
    var name = names[idx]
    print ("Name " + name)
}

var osnames = db.unique_values_for_property_in_os("short-id")

for (idx in osnames) {
    var name = osnames[idx]

    print ("OS short id " + name)
}