def s_ICE1PCS01_parm(dict1):

    R_series = float(dict1['R_series'])
    C_series = float(dict1['C_series'])
    C_parallel = float(dict1['C_parallel'])
    fc = float(dict1['fc'])
    KFQ = float(dict1['KFQ'])

    a1_filter = R_series*C_series
    b2_filter = R_series*C_series*C_parallel
    b1_filter = (C_series + C_parallel)

    dt_clock = 0.001/fc
    c1 = KFQ*fc

    dict1['a1_filter'] = '%11.4e' % (a1_filter)
    dict1['b2_filter'] = '%11.4e' % (b2_filter)
    dict1['b1_filter'] = '%11.4e' % (b1_filter)
    dict1['dt_clock'] = '%11.4e' % (dt_clock)
    dict1['c1'] = '%11.4e' % (c1)

    print('s_ICE1PCS01: parameters computed')
