def harmonic_elimination_2_parm(dict1):
    a2 = float(dict1['a2'])
    a3 = float(dict1['a3'])

    a4 = 180.0 - a3
    a5 = 180.0 - a2
    a6 = 180.0
    a7 = 180.0 + a2
    a8 = 180.0 + a3
    a9 = 360.0 - a3
    a10 = 360.0 - a2

    dict1['a4'] = '%11.4e' % (a4)
    dict1['a5'] = '%11.4e' % (a5)
    dict1['a6'] = '%11.4e' % (a6)
    dict1['a7'] = '%11.4e' % (a7)
    dict1['a8'] = '%11.4e' % (a8)
    dict1['a9'] = '%11.4e' % (a9)
    dict1['a10'] = '%11.4e' % (a10)
    print('harmonic_elimination_2_parm: parameters computed')
