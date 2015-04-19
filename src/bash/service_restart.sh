#!/bin/bash
# Create a `service foobar start|stop|restart` function
service() {
	/etc/init.d/$1 $2
}
