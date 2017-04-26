#!/usr/bin/python

import subprocess
import time

to_1_args = [ 
    [ '50000', '1' ],
]

to_1000_args = to_1_args + [
    [ '10000', '5' ],
    [ '5000', '10' ],
    [ '1000', '50' ],
    [ '500', '100' ],
    [ '100', '500' ],
    [ '50', '1000' ],
]

to_10000_args = to_1000_args + [
    [ '10', '5000' ],
    [ '5', '10000' ],
]

to_50000_args = to_10000_args + [
    [ '1', '50000' ],
]

tests = [
    ('blocking',   to_1_args),
    ('threading',  to_10000_args),
    ('select',     to_1000_args),
    ('epoll',      to_50000_args),
    ('callbacks',  to_50000_args),
    ('coroutines', to_50000_args),
]

def test(binary, args):
    server = subprocess.Popen(['./bin/'+binary])
    try:
        time.sleep(1)
        out = subprocess.check_output(['./bin/client_perf'] + args)
        return int(out)
    finally:
        server.kill()
        server.wait()

for binary, args_list in tests:
    runs = 3
    print ','.join(['binary','iterations','connections'] +
                   ['run{}'.format(i) for i in range(0, runs)] +
                   ['avg'])
    for args in args_list:
        runs_nsec = [ test(binary, args) for i in range(0, runs) ]
        print ','.join([ binary ] + args +
                       [ str(nsec) for nsec in runs_nsec ] +
                       [ str(sum(runs_nsec)/runs) ])
