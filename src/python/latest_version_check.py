#!/usr/bin/python
# licensed to the apache software foundation (asf) under one or more
# contributor license agreements.  see the notice file distributed with
# this work for additional information regarding copyright ownership.
# the asf licenses this file to you under the apache license, version 2.0
# (the "license"); you may not use this file except in compliance with
# the license.  you may obtain a copy of the license at
#
#     http://www.apache.org/licenses/license-2.0
#
# unless required by applicable law or agreed to in writing, software
# distributed under the license is distributed on an "as is" basis,
# without warranties or conditions of any kind, either express or implied.
# see the license for the specific language governing permissions and
# limitations under the license.

import urllib2
from pkg_resources import parse_version

try:
    import simplejson as json
except:
    import json

from raxmon_cli import __version__
from raxmon_cli.common import run_action

URL = 'http://pypi.python.org/pypi/rackspace-monitoring-cli/json'

options = []
required_options = []

def callback(driver, options, args, callback):
    req = urllib2.Request(url=URL)
    f = urllib2.urlopen(req)
    data = json.loads(f.read())
    latest_version = data['info']['version']

    print('Current version: %s' % (__version__))
    print('Latest version: %s\n' % (latest_version))

    if (parse_version(__version__) >= parse_version(latest_version)):
        print 'Your version is up to date!'
    else:
        print('Newer version is available. You can upgrade your version by' +
              ' running the following command:')
        print('sudo pip install --upgrade rackspace-monitoring-cli')
    callback(None)

run_action(options, required_options, 'version', 'check', callback)
